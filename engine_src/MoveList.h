extern "C" {

    class MoveList{
        
        private:

        int moves[256];
        int count = 0; 

        public:

            MoveList(){};

            //Add move to the moves array at index specified by the count member variable
            void appendMove(int startSquareIndex, int targetSquareIndex, int piece, int promotedPiece, 
                        bool fCapture, bool fDoublePawnPush, bool fEnPassant, bool fCastling);
            
            //Get the array of moves
            int* getMoves();
            
            //Get the value of the count variable
            const int getCount();
    };
}


