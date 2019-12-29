type console;

type rawData;
type log;
type logEncoded;

[@bs.module "console-feed"]
external hook: (console, logEncoded => unit) => unit = "Hook";

[@bs.module "console-feed"] external decode: logEncoded => log = "Decode";

[@bs.module "console-feed/lib/Hook/parse/index.js"]
external parse: (string, rawData) => log = "default";

[@bs.module "console-feed"] external encode: log => logEncoded = "Encode";

module Display = {
  [@bs.module "console-feed"] [@react.component]
  external make:
    (~logs: array(log), ~variant: string=?, ~styles: Js.t('styles)=?) =>
    React.element =
    "Console";
};
