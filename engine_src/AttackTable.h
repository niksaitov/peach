using U64 = unsigned long long;

class AttackTable{
    
    private:
    
        U64 pawnAttacks[2][64];
        U64 knightAttacks[64];
        U64 kingAttacks[64];

        //The maximum number of relevant bits for a bishop is 9, so the number of possible occupancy combinatons is 2^9 = 512
        U64 bishopAttacks[64][512];

        //The maximum number of relevant bits for a bishop is 11, so the number of possible occupancy combinations is 2^11 = 4096
        U64 rookAttacks[64][4096];

        U64 bishopMasks[64];
        U64 rookMasks[64];
        U64 bishopMagics[64] = {0};
        U64 rookMagics[64] = {0};

        //Initialise magic numbers
        void initialiseMagicNumbers();

        //Initialise leaping piece attack tables
        void initialiseLeapingPieceTables();

        //Initialise sliding piece attack tables
        void initialiseSlidingPieceTables(bool fBishop);

    public:

        //Class constructor to initialise magic numbers and piece attacks
        AttackTable(){
            
            initialiseMagicNumbers();
            initialiseLeapingPieceTables();
            initialiseSlidingPieceTables(true);
            initialiseSlidingPieceTables(false);

        }

        //Get piece attacks
        const U64 getPawnAttacks(int sideToMove, int squareIndex);
        const U64 getKnightAttacks(int squareIndex);
        const U64 getKingAttacks(int squareIndex);
        const U64 getBishopAttacks(int squareIndex, U64 occupancy);
        const U64 getRookAttacks(int squareIndex, U64 occupancy);
        const U64 getQueenAttacks(int squareIndex, U64 occupancy);      
};