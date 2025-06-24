#pragma once

#include <SFML/Graphics.hpp>

#include "pallete.h"

enum class CellState { Empty, Player1, Player2, Blocked };
enum class HighlightState { 
    None,
    Selected,
    AvailableForCloning, 
    AvailableForMoving 
};

class Cell {
public:
    int hexagon_size = 35;
    int indents = 5;

    Cell() : state(CellState::Empty), x(0), y(0) {};

    void draw();

    void setWindow(sf::RenderWindow* window);
    void setPosition(int x, int y);
    void setGlobalPosition(int x, int y);
    void setState(CellState state);

    void initShapes();

    void ifHovered(const sf::Vector2f& mousePos);
    void update(float dt);

    bool isHovered(const sf::Vector2f& mousePos);

    void setHighlightState(HighlightState state);

    int getX() const { return x; }
    int getY() const { return y; }

    CellState getState() const { return state; }
    HighlightState getHighlightState() { return highlightState; }

    void setColors(CellState state);

    void startAnimation(int targetIndent, int targetSize);

    void setIndentsAndSize(int indents, int size) {
        this->indents = indents;
        this->hexagon_size = size;
        initShapes();
    }

    CellState state;

    sf::Color hoverColor;
    sf::Color normalColor;
    sf::Color currentColor;
    sf::Color oldColor;
    sf::Color targetColor;
 
    float animationTime = 0.0f;
private:


    int x;
    int y;

    int globalX;
    int globalY;

    sf::ConvexShape outerHexagon;
    sf::ConvexShape innerHexagon;
    sf::ConvexShape bgHexagon;

    sf::RenderWindow* window;

    bool hovered = false;
    bool oldHoveredState = false;  
    
    bool isAnimating = false;
    float animationDuration = 0.0f;
    int targetIndent = indents;
    int targetSize = hexagon_size;
    int oldIndent = indents;
    int oldSize = hexagon_size;

    HighlightState highlightState = HighlightState::None;
};


class Board
{
private:
    float sleepTime = 0;
    
    sf::RenderWindow& window;

    void selectCell(Cell& cell);

    Cell* selectedCell = nullptr;


    std::vector<Cell*> availableCellsForCloning;
    std::vector<Cell*> availableCellsForMoving;
    void highlightAvailableForCloningCells();
    void highlightAvailableForMovingCells();
    void clearHighlightedCells(std::vector<Cell*> cells);
    void clearHighlightedCells();

    void cloneToCell(Cell& cell);
    void moveToCell(Cell& cell);
    void capture(Cell& cell);

    
    void gameIsOver();
    
public:
    std::vector<Cell> getCellsWithState(CellState state) const;
    
    std::vector<std::vector<Cell>> cells;

    bool isPlayer1Turn = true;

    bool isGameOver = false;
    bool isPlayer1Win = false;
    bool singleGame = false;

    Board(sf::RenderWindow& window, bool singleGame);

    void draw();

    void handleEvent(const sf::Event& event);

    void update(float dt);

    void setIsPlayer1Turn(bool isPlayer1Turn);

    int cloneFromTo(Cell& from, Cell& to);
    int moveFromTo(Cell& from, Cell& to);
};


class BoardState {
public:
    BoardState(Board board) {
        cells.resize(9, std::vector<Cell>(9));
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                cells[i][j] = board.cells[i][j];
                cells[i][j].setState(board.cells[i][j].getState());
            }
        }
    };

    std::vector<std::vector<Cell>> cells;

    std::vector<std::pair<int, int>> getCellsWithState(CellState state) const;
    std::vector<std::pair<int, int>> getAvailableForCloningCells(int fromRow, int fromCol) const;
    std::vector<std::pair<int, int>> getAvailableForMovingCells(int fromRow, int fromCol) const;
    int cloneFromTo(int fromRow, int fromCol, int toRow, int toCol);
    int moveFromTo(int fromRow, int fromCol, int toRow, int toCol);
    void capture(Cell& cell);
};
