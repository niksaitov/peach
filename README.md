# peach

A chess engine written from scratch in C++, compiled to WebAssembly and playable in the browser.

No frameworks, no external chess libraries: bitboards, magic move generation, alpha-beta
search and a hand-tuned evaluation, all built up from an empty file.

## Building it

Native binary (a CLI that searches a position and prints the principal variation):

```bash
cd engine
make
./main
```

WebAssembly build, which emits `engine.js` and `engine.wasm` straight into the site's
`public/` directory (requires the Emscripten SDK on your PATH):

```bash
cd engine
make wasm
```

Then run the site:

```bash
cd website
npm install
npm run dev
```

## Playing it

The web app runs the engine entirely client side. The compiled WASM module lives in a web
worker, so the board stays responsive while the engine thinks.

- **Play a full game against it.** You take white, the engine answers as black.
- **Set up any position and ask for the best move.** Analysis mode lets you move both sides freely, then hand the position to the engine.
- **Decide how hard it thinks.** A depth slider sets how many moves ahead the search looks. Higher is stronger and slower.

## Under the hood

**Board representation.** Twelve piece bitboards plus occupancy sets, with a Zobrist hash
maintained incrementally for repetition detection and transposition lookups.

**Move generation.** Magic bitboards for sliding pieces, with the magics and relevant
occupancy masks precomputed into lookup tables at startup. Leaper attacks come from
generated tables.

**Search.** Iterative deepening negamax with:

- aspiration windows around the previous iteration's score
- principal variation search with a zero-window scout, and PV tracking through a triangular table
- quiescence search on captures to settle tactics before evaluating
- a transposition table keyed on the Zobrist hash, with exact/alpha/beta bound flags
- null move pruning and late move reductions
- MVV-LVA capture ordering, killer moves and history heuristics
- threefold repetition detection

**Evaluation.** Hand-crafted and tapered between an opening and an endgame score:
material, piece-square tables, doubled and isolated pawn penalties, passed pawn bonuses,
bishop and queen mobility, semi-open and open files, and a king safety term.

## Background

peach started as an A-level computer science NEA: the same engine core, wrapped in a Flask
site with accounts and a drag-and-drop board. It has since been rebuilt as a multi-file
project with a proper build system, an Emscripten target, and a modern frontend, while the
search and evaluation kept growing.
