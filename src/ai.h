#pragma once

#include "Board.h"

enum class MoveType {
    Clone,
    Move
};

struct Move {
    int fromRow, fromCol;
    int toRow, toCol;
    MoveType type;
};

Move getBestMove(Board board);
void doBestMove(Board& board);

