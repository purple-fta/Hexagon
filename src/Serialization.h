#pragma once

#include "Board.h"

#include <fstream>
#include <filesystem>
#include <stdexcept>


std::vector<int> seialize(Board& board) {
    std::vector<int> values;

    values.push_back(static_cast<int>(board.singleGame));
    values.push_back(static_cast<int>(board.isPlayer1Turn));

    for (const auto& row : board.cells) {
        for (const auto& cell : row) {
            values.push_back(static_cast<int>(cell.getState()));
        }
    }
    return values;
}


void save(Board& board) {
    const std::string filename = "board_save.sv";
    std::vector<int> values = seialize(board);

    std::ofstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file for saving.");
    }

    for (const int value : values) {
        file << value << '\n';
    }

    file.close();
}


Board deserialize(sf::RenderWindow &window, std::vector<int>& values) {
    bool singleGame = static_cast<bool>(values[0]);
    bool isPlayer1Turn = static_cast<bool>(values[1]);
    
    Board board(window, singleGame);
    board.isPlayer1Turn = isPlayer1Turn;

    for (int i = 2; i < values.size(); i++) {
        int row = (i - 2) / 9;
        int col = (i - 2) % 9;
        CellState state = static_cast<CellState>(values[i]);
        board.cells[row][col].setState(state);
        board.cells[row][col].setColors(state);
    }

    return board;
}



Board load(sf::RenderWindow &window) {

    const std::string filename = "board_save.sv";

    if (std::filesystem::exists(filename)) {
        std::cout << "File exists" << std::endl;
        std::ifstream file(filename);

        std::vector<int> values;
        std::string line;
        while (std::getline(file, line)) {
            values.push_back(std::stoi(line));
        }
        file.close();
        
        return deserialize(window, values);
    } else {
        std::cout << "File not exists" << std::endl;
        return Board(window, true);
    }
}
