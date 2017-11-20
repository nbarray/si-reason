let component = ReasonReact.statelessComponent("Column");

let make = (~board, ~x, ~y, ~tryMove, ~winner, _children) => {
  ...component,
  render: (_self) =>
    <div
      className=(
        Board.isValidMove((x, y), board) ? "Column Column--canMove" : "Column"
      )
      onClick=tryMove>
      (
        board
        |> Array.map((layer) => layer[x][y])
        |> Array.mapi(
             (i, el) => {
               let winning =
                 switch winner {
                 | Some((_, positions))
                     when positions |> List.exists((pos) => pos == (i, x, y)) =>
                   true
                 | _ => false
                 };
               switch el {
               | None => ReasonReact.nullElement
               | Some(Board.P1) =>
                 <Bead key=(string_of_int(i)) player=Board.P1 winning />
               | Some(Board.P2) =>
                 <Bead key=(string_of_int(i)) player=Board.P2 winning />
               }
             }
           )
        |> ReasonReact.arrayToElement
      )
    </div>
};
