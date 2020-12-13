//
// Created by guykn on 12/12/2020.
//

#ifndef CHESS_MOVEGENERATION_H
#define CHESS_MOVEGENERATION_H


#include "Bitboards.h"
#include "MoveList.h"
#include "ChessBoard.h"

namespace Chess{
    template<Player player>
    void generateMoves(ChessBoard& chessBoard);
}


#endif //CHESS_MOVEGENERATION_H
