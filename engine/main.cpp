#include <iostream>

#include "globals.h"
#include "random.h"
#include "Position.h"
#include "move_encoding.h"
#include "const.h"

using std::cout, std::string;

void search(string fenString, int depth)
{
    seedRandom();
    generateKeys();
    generateEvaluationMasks();

    Position position(fenString);
    position.getBoard().printState();
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

        cout << "\n\nEvaluation: " << score;
        cout << "\nPrincipled variation: ";
        position.printPV();
    }

    cout << "\n\nBest Move: ";
    printMove(position.getBestMove());
    cout << "\n";
}

int main()
{
    search(START_POSITION_FEN, 10);
    return 0;
}
