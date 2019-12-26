module Position = {
  type t = {
    line: int,
    col: int,
  };
};

module Loc = {
  type t = {
    loc_start: Position.t,
    loc_end: Position.t,
  };
};

type error = {
  loc: option(Loc.t),
  message: string,
};

type success = {
  code: string,
  warnings: string,
};

type result = Belt.Result.t(success, error);

module InternalResult = {
  type t = {. "_type": Js.nullable(string)};

  type internalSuccess = {. "js_code": string};

  type internalError = {
    .
    "js_error_msg": string,
    "row": int,
    "column": int,
    "endRow": int,
    "endColumn": int,
    "text": string,
  };

  external unsafeFromJson: 'a => t = "%identity";
  external unsafeAsSuccess: t => internalSuccess = "%identity";
  external unsafeAsError: t => internalError = "%identity";

  let toResult = jsObj =>
    switch (Js.Nullable.toOption(jsObj##_type)) {
    | Some("error") =>
      let error = jsObj |> unsafeAsError;
      Result.Error({
        message: error##text,
        loc:
          Some({
            loc_start: {
              line: error##row,
              col: error##column,
            },
            loc_end: {
              line: error##endRow,
              col: error##endColumn,
            },
          }),
      });
    | _ =>
      Result.Ok({
        code: (jsObj |> unsafeAsSuccess)##js_code,
        warnings: "",
      })
    };
};

[@bs.val] [@bs.scope "ocaml"]
external ocamlCompile: string => string = "compile";
[@bs.val] [@bs.scope "reason"]
external reasonCompile: string => string = "compile";

let compile = (compiler, code): result => {
  let json = compiler(code);

  json
  |> InternalResult.unsafeFromJson
  |> InternalResult.toResult
  |> (
    fun
    | Ok({code}) => Ok({code, warnings: ""})
    | Error(error) => Error(error)
  );
};

let ocaml = compile(ocamlCompile);
let reason = compile(reasonCompile);
