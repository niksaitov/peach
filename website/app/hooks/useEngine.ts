"use client";

import { useEffect, useRef, useState, useCallback } from "react";

type Callback = (move: string) => void;

export function useEngine() {
  const workerRef = useRef<Worker | null>(null);
  const pendingRef = useRef<Map<string, Callback>>(new Map());
  const [ready, setReady] = useState(false);
  const [thinking, setThinking] = useState(false);

  useEffect(() => {
    const worker = new Worker("/engine-worker.js");

    worker.onmessage = (e: MessageEvent) => {
      const { type, id, move, message } = e.data;
      if (type === "ready") {
        setReady(true);
      } else if (type === "result") {
        setThinking(false);
        const cb = pendingRef.current.get(id);
        if (cb) {
          pendingRef.current.delete(id);
          cb(move);
        }
      } else if (type === "error") {
        console.error("[useEngine] worker error:", message);
        setThinking(false);
        pendingRef.current.clear();
      }
    };

    worker.onerror = (e) => {
      console.error("[useEngine] worker uncaught error:", e.message, e.filename, e.lineno);
    };

    workerRef.current = worker;
    return () => {
      worker.terminate();
      workerRef.current = null;
    };
  }, []);

  const search = useCallback(
    (fen: string, depthPlies: number, onResult: Callback) => {
      if (!workerRef.current || !ready) return;
      const id = crypto.randomUUID();
      pendingRef.current.set(id, onResult);
      setThinking(true);
      workerRef.current.postMessage({ type: "search", fen, depthPlies, id });
    },
    [ready]
  );

  const cancel = useCallback(() => {
    pendingRef.current.clear();
    setThinking(false);
  }, []);

  return { ready, thinking, search, cancel };
}
