let makePolestar:
  (~onError: exn => unit, ~onEntry: unit => unit) => Polestar.polestar;

let eval: (Polestar.polestar, string) => unit;
