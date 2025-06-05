/*
//Calculate and print relevant bit constants
void printRelevantBitConstants(){

    std::cout << '\n' << "BISHOP RELEVANT BITS: \n\n";
    for(int rank = 0; rank < 8; rank++){
        for(int file = 0; file < 8; file++){
            int squareIndex = rank * 8 + file;
            std::cout << getPopulationCount(maskBishopAttacks(squareIndex)) << ", ";
        }
        std::cout << '\n';
    }

    std::cout << '\n' << "ROOK RELEVANT BITS: \n\n";
    for(int rank = 0; rank < 8; rank++){
        for(int file = 0; file < 8; file++){
            int squareIndex = rank * 8 + file;
            std::cout << getPopulationCount(maskRookAttacks(squareIndex)) << ", ";
        }
        std::cout << '\n';
    }
}
*/