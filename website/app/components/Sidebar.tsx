"use client";

import { useState } from "react";
import { Chess } from "chess.js";
import type { GameStatus, Mode } from "./ChessGame";

interface SidebarProps {
  game: Chess;
  fen: string;
  status: GameStatus;
  mode: Mode;
  onModeChange: (mode: Mode) => void;
  onFenImport: (fen: string) => void;
  onReset: () => void;
}

function CopyButton({ label, getValue }: { label: string; getValue: () => string }) {
  const [copied, setCopied] = useState(false);
  function handleCopy() {
    navigator.clipboard.writeText(getValue());
    setCopied(true);
    setTimeout(() => setCopied(false), 1500);
  }
  return (
    <button
      onClick={handleCopy}
      className="w-full px-3 py-2.5 text-sm rounded-lg bg-zinc-700 hover:bg-zinc-600 text-zinc-200 transition-colors"
    >
      {copied ? "Copied!" : label}
    </button>
  );
}

export default function Sidebar({ game, fen, status, mode, onModeChange, onFenImport, onReset }: SidebarProps) {
  const [fenInput, setFenInput] = useState("");
  const [fenError, setFenError] = useState<string | null>(null);

  function handleFenImport() {
    const trimmed = fenInput.trim();
    if (!trimmed) return;
    try {
      const test = new Chess();
      test.load(trimmed);
      onFenImport(trimmed);
      setFenInput("");
      setFenError(null);
    } catch {
      setFenError("Invalid FEN string");
    }
  }

  return (
    <div className="flex flex-col gap-5 w-64 text-zinc-300">

      {/* Mode switcher */}
      <div>
        <p className="text-xs uppercase tracking-widest text-zinc-500 mb-2">Mode</p>
        <div className="flex rounded overflow-hidden border border-zinc-700">
          {(["play", "evaluate"] as Mode[]).map((m) => (
            <button
              key={m}
              onClick={() => onModeChange(m)}
              className={`flex-1 py-2.5 text-sm capitalize transition-colors ${
                mode === m
                  ? "bg-zinc-200 text-zinc-900 font-semibold"
                  : "bg-zinc-800 hover:bg-zinc-700 text-zinc-400"
              }`}
            >
              {m}
            </button>
          ))}
        </div>
      </div>

      {/* Status */}
      {status && (
        <div className={`px-3 py-2 rounded text-sm font-medium ${status.isOver ? "bg-zinc-700 text-zinc-200" : "bg-amber-700 text-amber-100"}`}>
          {status.message}
        </div>
      )}

      {/* Game controls */}
      <div>
        <p className="text-xs uppercase tracking-widest text-zinc-500 mb-2">Game</p>
        <button
          onClick={onReset}
          className="w-full px-3 py-2.5 text-sm rounded-lg bg-zinc-700 hover:bg-zinc-600 text-zinc-200 transition-colors"
        >
          New Game
        </button>
      </div>

      {/* FEN import */}
      <div>
        <p className="text-xs uppercase tracking-widest text-zinc-500 mb-2">Import FEN</p>
        <textarea
          value={fenInput}
          onChange={(e) => { setFenInput(e.target.value); setFenError(null); }}
          placeholder="Paste FEN string…"
          rows={3}
          className="w-full px-3 py-2 text-xs rounded bg-zinc-800 border border-zinc-700 text-zinc-200 placeholder-zinc-600 resize-none focus:outline-none focus:border-zinc-500"
        />
        {fenError && <p className="text-xs text-red-400 mt-1">{fenError}</p>}
        <button
          onClick={handleFenImport}
          className="mt-2 w-full px-3 py-2.5 text-sm rounded-lg bg-zinc-700 hover:bg-zinc-600 text-zinc-200 transition-colors"
        >
          Load Position
        </button>
      </div>

      {/* Export */}
      <div>
        <p className="text-xs uppercase tracking-widest text-zinc-500 mb-2">Export</p>
        <div className="flex flex-col gap-2">
          <CopyButton label="Copy FEN" getValue={() => fen} />
          <CopyButton label="Copy PGN" getValue={() => game.pgn()} />
        </div>
      </div>


    </div>
  );
}
