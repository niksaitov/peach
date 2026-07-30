#include <emscripten.h>
#include <cctype>
#include <cstring>
#include <string>

#include "globals.h"
#include "random.h"
#include "Position.h"
#include "move_encoding.h"
#include "const.h"

static bool initialised = false;

static void init()
{
    if (initialised) return;
    seedRandom();
    generateKeys();
    generateEvaluationMasks();
    initialised = true;
}

extern "C"
{
    // Returns the best move as a UCI string
    // The caller must not free the returned pointer as it points to a static buffer.
    EMSCRIPTEN_KEEPALIVE
    const char *getBestMove(const char *fen, int depth)
    {
        init();

        std::string fenStr(fen);
        Position position(fenStr);
        position.resetSearchVariables();

        int alpha = -INF, beta = INF;

        for (int currentDepth = 1; currentDepth <= depth; currentDepth++)
        {
            int score = position.negamax(alpha, beta, currentDepth);

            if ((score <= alpha) || (score >= beta))
            {
                alpha = -INF;
                beta = INF;
                continue;
            }

            alpha = score + ASPIRATION_WINDOW;
            beta = score - ASPIRATION_WINDOW;
        }

        int move = position.getBestMove();
        static char result[6];

        if (!move)
        {
            strncpy(result, "0000", 5);
            return result;
        }

        std::string moveStr = SQUARE_INDEX_TO_COORDINATES[getStartSquareIndex(move)]
                            + SQUARE_INDEX_TO_COORDINATES[getTargetSquareIndex(move)];

        int promoted = getPromotedPiece(move);
        if (promoted)
            moveStr += (char)tolower(PIECE_INDEX_TO_ASCII[promoted]);

        strncpy(result, moveStr.c_str(), 5);
        result[5] = '\0';
        return result;
    }
}
