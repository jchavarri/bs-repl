open Js.Promise;

exception InvalidUrl(string);

module Config = {
  let bsStdlibUrl = "";
};

module Protocol = {
  type t =
    | Npm
    | Vfs
    | Anonymous
    | Unknown(string);

  let ofString =
    fun
    | "npm:" => Npm
    | "vfs:" => Vfs
    | "polestar-anonymous:" => Anonymous
    | p => Unknown(p);
};

let globals = {
  "process": {
    "env": {
      "NODE_ENV": "production",
    },
  },
};

let parseDependencies = code => {
  let raw = Konan.parse(code);
  raw##strings;
};

let handleFilesystem = (~url, ~meta as _, ~pathname as filename) => {
  let fileContent = ContainerFs.get(~filename);

  Js.Promise.make((~resolve, ~reject: _) =>
    switch (fileContent) {
    | None =>
      reject(.
        {
          failwith("Unknown file " ++ filename);
        },
      )
    | Some(code) =>
      resolve(.
        Polestar.Fetcher.FetchResult.make(
          ~url,
          ~id=url,
          ~dependencies=Array(parseDependencies(code)),
          ~code,
          (),
        ),
      )
    }
  );
};

let handleBsStdlib = (~url, ~meta as _, ~pathname) => {
  Fetch.fetch(Config.bsStdlibUrl ++ "/" ++ pathname)
  |> then_(Fetch.Response.text)
  |> then_(code =>
       resolve(
         Polestar.Fetcher.FetchResult.make(
           ~url,
           ~id=url,
           ~dependencies=Array(parseDependencies(code)),
           ~code,
           (),
         ),
       )
     );
};

// npm://uuid@latest
// => ("npm:", "uuid@latest")
let getProtocolPathname: string => option((string, string)) =
  url => {
    url
    ->Js.String2.match([%re "/(^[a-zA-Z\\-]+\\:)(?:\\/.)(.+)/"])
    ->Belt.Option.flatMap(matches =>
        switch (matches->Belt.Array.length) {
        | 3 =>
          Some((matches->Array.getUnsafe(1), matches->Array.getUnsafe(2)))
        | _ => None
        }
      );
  };

let fetcher =
  (. url, meta: Polestar.Fetcher.meta) => {
    let parsed = getProtocolPathname(url);
    Js.log(url);

    Js.Promise.make((~resolve, ~reject) =>
      switch (parsed) {
      | None => reject(. InvalidUrl(url))
      | Some(parsed) =>
        // [%log.debug "parsed"; parsed];
        resolve(. parsed)
      }
    )
    |> Js.Promise.then_(((protocol, pathname)) =>
         switch (protocol |> Protocol.ofString) {
         | Anonymous
         | Vfs =>
           if (pathname |> Js.String.startsWith("stdlib")) {
             handleBsStdlib(~url, ~meta, ~pathname);
           } else {
             handleFilesystem(~url, ~meta, ~pathname);
           }
         | Npm =>
           if (pathname |> Js.String.startsWith("bs-platform@latest/lib/js/")) {
             let newFilesystem =
               pathname
               |> Js.String.replace("bs-platform@latest/lib/js", "stdlib");
             handleBsStdlib(~url, ~meta, ~pathname=newFilesystem);
           } else {
             Demoboard.fetchFromNpm(~url, ~meta, ~pathname);
           }
         | _ =>
           Js.Promise.make((~resolve as _, ~reject) =>
             reject(. InvalidUrl(url))
           )
         }
       );
  };

let makePolestar = (~onError, ~onEntry) =>
  Polestar.make({
    "globals": globals,
    "moduleThis": Browser.window,
    "fetcher": fetcher,
    "resolver": Polestar.defaultResolver,
    "onEntry":
      (.) => {
        onEntry();
      },
    "onError":
      (. error) => {
        onError(error);
      },
  });

let cleanDom: unit => unit = [%raw
  () => {|
  document.body.innerHTML = '<div id="root"></div>'
|}
];

let eval = (polestar, code) => {
  let dependencies = parseDependencies(code);
  cleanDom();

  let _ =
    polestar->Polestar.evaluate(dependencies, code)
    |> then_(_result => resolve());
  ();
};
