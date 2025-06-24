#include "ai.h"
#include <iostream>


Move getBestMove(const BoardState& board) {
    Move bestMove;
    int bestScore = -1;

    auto availableCells = board.getCellsWithState(CellState::Player2);
    for (auto [fromRow, fromCol] : availableCells) {
        auto availableForCloning = board.getAvailableForCloningCells(fromRow, fromCol);
        for (auto [toRow, toCol] : availableForCloning) {
            BoardState copyBoard = board;
            int score = copyBoard.cloneFromTo(fromRow, fromCol, toRow, toCol);
            if (score > bestScore) {
                bestScore = score;
                bestMove = {fromRow, fromCol, toRow, toCol, MoveType::Clone};
            }
        }
        auto availableForMoving = board.getAvailableForMovingCells(fromRow, fromCol);
        for (auto [toRow, toCol] : availableForMoving) {
            BoardState copyBoard = board;
            int score = copyBoard.moveFromTo(fromRow, fromCol, toRow, toCol);
            if (score > bestScore) {
                bestScore = score;
                bestMove = {fromRow, fromCol, toRow, toCol, MoveType::Move};
            }
        }
    }
    return bestMove;
}

void doBestMove(Board& board) {
    Move move = getBestMove(BoardState(board));

    std::cout << "Best move: " << move.fromRow << ", " << move.fromCol << " to " << move.toRow << ", " << move.toCol << std::endl;

    Cell& from = board.cells[move.fromRow][move.fromCol];
    Cell& to = board.cells[move.toRow][move.toCol];

    if (move.type == MoveType::Clone) {
        board.cloneFromTo(from, to);
    } else if (move.type == MoveType::Move) {
        board.moveFromTo(from, to);
    }
}