type filename = string;

type fromHost =
  | CommFileUpdate(filename, string)
  | CommSetEntry(filename);

type toHost =
  | CommReady
  | CommSendLog(ConsoleFeed.logEncoded)
  | CommEvalStart
  | CommEvalFinished;

[@bs.val] external window: Dom.window = "window";

type message_event('message) = {. "data": message_event_data('message)}
and message_event_data('message) = {
  .
  "_type": Js.Nullable.t(string),
  "payload": 'message,
};

type message_handler('message) = message_event('message) => unit;

[@bs.send]
external addMessageListener:
  (Dom.window, [@bs.as "message"] _, message_handler('message), bool) => unit =
  "addEventListener";

let toHost: toHost => unit = [%raw
  message => {
    {|
  window.top.postMessage({
    "type": "bs-repl",
    "payload": message
  }, "*")
 |};
  }
];

let toFrame: (Dom.element, fromHost) => unit = [%raw
  (iframe, message) => {
    {|
    console.log("sending", message);
  iframe.contentWindow.postMessage({
    "type": "bs-repl",
    "payload": message
  }, "*")
 |};
  }
];
