"use client";

import type { Mode } from "./ChessGame";

interface EnginePanelProps {
  mode: Mode;
  searchDepth: number;
  pvCount: number;
  engineReady: boolean;
  thinking: boolean;
  analysisResult: string | null;
  onSearchDepthChange: (depth: number) => void;
  onPvCountChange: (count: number) => void;
  onAnalyze: () => void;
  onResetSearch: () => void;
}

export default function EnginePanel({
  mode, searchDepth, pvCount,
  engineReady, thinking, analysisResult,
  onSearchDepthChange, onPvCountChange, onAnalyze, onResetSearch,
}: EnginePanelProps) {
  return (
    <div className="flex flex-col gap-5 w-56 text-zinc-300">

      <div>
        {/* Header + readiness */}
        <div className="flex items-center justify-between mb-3">
          <p className="text-xs uppercase tracking-widest text-zinc-500">Engine</p>
          <span className={`text-xs font-medium ${engineReady ? "text-emerald-500" : "text-zinc-600"}`}>
            {engineReady ? "ready" : "loading…"}
          </span>
        </div>

        {/* Depth — both modes */}
        <div className="mb-5">
          <p className="text-xs text-zinc-500 mb-1.5">
            Depth{" "}
            <span className="text-zinc-300 font-medium">
              {searchDepth} {searchDepth === 1 ? "move" : "moves"}
            </span>
          </p>
          <input
            type="range"
            min={1}
            max={15}
            value={searchDepth}
            onChange={(e) => onSearchDepthChange(Number(e.target.value))}
            className="w-full accent-zinc-400 cursor-pointer"
          />
          <div className="flex justify-between text-xs text-zinc-600 mt-0.5 px-0.5">
            <span>1</span><span>15</span>
          </div>
        </div>

        {/* Evaluate-only controls */}
        {mode === "evaluate" && (
          <>
            {/* Principal variations */}
            <div className="mb-5">
              <p className="text-xs text-zinc-500 mb-1.5">Variations</p>
              <div className="flex gap-1">
                {[1, 2, 3, 4, 5].map((n) => (
                  <button
                    key={n}
                    onClick={() => onPvCountChange(n)}
                    className={`flex-1 py-2 text-xs rounded-lg transition-colors ${
                      pvCount === n
                        ? "bg-zinc-300 text-zinc-900 font-semibold"
                        : "bg-zinc-800 hover:bg-zinc-700 text-zinc-400"
                    }`}
                  >
                    {n}
                  </button>
                ))}
              </div>
            </div>

            {/* Actions */}
            <div className="flex flex-col gap-2">
              <button
                onClick={onAnalyze}
                disabled={!engineReady || thinking}
                className="w-full px-3 py-2.5 text-sm rounded-lg bg-zinc-700 hover:bg-zinc-600 disabled:opacity-50 disabled:cursor-not-allowed text-zinc-200 transition-colors"
              >
                {thinking ? "Thinking…" : "Analyze"}
              </button>
              <button
                onClick={onResetSearch}
                disabled={!thinking}
                className="w-full px-3 py-2.5 text-sm rounded-lg bg-zinc-800 hover:bg-zinc-700 disabled:opacity-40 disabled:cursor-not-allowed text-zinc-400 transition-colors"
              >
                Reset Search
              </button>
            </div>

            {/* Analysis result */}
            {analysisResult && !thinking && (
              <div className="mt-3 px-3 py-2.5 rounded-lg bg-zinc-800 border border-zinc-700">
                <p className="text-xs text-zinc-500 mb-0.5">Best move</p>
                <p className="text-base font-semibold text-zinc-100">{analysisResult}</p>
              </div>
            )}
          </>
        )}

        {/* Play mode thinking indicator */}
        {mode === "play" && thinking && (
          <div className="mt-1 px-3 py-2 rounded-lg bg-zinc-800 border border-zinc-700 text-xs text-zinc-400">
            Engine thinking…
          </div>
        )}
      </div>

    </div>
  );
}
