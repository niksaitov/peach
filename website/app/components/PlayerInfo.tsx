"use client";

import { useState } from "react";

const SORT_ORDER = ["p", "n", "b", "r", "q"];

const OPPONENT_SYMBOLS: Record<"white" | "black", Record<string, string>> = {
  white: { p: "♟", n: "♞", b: "♝", r: "♜", q: "♛" },
  black: { p: "♙", n: "♘", b: "♗", r: "♖", q: "♕" },
};

interface PlayerInfoProps {
  color: "white" | "black";
  name: string;
  captures: string[];
  materialAdvantage: number;
  onNameChange: (name: string) => void;
}

export default function PlayerInfo({ color, name, captures, materialAdvantage, onNameChange }: PlayerInfoProps) {
  const [editing, setEditing] = useState(false);
  const [draft, setDraft] = useState(name);

  const symbols = OPPONENT_SYMBOLS[color];
  const sorted = [...captures].sort((a, b) => SORT_ORDER.indexOf(a) - SORT_ORDER.indexOf(b));
  const displayStr = sorted.map((p) => symbols[p] ?? "").join("");

  function commitEdit() {
    const trimmed = draft.trim() || (color === "white" ? "White" : "Black");
    onNameChange(trimmed);
    setEditing(false);
  }

  return (
    <div className="flex items-center gap-2 h-7 select-none" style={{ width: 640 }}>
      {editing ? (
        <input
          autoFocus
          value={draft}
          onChange={(e) => setDraft(e.target.value)}
          onBlur={commitEdit}
          onKeyDown={(e) => {
            if (e.key === "Enter") commitEdit();
            if (e.key === "Escape") { setDraft(name); setEditing(false); }
          }}
          className="text-sm font-semibold text-zinc-200 bg-transparent border-b border-zinc-500 outline-none w-32"
        />
      ) : (
        <span
          className="text-sm font-semibold text-zinc-200 cursor-pointer hover:text-white"
          onClick={() => { setDraft(name); setEditing(true); }}
        >
          {name}
        </span>
      )}
      {displayStr && (
        <span className="text-base leading-none text-zinc-300 tracking-tight">{displayStr}</span>
      )}
      {materialAdvantage > 0 && (
        <span className="text-xs text-zinc-400 font-medium">+{materialAdvantage}</span>
      )}
    </div>
  );
}
