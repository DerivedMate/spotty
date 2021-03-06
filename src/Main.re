[%raw "require('isomorphic-fetch')"];
open Utils;
Dotenv.config();

let run = (playlist_url, output_dir) => {
  open Belt.Option; 

  let id =
    Js.Re.exec_(Regex.playlist_id, playlist_url)
    ->flatMap(res => res->Js.Re.captures->Belt.Array.get(1))
    ->flatMap(Js.Nullable.toOption)
    ->getExn; // Justification: I'd be impossible to continue, should the id not be found

  Spotify.fetch_access_token()
  >>= Spotify.fetch_playlist(_, id)
  >>= Youtube.get_playlist
  >>- Future.flatMap(
        _,
        sync_future_consume(_, t => {
          Dl.get_format(t.url) 
          ||- Dl.download(output_dir, t)
        }),
      );
};

let main = () => {
  switch (Node_process.argv) {
  | [|_node, _script, url, output|] => run(url, output)
  | _ =>
    "Usage: spotty <playlist_url> <output_dir_path>"
    -> Js.Exn.raiseError
    -> raise
  };
};

main()
