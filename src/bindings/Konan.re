[@bs.module]
external parse:
  string =>
  {
    .
    "expressions": array(string),
    "strings": array(string),
  } =
  "konan";
