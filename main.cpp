#include <iostream>
#include "Bitboards.h"
#include "ChessBoard.h"

using namespace Chess;
using namespace Masks;
using std::cout;
int main() {
    ChessBoard chessBoard = ChessBoard();
    for (int i = 0; i < 6; i++) {
        while (true) {
            chessBoard.assertOk();
            chessBoard.printBitboards();
            MoveList moveList;
            chessBoard.generateMoves(moveList);
            cout << "\n" << moveList << "\n" << chessBoard << "Enter a move: ";
            MoveInputData moveInput = MoveInputData::readMove();
            Move move = moveList.getMoveFromInputData(moveInput);

            if (move.isOk()) {
                chessBoard.doMove(move);
                break;
            } else {
                cout << "Invalid move. Please enter a move again.";
            }
        }
    }
    cout << chessBoard;

    return 0;
}

