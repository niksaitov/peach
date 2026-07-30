"use client";

import { useState } from "react";
import { Chessboard } from "react-chessboard";
import { Chess, type Square, type Color } from "chess.js";
import type { GameStatus } from "./ChessGame";

const LIGHT_SQUARE = "#f0d9b5";
const DARK_SQUARE = "#b58863";

type SquareStyles = Record<string, React.CSSProperties>;

interface BoardProps {
  game: Chess;
  fen: string;
  status: GameStatus;
  allowedColor?: Color;
  onMoveComplete: (fen: string, status: GameStatus) => void;
}

function legalMoveStyles(game: Chess, square: Square): SquareStyles {
  const moves = game.moves({ square, verbose: true });
  if (!moves.length) return {};
  const styles: SquareStyles = {
    [square]: { backgroundColor: "rgba(255, 255, 0, 0.4)" },
  };
  for (const move of moves) {
    const isOccupied = !!game.get(move.to);
    styles[move.to] = isOccupied
      ? { background: "radial-gradient(circle, transparent 55%, rgba(0,0,0,0.25) 55%)", borderRadius: "50%" }
      : { background: "radial-gradient(circle, rgba(0,0,0,0.2) 28%, transparent 28%)" };
  }
  return styles;
}

function checkStyles(game: Chess): SquareStyles {
  if (!game.inCheck()) return {};
  const turn: Color = game.turn();
  for (const row of game.board()) {
    for (const cell of row) {
      if (cell?.type === "k" && cell.color === turn) {
        return { [cell.square]: { backgroundColor: "rgba(220, 30, 30, 0.6)" } };
      }
    }
  }
  return {};
}

function computeStatus(game: Chess): GameStatus {
  if (game.isCheckmate())
    return { message: `Checkmate — ${game.turn() === "w" ? "Black" : "White"} wins`, isOver: true };
  if (game.isStalemate()) return { message: "Stalemate — draw", isOver: true };
  if (game.isDraw()) return { message: "Draw", isOver: true };
  if (game.inCheck())
    return { message: `${game.turn() === "w" ? "White" : "Black"} is in check`, isOver: false };
  return null;
}

export default function Board({ game, fen, status, allowedColor, onMoveComplete }: BoardProps) {
  const [squareStyles, setSquareStyles] = useState<SquareStyles>({});
  const [selected, setSelected] = useState<Square | null>(null);

  function afterMove() {
    setSquareStyles(checkStyles(game));
    setSelected(null);
    onMoveComplete(game.fen(), computeStatus(game));
  }

  function onSquareClick({ square }: { square: string }) {
    if (status?.isOver) return;
    if (allowedColor && game.turn() !== allowedColor) return;
    const sq = square as Square;

    if (selected && selected !== sq) {
      try {
        game.move({ from: selected, to: sq, promotion: "q" });
        afterMove();
        return;
      } catch {
        // not a legal move — fall through to piece selection
      }
    }

    const piece = game.get(sq);
    if (piece && piece.color === game.turn()) {
      setSquareStyles({ ...legalMoveStyles(game, sq), ...checkStyles(game) });
      setSelected(sq);
    } else {
      setSquareStyles(checkStyles(game));
      setSelected(null);
    }
  }

  function onPieceDrop({ sourceSquare, targetSquare }: { sourceSquare: string; targetSquare: string | null }): boolean {
    if (status?.isOver || !targetSquare) return false;
    if (allowedColor && game.turn() !== allowedColor) return false;
    try {
      game.move({ from: sourceSquare as Square, to: targetSquare as Square, promotion: "q" });
    } catch {
      return false;
    }
    afterMove();
    return true;
  }

  function onPieceDragBegin({ square }: { isSparePiece: boolean; piece: unknown; square: string | null }) {
    if (!square || status?.isOver) return;
    if (allowedColor && game.turn() !== allowedColor) return;
    setSquareStyles({ ...legalMoveStyles(game, square as Square), ...checkStyles(game) });
    setSelected(square as Square);
  }

  return (
    <div style={{ width: 640 }}>
      <Chessboard
        options={{
          position: fen,
          onPieceDrop,
          onSquareClick,
          onPieceDrag: onPieceDragBegin,
          squareStyles,
          lightSquareStyle: { backgroundColor: LIGHT_SQUARE },
          darkSquareStyle: { backgroundColor: DARK_SQUARE },
          boardStyle: { width: 640 },
          allowDragging: !status?.isOver,
        }}
      />
    </div>
  );
}
