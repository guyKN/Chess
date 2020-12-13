//
// Created by guykn on 12/7/2020.
//

#ifndef CHESS_CHESSBOARD_H
#define CHESS_CHESSBOARD_H


#include "types.h"
#include "Move.h"
#include "MoveInputData.h"
#include "MoveList.h"

using std::cout;
namespace Chess {

    //todo: expirement with additional representations in addition to bitboards
    // specificly: pieceLists as vectors, and an array of all pieces by squarew

    class ChessBoard {
        Bitboard pieceBitboards[NUM_PIECES] = {};
        Bitboard byPlayerBitboard[NUM_PLAYERS] = {};
        Player currentPlayer = WHITE;

        inline Bitboard &bitboardOf(Piece piece) {
            return pieceBitboards[piece];
        }

        inline Bitboard &bitboardOf(Player player){
            return byPlayerBitboard[player];
        }

        inline Bitboard getBitboardOf(Piece piece) const {
            return pieceBitboards[piece];
        }


        template<bool checkPieceNone>
        Piece pieceOn(Square square, Piece startingPiece, Piece endPiece) const {
            SquareMask mask = maskOf(square);
            if constexpr(checkPieceNone) {
                if (mask & getBitboardOf(PIECE_NONE)) {
                    return PIECE_NONE;
                }
            }
            for (Piece piece = startingPiece; piece <= endPiece; ++piece) {
                if (getBitboardOf(piece) & mask) {
                    return piece;
                }
            }
            assert(false);
            return PIECE_NONE;
        }

        void swapPlayer() {
            currentPlayer = ~currentPlayer;
        }

        template<Player player>
        void generatePawnQuietMoves(MoveList &moveList);

        template<Player, int>
        void generatePawnCaptures(MoveList &moveList);

        bool noPieceOverlap();

        bool noColorOverlap();

        template<Player player>
        void generateMovesForPlayer(MoveList &moveList);

    public:

        void printBitboards(){
            cout << "\nCurrent player: " << currentPlayer;
            cout << "\nWhite Pawns:";
            printBitboard(bitboardOf(PIECE_WHITE_PAWN));

            cout << "\nBlack Pawns:";
            printBitboard(bitboardOf(PIECE_BLACK_PAWN));

            cout << "\nEmpty squares:";
            printBitboard(bitboardOf(PIECE_NONE));

            cout << "\nWhite Pieces:";
            printBitboard(bitboardOf(WHITE));

            cout << "\nBlack Pieces:";
            printBitboard(bitboardOf(BLACK));
        }


        explicit ChessBoard(const Piece piecesBySquare[NUM_SQUARES], Player currentPlayerColor);

        ChessBoard() : ChessBoard(STARTING_BOARD, STARTING_PLAYER) {}

        void doMove(Move move);

        friend std::ostream &operator<<(std::ostream &os, const ChessBoard &chessBoard);

        void assertOk(){
            if(!isOk()){
                printBitboards();
                assert(false);
            }
        }

        //returns true if the board has no overlap of pieces
        bool isOk();

        Piece pieceOn(Square square) const {
            return pieceOn<true>(square, PIECE_FIRST, PIECE_LAST_BLACK);
        }

        Piece pieceOn(Square square, Player player) const {
            return pieceOn<true>(square, firstPieceOf(player), lastPieceOf(player));
        }

        Piece requiredPieceOn(Square square, Player player) const{
            return pieceOn<false>(square, firstPieceOf(player), lastPieceOf(player));
        }


        void generateMoves(MoveList &moveList);
    };
}


#endif //CHESS_CHESSBOARD_H
