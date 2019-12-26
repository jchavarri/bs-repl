let valueFromEvent = evt => ReactEvent.Form.target(evt)##value;

module Keyboard = {
  type key =
    | ArrowUp
    | ArrowDown
    | Backspace
    | Enter
    | Escape
    | Unsupported;

  let keyFromEventKey = str =>
    switch (str) {
    | "ArrowUp" => ArrowUp
    | "ArrowDown" => ArrowDown
    | "Backspace" => Backspace
    | "Enter" => Enter
    | "Escape" => Escape
    | _ => Unsupported
    };
};
