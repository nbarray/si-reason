/* Module called BoardView to avoid conflicting filename with the board logic module */
[@bs.val] external requestAnimationFrame : (unit => unit) => unit = "";

[@bs.val] external getElementById : string => Dom.element =
  "document.getElementById";

let columnKey = (x, y) => {j|$x-$y|j};

let columnBaseTransform = ((x, y)) =>
  "translate("
  ++ (string_of_float(x) ++ ("px, " ++ (string_of_float(y) ++ "px)")));

let emptyColumnPositions =
  (0., 0.) |> Array.make_matrix(Board.numRows, Board.numRows);

type state = {columnPositions: array(array((float, float)))};

type actions =
  | UpdateColumnPositions;

let component = ReasonReact.reducerComponent("Board");

let make = (~rotation, ~board, ~move, _children) => {
  ...component,
  initialState: () => {columnPositions: emptyColumnPositions},
  reducer: (action, _) =>
    switch action {
    | UpdateColumnPositions =>
      ReasonReact.Update({
        columnPositions:
          emptyColumnPositions
          |> Array.mapi(
               (x, row) =>
                 row
                 |> Array.mapi(
                      (y, _) => {
                        let rect =
                          ReactDOMRe.domElementToObj(
                            getElementById(BoardBase.markerId(x, y))
                          )##getBoundingClientRect
                            ();
                        (rect##left, rect##top)
                      }
                    )
             )
      })
    },
  didMount: ({reduce}) => {
    let rec onAnimationFrame = () => {
      reduce((_) => UpdateColumnPositions, ());
      requestAnimationFrame(onAnimationFrame)
    };
    requestAnimationFrame(onAnimationFrame);
    ReasonReact.NoUpdate
  },
  render: ({state: {columnPositions}}) =>
    <div className="Board">
      <BoardBase
        rotation
        winning=(
          switch (Board.winner(board)) {
          | Some(_) => true
          | None => false
          }
        )
      />
      (
        Board.ijList
        /* Figure out order in render perspective */
        |> List.sort(
             ((x1, y1), (x2, y2)) => {
               let (_, yPx1) = columnPositions[x1][y1];
               let (_, yPx2) = columnPositions[x2][y2];
               int_of_float(yPx1) - int_of_float(yPx2)
             }
           )
        |> List.mapi((i, (x, y)) => (i, (x, y)))
        /* Put columns back in stable order, otherwise they rerender everytime */
        |> List.sort(
             ((_, (x1, y1)), (_, (x2, y2))) => x1 == x2 ? y1 - y2 : x1 - x2
           )
        |> List.map(
             ((i, (x, y))) =>
               <div
                 className="Board-columnContainer"
                 key=(columnKey(x, y))
                 style=(
                   ReactDOMRe.Style.make(
                     ~transform=columnBaseTransform(columnPositions[x][y]),
                     ~opacity=
                       string_of_float(float_of_int(i) /. 16. *. 0.5 +. 0.5),
                     ~zIndex=string_of_int(i),
                     ()
                   )
                 )>
                 <Column
                   board
                   x
                   y
                   tryMove=(
                     (_) =>
                       if (Board.isValidMove((x, y), board)) {
                         move((x, y))
                       }
                   )
                   winner=(Board.winner(board))
                 />
               </div>
           )
        |> Array.of_list
        |> ReasonReact.arrayToElement
      )
    </div>
};
