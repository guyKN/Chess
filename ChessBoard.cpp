//
// Created by guykn on 12/7/2020.
//

#include <string>
#include "ChessBoard.h"
#include "iostream"
#include "Bitboards.h"

namespace Chess {
    using namespace Masks;
    ChessBoard::ChessBoard(const Piece piecesBySquare[NUM_SQUARES], Player currentPlayerColor) : currentPlayer(
            currentPlayerColor) {

        for (Piece piece = PIECE_FIRST; piece <= PIECE_LAST; ++piece) {
            bitboardOf(piece) = BITBOARD_EMPTY;
        }

        bitboardOf(WHITE) = BITBOARD_EMPTY;
        bitboardOf(BLACK) = BITBOARD_EMPTY;


        SquareMask squareMask = SQUARE_MASK_FIRST;
        for (Square square = SQ_FIRST; square <= SQ_LAST; ++square) {
            Piece piece = piecesBySquare[square];
            bitboardOf(piece) |= squareMask;
            if(piece != PIECE_NONE){
                bitboardOf(playerOf(piece)) |= squareMask;
            }
            squareMask <<= 1;
        }
    }


    std::ostream &operator<<(std::ostream &os, const ChessBoard &chessBoard) {
        os << "\n  +---+---+---+---+---+---+---+---+\n";
        for (Rank rank = RANK_FIRST; rank <= RANK_LAST; ++rank) {
            os << (1 + rank) << " ";
            for (File file = FILE_FIRST; file <= FILE_LAST; ++file) {
                Piece piece = chessBoard.pieceOn(makeSquare(rank, file));
                os << "| " << toChar(piece) << " ";
            }
            os << "| " << "\n  +---+---+---+---+---+---+---+---+\n";
        }
        os << "    A   B   C   D   E   F   G   H\n";
        return os;
    }

    void ChessBoard::doMove(Move move) {
        (bitboardOf(PIECE_NONE)) |= maskOf(move.srcSquare);
        (bitboardOf(move.srcPiece) &= notSquareMask(move.srcSquare)) |= maskOf(move.dstSquare);
        (bitboardOf(move.dstPiece)) &= notSquareMask(move.dstSquare);

        ((bitboardOf(currentPlayer)) &= notSquareMask(move.srcSquare)) |= maskOf(move.dstSquare);
        bitboardOf(~currentPlayer) &= notSquareMask(move.dstSquare);

        swapPlayer();
    }



    bool ChessBoard::isOk() {
        return noPieceOverlap() & noColorOverlap();
    }

    bool ChessBoard::noPieceOverlap() {
        /// ensures that each Square is occupied by either exactly one piece, or by PIECE_NONE
        Bitboard pieces = BITBOARD_EMPTY;
        for (Piece piece = PIECE_FIRST; piece <= PIECE_LAST; ++piece) {
            if (pieces & bitboardOf(piece)) {
                return false;
            }
            pieces |= bitboardOf(piece);
        }
        return pieces == BITBOARD_FULL;
    }

    bool ChessBoard::noColorOverlap(){
        /// ensures that each Square is occupied by either exactly one player, or by PIECE_NONE
        Bitboard pieces = bitboardOf(PIECE_NONE);
        if(pieces & bitboardOf(WHITE)){
            return false;
        }
        pieces |= bitboardOf(WHITE);

        if(pieces & bitboardOf(BLACK)){
            return false;
        }
        pieces |= bitboardOf(BLACK);

        return pieces == BITBOARD_FULL;

    }


    template<Player player>
    void ChessBoard::generatePawnQuietMoves(MoveList &moveList) {
        constexpr int forward1shift = directionOf(player) * 8;
        constexpr int forward4shift = forward1shift*4;
        constexpr int forward2shift = 2 * forward1shift;
        constexpr Piece pawnPiece = makePiece(PIECE_TYPE_PAWN, player);
        constexpr Bitboard rankFourMask = signedShift<forward4shift>(firstRankOf(player));
        Bitboard pawns = bitboardOf(pawnPiece);
        Bitboard emptySquares = bitboardOf(PIECE_NONE);
        Bitboard mayMoveForward2mask = signedShift<forward1shift>(emptySquares) & emptySquares & rankFourMask;
        cout << "signedShift<forward1> of emptySquares:\n";
        printBitboard(signedShift<forward1shift>(emptySquares));
        cout << "rankFourMask: ";
        printBitboard(rankFourMask);
        cout << "May move forward 2 mask: \n";
        printBitboard(mayMoveForward2mask);


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
    void ChessBoard::generatePawnCaptures(MoveList &moveList) {
        //todo: check if doing this is fastest, or if doing captures both ways together is faster
        constexpr int shift = directionOf(player) * 8 + captureDirection;
        constexpr Piece pawnPiece = makePiece(PIECE_TYPE_PAWN, player);
        constexpr Bitboard fileMask = maskOfFileShift<captureDirection>();
        constexpr Player opponent = ~player;

        Bitboard validPawns = bitboardOf(pawnPiece) & fileMask; //all pawns, except for pawns on the edge that can't capture
        Bitboard enemyPieces = bitboardOf(opponent);
        while (validPawns){
            Square srcSquare = popLsb(&validPawns);
            SquareMask srcSquareMask = maskOf(srcSquare);
            Bitboard moveBitboard = signedShift<shift>(srcSquareMask)  & enemyPieces;
            if(moveBitboard){
                Square dstSquare = lsb(moveBitboard);
                moveList.addMove(Move(srcSquare,
                                      dstSquare,
                                      pawnPiece,
                                      requiredPieceOn(dstSquare, opponent)));
            }
        }
    }

    template<Player player>
    void ChessBoard::generateMovesForPlayer(MoveList &moveList){
        generatePawnQuietMoves<player>(moveList);
        generatePawnCaptures<player, 1>(moveList);
        generatePawnCaptures<player, -1>(moveList);
    }

    void ChessBoard::generateMoves(MoveList &moveList) {
        if(currentPlayer == WHITE) {
            generateMovesForPlayer<WHITE>(moveList);
        } else{
            generateMovesForPlayer<BLACK>(moveList);
        }
    }


    /*
              SquareMask srcSquareMask = SQUARE_MASK_FIRST;
        Square srcSquare = SQ_FIRST;
        Square dstSquare;
        for (Rank rank = RANK_FIRST; rank <= RANK_LAST; ++rank) {
            for (File file = FILE_FIRST; file <= FILE_LAST; ++file, ++srcSquare, srcSquareMask<<=1) {
                for (Piece piece = firstPieceOf(currentPlayer); piece <= lastPieceOf(currentPlayer); ++piece) {
                    if (bitboardOf(piece) & srcSquareMask) {
                        dstSquare = srcSquare + NORTH;
                        if(maskOf(dstSquare) & bitboardOf(PIECE_NONE)){
                            moveList.addMove(Move(srcSquare, dstSquare, PIECE_WHITE_PAWN, pieceOn(dstSquare)));
                            if(rank == RANK_2){
                                dstSquare = dstSquare + NORTH;
                                if(maskOf(dstSquare) & bitboardOf(PIECE_NONE)){
                                    moveList.addMove(Move(srcSquare, dstSquare, PIECE_WHITE_PAWN, PIECE_NONE));
                                }
                            }
                        }
                        if(file != FILE_A){
                            dstSquare = srcSquare + NORTH_WEST;
                            if(bitboardOf(PIECE_BLACK_PAWN) & maskOf(dstSquare)){
                                moveList.addMove(Move(srcSquare, dstSquare, PIECE_WHITE_PAWN, requiredPieceOn(dstSquare, ~currentPlayer)));
                            }
                        }
                        if(file != FILE_H){
                            dstSquare = srcSquare + NORTH_EAST;
                            if(bitboardOf(PIECE_BLACK_PAWN) & maskOfFile(dstSquare)){
                                moveList.addMove(Move(srcSquare, dstSquare, PIECE_WHITE_PAWN, requiredPieceOn(dstSquare, ~currentPlayer)));
                            }
                        }
                    }
                }
            }
        }*/


}