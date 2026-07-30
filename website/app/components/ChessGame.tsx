"use client";

import { useRef, useState } from "react";
import { Chess, type Square } from "chess.js";
import Board from "./Board";
import Sidebar from "./Sidebar";
import EnginePanel from "./EnginePanel";
import PlayerInfo from "./PlayerInfo";
import { useEngine } from "../hooks/useEngine";

export type Mode = "play" | "evaluate";
export type GameStatus = { message: string; isOver: boolean } | null;

const PIECE_VALUES: Record<string, number> = { p: 1, n: 3, b: 3, r: 5, q: 9 };

function computeCaptures(game: Chess) {
  const starts: Record<string, number> = { p: 8, n: 2, b: 2, r: 2, q: 1 };
  const whitePieces: Record<string, number> = {};
  const blackPieces: Record<string, number> = {};

  for (const row of game.board()) {
    for (const cell of row) {
      if (cell && cell.type !== "k") {
        if (cell.color === "w") whitePieces[cell.type] = (whitePieces[cell.type] || 0) + 1;
        else blackPieces[cell.type] = (blackPieces[cell.type] || 0) + 1;
      }
    }
  }

  const capturedByWhite: string[] = [];
  const capturedByBlack: string[] = [];

  for (const [type, start] of Object.entries(starts)) {
    const blackMissing = start - (blackPieces[type] || 0);
    for (let i = 0; i < blackMissing; i++) capturedByWhite.push(type);
    const whiteMissing = start - (whitePieces[type] || 0);
    for (let i = 0; i < whiteMissing; i++) capturedByBlack.push(type);
  }

  return { capturedByWhite, capturedByBlack };
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

function uciToSan(game: Chess, uci: string): string | null {
  if (uci === "0000" || uci.length < 4) return null;
  const from = uci.slice(0, 2) as Square;
  const to = uci.slice(2, 4) as Square;
  const promotion = uci[4] as "q" | "r" | "b" | "n" | undefined;
  try {
    const move = game.move({ from, to, ...(promotion ? { promotion } : {}) });
    game.undo();
    return move.san;
  } catch {
    return null;
  }
}

function applyUCIMove(game: Chess, uci: string): boolean {
  if (uci === "0000" || uci.length < 4) return false;
  const from = uci.slice(0, 2) as Square;
  const to = uci.slice(2, 4) as Square;
  const promotion = uci[4] as "q" | "r" | "b" | "n" | undefined;
  try {
    game.move({ from, to, ...(promotion ? { promotion } : {}) });
    return true;
  } catch {
    return false;
  }
}

export default function ChessGame() {
  const gameRef = useRef(new Chess());
  const [fen, setFen] = useState(gameRef.current.fen());
  const [status, setStatus] = useState<GameStatus>(null);
  const [mode, setMode] = useState<Mode>("play");
  const [resetKey, setResetKey] = useState(0);
  const [names, setNames] = useState({ white: "White", black: "Black" });
  const [searchDepth, setSearchDepth] = useState(10);
  const [pvCount, setPvCount] = useState(1);
  const [analysisResult, setAnalysisResult] = useState<string | null>(null);

  const engine = useEngine();

  function triggerEnginePlay(fenStr: string) {
    engine.search(fenStr, searchDepth * 2, (uci) => {
      const game = gameRef.current;
      if (!applyUCIMove(game, uci)) return;
      setFen(game.fen());
      setStatus(computeStatus(game));
    });
  }

  function handleMoveComplete(newFen: string, newStatus: GameStatus) {
    setFen(newFen);
    setStatus(newStatus);
    setAnalysisResult(null);
    if (mode === "play" && !newStatus?.isOver && gameRef.current.turn() === "b") {
      triggerEnginePlay(newFen);
    }
  }

  function handleFenImport(fenStr: string) {
    engine.cancel();
    gameRef.current.load(fenStr);
    setFen(gameRef.current.fen());
    setStatus(null);
    setAnalysisResult(null);
    setResetKey((k) => k + 1);
  }

  function handleReset() {
    engine.cancel();
    gameRef.current.reset();
    setFen(gameRef.current.fen());
    setStatus(null);
    setAnalysisResult(null);
    setResetKey((k) => k + 1);
  }

  function handleAnalyze() {
    engine.search(fen, searchDepth * 2, (uci) => {
      const san = uciToSan(gameRef.current, uci);
      setAnalysisResult(san ?? uci);
    });
  }

  function handleResetSearch() {
    engine.cancel();
    setAnalysisResult(null);
  }

  // Recomputed each render — gameRef.current reflects current board after every setFen
  const { capturedByWhite, capturedByBlack } = computeCaptures(gameRef.current);
  const whiteScore = capturedByWhite.reduce((s, p) => s + (PIECE_VALUES[p] ?? 0), 0);
  const blackScore = capturedByBlack.reduce((s, p) => s + (PIECE_VALUES[p] ?? 0), 0);
  const whiteAdvantage = whiteScore - blackScore;

  return (
    <div className="flex items-start gap-8">
      <EnginePanel
        mode={mode}
        searchDepth={searchDepth}
        pvCount={pvCount}
        engineReady={engine.ready}
        thinking={engine.thinking}
        analysisResult={analysisResult}
        onSearchDepthChange={setSearchDepth}
        onPvCountChange={setPvCount}
        onAnalyze={handleAnalyze}
        onResetSearch={handleResetSearch}
      />
      <div className="flex flex-col gap-1">
        <PlayerInfo
          color="black"
          name={names.black}
          captures={capturedByBlack}
          materialAdvantage={-whiteAdvantage}
          onNameChange={(n) => setNames((prev) => ({ ...prev, black: n }))}
        />
        <Board
          key={resetKey}
          game={gameRef.current}
          fen={fen}
          status={status}
          allowedColor={mode === "play" ? "w" : undefined}
          onMoveComplete={handleMoveComplete}
        />
        <PlayerInfo
          color="white"
          name={names.white}
          captures={capturedByWhite}
          materialAdvantage={whiteAdvantage}
          onNameChange={(n) => setNames((prev) => ({ ...prev, white: n }))}
        />
      </div>
      <Sidebar
        game={gameRef.current}
        fen={fen}
        status={status}
        mode={mode}
        onModeChange={setMode}
        onFenImport={handleFenImport}
        onReset={handleReset}
      />
    </div>
  );
}
