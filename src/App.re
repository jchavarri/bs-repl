type history = (list(string), list(string)); /* Use a tuple of two lists to keep elements in second list when arrow up is pressed */
type source =
  | Textarea
  | History(string);
type pastCommand = {
  input: string,
  result: Bs.result,
};
type state = {
  currentInput: string,
  history,
  source,
  executed: list(pastCommand),
};

type action =
  | InputChanged(string)
  | KeyPressed(Events.Keyboard.key);

let removeTopExtraOutput = jsCode => {
  jsCode
  ->Js.String2.replaceByRe([%re {|/^\/\/ Generated.*\n/|}], "")
  ->Js.String2.replaceByRe([%re {|/^'use strict';/|}], "");
};
let removeBottomExtraOutput = jsCode => {
  let lastDoubleNewLine = jsCode->Js.String2.lastIndexOf("\n\n");
  jsCode->Js.String2.substring(~from=0, ~to_=lastDoubleNewLine);
};

let reducer = (state, action) => {
  switch (action) {
  | InputChanged(currentInput) => {...state, currentInput, source: Textarea}
  | KeyPressed(key) =>
    switch (key) {
    | ArrowUp =>
      let (head, tail) = state.history;
      switch (head) {
      | [] => {...state, source: Textarea}
      | [hd, ...rest] => {
          ...state,
          history: (rest, [hd, ...tail]),
          source: History(hd),
        }
      };
    | ArrowDown =>
      let (head, tail) = state.history;
      switch (tail, state.source) {
      | ([], _) => {...state, source: Textarea}
      | ([hd, ..._], Textarea) => {...state, source: History(hd)}
      | ([hd, ...rest], History(_)) => {
          ...state,
          history: ([hd, ...head], rest),
          source:
            switch (rest) {
            | [] => Textarea
            | [hd, ..._] => History(hd)
            },
        }
      };
    | Enter =>
      switch (state.source, state.currentInput->Js.String2.trim) {
      | (Textarea, input) when input === "" => state
      | (Textarea, input)
      | (History(input), _) =>
        let (head, tail) = state.history;
        let history = List.concat(tail->List.reverse, head);
        let result =
          switch (Bs.reason(input)) {
          | Ok({code, warnings}) =>
            let code =
              code
              ->removeTopExtraOutput
              ->removeBottomExtraOutput
              ->Js.String2.trim
              ->Js.String2.replaceByRe([%re {|/\n+/g|}], "\n");
            Ok({Bs.code, warnings});
          | Error(_) as r => r
          };
        {
          currentInput: "",
          history: ([input, ...history], []),
          source: Textarea,
          executed: [{input, result}, ...state.executed],
        };
      }
    | _ => state
    }
  };
};

let s = React.string;

[@react.component]
let make = () => {
  let (state, dispatch) =
    React.useReducer(
      reducer,
      {currentInput: "", history: ([], []), source: Textarea, executed: []},
    );
  <>
    <div id="toplevel-container">
      <pre id="output">
        <div>
          {state.executed
           ->List.reverse
           ->List.mapWithIndex((i, {input, result}) => {
               <React.Fragment key={string_of_int(i)}>
                 <div className="sharp"> input->s </div>
                 {switch (result) {
                  | Ok({code}) =>
                    <div className="caml"> <span> code->s </span> </div>
                  | Error({message}) =>
                    <div className="stderr"> <span> message->s </span> </div>
                  }}
               </React.Fragment>
             })
           ->List.toArray
           ->React.array}
          <div id="sharp" className="sharp" />
          <textarea
            id="userinput"
            onChange={e =>
              dispatch @@ InputChanged(Events.valueFromEvent(e))
            }
            onKeyDown={event => {
              let key =
                event
                ->ReactEvent.Keyboard.key
                ->Events.Keyboard.keyFromEventKey;
              switch (key) {
              | Enter
              | ArrowUp
              | ArrowDown =>
                event->ReactEvent.Synthetic.preventDefault;
                dispatch @@ KeyPressed(key);
              | _ => ()
              };
            }}
            value={
              switch (state.source, state.currentInput) {
              | (Textarea, input)
              | (History(input), _) => input
              }
            }
          />
          <button type_="button" className="btn btn-default" id="btn-share">
            "Share"->s
          </button>
        </div>
      </pre>
    </div>
    // <div id="toplevel-side">
    //   <h3> "BuckleScript + Reason"->s </h3>
    //   <h4> "OCaml with JavaScript syntax compiled to JavaScript."->s </h4>
    //   <p>
    //     "BuckleScript is easy to install, compiles really fast, and produces
    //     readable and optimized JavaScript output code.
    //     Reason is a syntax that quickly becomes familiar for JavaScript
    //     developers.
    //     Give them a try!"
    //     ->s
    //   </p>
    //   <p>
    //     "This web-based OCaml toplevel is compiled using Js_of_ocaml."->s
    //   </p>
    //   <h4> "Command"->s </h4>
    //   <table className="table table-striped table-condensed">
    //     <tbody>
    //       <tr> <td> "Enter/Return"->s </td> <td> "Submit code"->s </td> </tr>
    //       <tr> <td> "Ctrl + Enter"->s </td> <td> "Newline"->s </td> </tr>
    //       <tr> <td> "Up / Down"->s </td> <td> "Browse history"->s </td> </tr>
    //       <tr> <td> "Ctrl + l"->s </td> <td> "Clear display"->s </td> </tr>
    //       <tr> <td> "Ctrl + k"->s </td> <td> "Reset toplevel"->s </td> </tr>
    //       <tr> <td> "Tab"->s </td> <td> "Indent code"->s </td> </tr>
    //     </tbody>
    //   </table>
    //   <h4> "Try to execute samples"->s </h4>
    //   <div id="toplevel-examples" className="list-group" />
    //   <canvas width="200" height="200" id="test-canvas" />
    //   <h4 className="clear"> "See the generated javascript code"->s </h4>
    //   <pre id="last-js" />
    // </div>
  </>;
};
