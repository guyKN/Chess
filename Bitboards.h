//
// Created by guykn on 12/11/2020.
//

#ifndef CHESS_BITBOARD_H
#define CHESS_BITBOARD_H

#include "ChessBoard.h"

namespace Chess::Masks {

    void init();
    template <int fileShift>
    constexpr Bitboard maskOfFileShift();

    template<int shift>
    constexpr Bitboard signedShift(Bitboard bitboard);

    template<int rankShift, int fileShift>
    inline Bitboard shift(Bitboard bitboard);


    constexpr Bitboard MASK_RANK_1 = 0x00000000000000FF;
    constexpr Bitboard MASK_FILE_A = 0x0101010101010101;


    constexpr Bitboard maskOfFile(File file) {
        return MASK_FILE_A << file;
    }

    constexpr Bitboard maskOfRank(Rank rank){
        return MASK_RANK_1 << rank;
    }

    constexpr Bitboard MASK_NOT_H_FILE = ~maskOfFile(FILE_H);
    constexpr Bitboard MASK_NOT_A_FILE = ~maskOfFile(FILE_A);

    constexpr Bitboard MASK_RANK_8 = maskOfRank(RANK_8);


    template<int fileShift>
    constexpr Bitboard maskOfFileShift() {
        if constexpr (fileShift>0){
            Bitboard mask = BITBOARD_FULL;
            for(int i=0;i<fileShift;i++){
                mask>>=1;
                mask&=MASK_NOT_H_FILE;
            }
            return mask;
        } else {
            Bitboard mask = BITBOARD_FULL;
            for(int i=0;i>fileShift;i--){
                mask<<=1;
                mask&=MASK_NOT_A_FILE;
            }
            return mask;
        }
    }

    template<int shift>
    constexpr Bitboard signedShift(Bitboard bitboard) {
        if constexpr (shift>0){
            return bitboard << shift;
        } else if constexpr (shift<0){
            return bitboard >> (-shift);
        } else{
            return bitboard;
        }
    }

    template<int rankShift, int fileShift>
    Bitboard shift(Bitboard bitboard) {
        static_assert(rankShift > -NUM_RANKS &&
                      rankShift < NUM_RANKS &&
                      fileShift > -NUM_FILES &&
                      fileShift < NUM_FILES,
                      "file and rank shift must be between -7 and 7");
        constexpr int totalShift = fileShift + rankShift * NUM_FILES;
        constexpr Bitboard mask = maskOfFileShift<fileShift>();
        return signedShift<totalShift>(bitboard & mask);

    }

    constexpr inline Bitboard firstRankOf(Player player) {
        return player == WHITE ? MASK_RANK_1 : MASK_RANK_8;
    }









/// code below shamelessly stolen from stockfish
/// https://github.com/official-stockfish/Stockfish
/// thank you stockfish



/// lsb() and msb() return the least/most significant bit in a non-zero bitboard

#if defined(__GNUC__)  // GCC, Clang, ICC

    inline Square lsb(Bitboard b) {
      assert(b);
      return Square(__builtin_ctzll(b));
    }

    inline Square msb(Bitboard b) {
      assert(b);
      return Square(63 ^ __builtin_clzll(b));
    }

#elif defined(_MSC_VER)  // MSVC

#ifdef _WIN64  // MSVC, WIN64

    inline Square lsb(Bitboard b) {
        assert(b);
        unsigned long idx;
        _BitScanForward64(&idx, b);
        return static_cast<Square>(idx);
    }

    inline Square msb(Bitboard b) {
        assert(b);
        unsigned long idx;
        _BitScanReverse64(&idx, b);
        return static_cast<Square>(idx);
    }

#else  // MSVC, WIN32

    inline Square lsb(Bitboard b) {
        assert(b);
        unsigned long idx;

        if (b & 0xffffffff) {
            _BitScanForward(&idx, int32_t(b));
            return Square(idx);
        } else {
            _BitScanForward(&idx, int32_t(b >> 32));
            return Square(idx + 32);
        }
    }

    inline Square msb(Bitboard b) {
        assert(b);
        unsigned long idx;

        if (b >> 32) {
            _BitScanReverse(&idx, int32_t(b >> 32));
            return Square(idx + 32);
        } else {
            _BitScanReverse(&idx, int32_t(b));
            return Square(idx);
        }
    }

#endif

#else  // Compiler is neither GCC nor MSVC compatible

#error "Compiler not supported."

#endif

/// popLsb() finds and clears the least significant bit in a non-zero bitboard

    inline Square popLsb(Bitboard *b) {
        assert(*b);
        const Square s = lsb(*b);
        *b &= *b - 1;
        return s;
    }

    inline SquareMask popLsbMask(Bitboard *b){
        //todo: is this fastest
        return maskOf(popLsb(b));
    }


/// frontmost_sq() returns the most advanced square for the given color,
/// requires a non-zero bitboard.
    inline Square frontmost_sq(Player c, Bitboard b) {
        assert(b);
        return c == WHITE ? msb(b) : lsb(b);
    }


}
#endif //CHESS_BITBOARD_H

