//
// Created by guykn on 12/12/2020.
//

#include "moveGeneration.h"
#include "ChessBoard.h"

namespace Chess {
    using namespace Masks;

    template<Player player>
    void generatePawnQuietMoves(MoveList &moveList, Bitboard pawns, Bitboard emptySquares) {
        constexpr int forward1shift = directionOf(player) * 8;
        constexpr int forward2shift = 2 * forward1shift;
        constexpr Piece pawnPiece = makePiece(PIECE_TYPE_PAWN, player);
        constexpr Bitboard rankFourMask = shift<forward1shift * 4>(firstRankOf(player));

        Bitboard mayMoveForward2mask = signedShift<forward1shift>(emptySquares) & emptySquares & rankFourMask;
        //todo: see if this is fastest
        while (pawns) {
            Square srcSquare = popLsb(&pawns);
            SquareMask srcSquareMask = maskOf(srcSquare);
            Bitboard legalMoves = (signedShift<forward1shift>(srcSquareMask) & emptySquares) |
                                  (signedShift<forward2shift>(srcSquareMask) & mayMoveForward2mask);
            while (legalMoves) {
                moveList.addMove(Move(srcSquare, popLsb(&legalMoves), pawnPiece, PIECE_NONE));
            }
        }
    }

    template<Player player, int captureDirection>
    void generatePawnCaptures(MoveList& moveList, Bitboard pawns, Bitboard enemyPieces, ChessBoard &chessBoard){
        //todo: check if doing this is fastest, or if doing captures both ways together is faster
        constexpr int shift = directionOf(player) * 8 + captureDirection;
        constexpr Piece pawnPiece = makePiece(PIECE_TYPE_PAWN, player);
        constexpr Bitboard fileMask = maskOfFileShift<captureDirection>();
        constexpr Player opponent = ~player;

        Bitboard validPawns = pawns & fileMask; //all pawns, except for pawns on the edge that can't capture

        while (validPawns){
            Square srcSquare = popLsb(&validPawns);
            SquareMask srcSquareMask = maskOf(srcSquare);
            Bitboard moveBitboard = signedShift<shift>(srcSquareMask)  & enemyPieces;
            if(moveBitboard){
                Square dstSquare = lsb(moveBitboard);
                moveList.addMove(Move(srcSquare,
                                      dstSquare,
                                      pawnPiece,
                                      chessBoard.requiredPieceOn(dstSquare, opponent)));
            }
        }
    }
    template <Player player>
    inline void generateMoves(ChessBoard &chessBoard, MoveList& moveList) {

    }
}