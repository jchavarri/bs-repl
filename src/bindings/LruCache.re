type t;
[@bs.module "lru-cache"] [@bs.new] external make: int => t = "LRU";

[@bs.send] external get: t => option('a) = "get";
