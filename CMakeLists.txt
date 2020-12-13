cmake_minimum_required(VERSION 3.17)
project(Chess4)

set(CMAKE_CXX_STANDARD 17)

add_executable(Chess4 main.cpp types.cpp types.h Bitboards.cpp Bitboards.h ChessBoard.cpp ChessBoard.h Move.cpp Move.h moveGeneration.cpp moveGeneration.h MoveInputData.cpp MoveInputData.h MoveList.cpp MoveList.h)