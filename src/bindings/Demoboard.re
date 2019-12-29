type options = {
  .
  // "dependencies": Js.Dict.t(string),
  "url": string,
};

[@bs.module "@frontarm/demoboard-worker/src/fetchDependency.ts"]
  external fetchDependency: (options) => Js.Promise.t(Polestar.Fetcher.FetchResult.t) = "fetchDependency";

let fetchFromNpm = (~url, ~meta as _, ~pathname as _) => {
  fetchDependency({
    "url": url
  })
}
