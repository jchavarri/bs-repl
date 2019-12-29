open ContainerComm;

let indexFileName = "index.re"
let entryFile = ref(indexFileName);
let polestar = ref(None);

let clearPolestar = () => {
  polestar := None;
};

let getPolestar = () => {
  switch (polestar^) {
  | None =>
    ContainerBundler.makePolestar(
      ~onError=
        exn => {
          // %log.debug
          // "Clear polestar instance";
          Js.log(exn);
          clearPolestar();
        },
      ~onEntry=() => toHost(CommEvalFinished),
    )
  | Some(polestar) => polestar
  };
};

window->addMessageListener(
  event => {
    let data = event##data;
    switch (data##_type->Js.Nullable.toOption) {
    | Some("bs-repl") =>
      switch (data##payload) {
      | ContainerComm.CommFileUpdate(filename, content) =>
        ContainerFs.set(~filename, ~content);
        let entryContent = ContainerFs.get(~filename=entryFile^);
        switch (entryContent) {
        | None => ()
        | Some(entryContent) =>
          toHost(CommEvalStart);
          Js.log(ContainerBundler.eval(getPolestar(), entryContent));
        };

      | CommSetEntry(filename) => entryFile := filename
      };
    | _ => ()
    };
  },
  false,
);

toHost(CommReady);

ConsoleFeed.hook([%raw "window.console"], log => toHost(CommSendLog(log)));

%bs.raw
{|
if (module.hot) {
  module.hot.accept("./Container.bs.js", () => {
    location.reload();
  });
}
|};
