importScripts('/engine.js');

let engineModule = null;

ChessEngine()
  .then((m) => {
    engineModule = m;
    postMessage({ type: 'ready' });
  })
  .catch((err) => {
    console.error('[engine-worker] ChessEngine() failed:', err);
    postMessage({ type: 'error', message: String(err) });
  });

self.onmessage = (e) => {
  if (e.data.type !== 'search') return;
  if (!engineModule) {
    postMessage({ type: 'error', id: e.data.id, message: 'Engine not ready' });
    return;
  }
  const { fen, depthPlies, id } = e.data;
  try {
    const move = engineModule.ccall('getBestMove', 'string', ['string', 'number'], [fen, depthPlies]);
    postMessage({ type: 'result', id, move });
  } catch (err) {
    console.error('[engine-worker] search failed:', err);
    postMessage({ type: 'error', id, message: String(err) });
  }
};
