// module LocalStorage = {
//   open Dom.Storage2;

//   let set = (value, key) => localStorage->setItem(key, value);
//   let get = key => localStorage->getItem(key);

//   let getWithDefault = (key, default) =>
//     switch (get(key)) {
//     | None => default
//     | Some(value) => value
//     };

//   let mapWithDefault = (key, default, fn) =>
//     switch (get(key)) {
//     | None => default
//     | Some(value) =>
//       try(fn(value)) {
//       | _ => default
//       }
//     };
// };

// let data = ref([|""|]);

// let idx = ref(0);

// let setup = () =>
//   let s = get_storage();
//   switch (Js.Opt.to_option(s##getItem(Js.string("history")))) {
//   | None => raise(Not_found)
//   | Some(s) =>
//     let a = Json.unsafe_input(s);
//     data := a;
//     idx := Array.length(a) - 1;
//   };
//   }) {
//   | _ => ()
//   };

// let push = text => {
//   let l = Array.length(data^);
//   let n = Array.make(l + 1, "");
//   data^[l - 1] = text;
//   Array.blit(data^, 0, n, 0, l);
//   data := n;
//   idx := l;
//   try({
//     let s = get_storage();
//     let str = Json.output(data^);
//     s##setItem(Js.string("history"), str);
//   }) {
//   | Not_found => ()
//   };
// };

// let current = text => data^[idx^] = text;

// let previous = textbox =>
//   if (idx^ > 0) {
//     decr(idx);
//     textbox##.value := Js.string(data^[idx^]);
//   };

// let next = textbox =>
//   if (idx^ < Array.length(data^) - 1) {
//     incr(idx);
//     textbox##.value := Js.string(data^[idx^]);
//   };
