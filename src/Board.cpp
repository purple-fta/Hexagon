#include <cmath>
#include <iostream>

#include "Board.h"
#include "pallete.h"
#include "ai.h"

namespace {
    int outlineThickness = 3;
    float animation_duration = 0.2f;
}

void Cell::draw() {
    window->draw(bgHexagon);
    window->draw(outerHexagon);
    window->draw(innerHexagon);
}

void Cell::setWindow(sf::RenderWindow* window) {
    this->window = window;
};
void Cell::setPosition(int x, int y) {
    this->x = x;
    this->y = y;
}
void Cell::setGlobalPosition(int x, int y) {
    this->globalX = x;
    this->globalY = y;
}
void Cell::setState(CellState state) {
    this->state = state;
}

void Cell::initShapes() {
    outerHexagon.setPointCount(6);
    innerHexagon.setPointCount(6);
    bgHexagon.setPointCount(6);

    for (int i = 0; i < 6; ++i) {
        float angle = i * 2 * 3.14159f / 6;
        outerHexagon.setPoint(i, {globalX + hexagon_size * static_cast<float>(cos(angle)), globalY + hexagon_size * static_cast<float>(sin(angle))});
        innerHexagon.setPoint(i, {globalX + (hexagon_size - indents) * static_cast<float>(cos(angle)), globalY + (hexagon_size - indents) * static_cast<float>(sin(angle))});
        bgHexagon.setPoint(i, {globalX + 35 * static_cast<float>(cos(angle)), globalY + 35 * static_cast<float>(sin(angle))});
    }

    setColors(state);
}

void Cell::ifHovered(const sf::Vector2f& mousePos) {
    hovered = isHovered(mousePos);

    if (hovered != oldHoveredState) {
        oldColor = currentColor;
        animationTime = 0.0f;
        targetColor = hovered ? hoverColor : normalColor;
        oldHoveredState = hovered;
    }
}

void Cell::update(float dt) {
    animationTime += dt;
    float t = animationTime / animation_duration;

    if (t > 1.0f) {
        t = 1.0f;
        sf::Color oldTargetColor = targetColor;
        sf::Color oldOuterColor = outerHexagon.getFillColor();
        setColors(state);
        targetColor = oldTargetColor;
        outerHexagon.setFillColor(oldOuterColor);
    }

    currentColor.r = (int)((1 - t) * oldColor.r + t * targetColor.r);
    currentColor.g = (int)((1 - t) * oldColor.g + t * targetColor.g);
    currentColor.b = (int)((1 - t) * oldColor.b + t * targetColor.b);

    if (highlightState == HighlightState::None)
        outerHexagon.setFillColor(currentColor);
    
    innerHexagon.setFillColor(currentColor);

    if (isAnimating) {
        animationDuration += dt;
        t = animationDuration / animation_duration;
        if (t > 1.0f) {
            t = 1.0f;
            isAnimating = false;
        } else if (t < 0.0f) {
            t = 0.0f;
        }

        int newIndents = (int)(1 - t) * oldIndent + t * targetIndent;
        int newSize = (int)(1 - t) * oldSize + t * targetSize;

        setIndentsAndSize(newIndents, newSize);
    }
}

void Cell::startAnimation(int targetIndent, int targetSize) {
    this->targetIndent = targetIndent;
    this->targetSize = targetSize;
    animationDuration = 0.0f;
    isAnimating = true;
    oldIndent = indents;
    oldSize = hexagon_size;
}

void Cell::setColors(CellState state) {
    switch (state) {
        case CellState::Empty:
            normalColor = Palette::emptyColor;
            hoverColor = Palette::emptyHoverColor;
            break;
        case CellState::Player1:
            normalColor = Palette::p1Color;
            hoverColor = Palette::p1HoverColor;
            break;
        case CellState::Player2:
            normalColor = Palette::p2Color;
            hoverColor = Palette::p2HoverColor;
            break;
        case CellState::Blocked:
            normalColor = Palette::Background;
            hoverColor = Palette::Background;
            break;
    }
    
    currentColor = normalColor;
    oldColor = normalColor;
    targetColor = normalColor;

    outerHexagon.setFillColor(normalColor);
    // outerHexagon.setOutlineThickness(outlineThickness);
    // outerHexagon.setOutlineColor(Palette::Background);

    innerHexagon.setFillColor(normalColor);

    bgHexagon.setFillColor(Palette::Surface0);
}

bool Cell::isHovered(const sf::Vector2f& mousePos) {
    float dx = mousePos.x - globalX;
    float dy = mousePos.y - globalY;
    float dist = std::sqrt(dx*dx + dy*dy);

    return dist < hexagon_size;
}

void Cell::setHighlightState(HighlightState state) {
    if (state == HighlightState::None) {
        outerHexagon.setFillColor(normalColor);
    } else if (state == HighlightState::Selected) {
        outerHexagon.setFillColor(Palette::selectedCell);
    } else if (state == HighlightState::AvailableForCloning) {
        outerHexagon.setFillColor(Palette::Sky);
    } else if (state == HighlightState::AvailableForMoving) {
        outerHexagon.setFillColor(Palette::Green);
    }
        
    highlightState = state;
}




Board::Board(sf::RenderWindow& window, bool singleGame) : window(window) {

    // 0 - empty, 1 - player1, 2 - player2, 3 - blocked
    std::vector<int> board = {
        3, 3, 3, 0, 2, 0, 3, 3, 3,
        3, 0, 0, 0, 0, 0, 0, 0, 3,
        1, 0, 0, 0, 0, 0, 0, 0, 1,
        0, 0, 0, 0, 3, 0, 0, 0, 0,
        0, 0, 0, 3, 0, 3, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        2, 0, 0, 0, 0, 0, 0, 0, 2,
        3, 3, 0, 0, 0, 0, 0, 3, 3,
        3, 3, 3, 3, 1, 3, 3, 3, 3
    };

    cells.resize(9, std::vector<Cell>(9));
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            cells[row][col].setWindow(&window);
            cells[row][col].setPosition(col, row);
            
            switch (board[row * 9 + col]) {
                case 0:
                    cells[row][col].setState(CellState::Empty);
                    break;
                case 1:
                    cells[row][col].setState(CellState::Player1);
                    break;
                case 2:
                    cells[row][col].setState(CellState::Player2);
                    break;
                case 3:
                    cells[row][col].setState(CellState::Blocked);
                    break;
            }
            
            int hexagon_size = 35 + outlineThickness*2;

            float hexWidth = hexagon_size * 2;
            float hexHeight = hexagon_size * sqrt(3);

            int startBoardX = (window.getSize().x - hexagon_size * 9) / 2 - hexagon_size * 9 / 2 * 0.25f;
            int startBoardY = (window.getSize().y - hexHeight * 9) / 2 + 37;

            int globalX = startBoardX + col * hexWidth;
            int globalY = startBoardY + row * hexHeight;

            if (col % 2 == 1) {
                globalY += hexHeight / 2;
                globalX -= hexWidth * 0.25f;
            }

            globalX -= hexWidth / 2 * (col / 2);

            cells[row][col].setGlobalPosition(globalX, globalY);

            cells[row][col].initShapes();
        }
    }

    this->singleGame = singleGame;
}

void Board::draw() {
    for (int row = 0; row < 9; row++) {
        for (int col = 0; col < 9; col++) {
            cells[row][col].draw();
        }
    }
}

void Board::handleEvent(const sf::Event& event) {
    if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>()) {
        for (auto& cellInRow : cells) {
            for (auto& cell : cellInRow) {
                cell.ifHovered({static_cast<float>(mouseMoved->position.x), 
                                static_cast<float>(mouseMoved->position.y)});
            }
        }
    } else if (const auto* mouseButton = event.getIf<sf::Event::MouseButtonPressed>()) { 
        if (mouseButton->button == sf::Mouse::Button::Left) {
            for (auto& cellInRow : cells) {
                for (auto& cell : cellInRow) {
                    if (cell.isHovered({static_cast<float>(mouseButton->position.x), 
                                        static_cast<float>(mouseButton->position.y)})) {
                        switch (cell.getHighlightState())
                        {
                            case HighlightState::None:
                                selectCell(cell);
                                if (selectedCell != nullptr) {
                                    highlightAvailableForCloningCells();
                                    highlightAvailableForMovingCells();
                                }
                                break;
                            case HighlightState::Selected:
                                cell.setHighlightState(HighlightState::None);
                                clearHighlightedCells();
                                break;
                            case HighlightState::AvailableForCloning:
                                cloneToCell(cell);
                                
                                if (singleGame) {
                                    isPlayer1Turn = !isPlayer1Turn;
                                    sleepTime = 0.0f;
                                } else {
                                    isPlayer1Turn = !isPlayer1Turn;
                                    gameIsOver();
                                }
                                
                                break;
                            case HighlightState::AvailableForMoving:
                                moveToCell(cell);
                                
                                if (singleGame) {
                                    isPlayer1Turn = !isPlayer1Turn;
                                    sleepTime = 0.0f;
                                } else {
                                    isPlayer1Turn = !isPlayer1Turn;
                                    gameIsOver();
                                }
                                break;
                        }                        
                    }
                }
            }
        }
    }
}

void Board::update(float dt) {
    for (auto& cellInRow : cells) {
        for (auto& cell : cellInRow) {
            cell.update(dt);
        }
    }

    if (!isPlayer1Turn && singleGame) {
        if (sleepTime > 0.85f) {
            doBestMove(*this);
            isPlayer1Turn = !isPlayer1Turn;
            gameIsOver();
            sleepTime = 0.0f;
        } else {
            sleepTime += dt;
        }
    }
}

void Board::setIsPlayer1Turn(bool isPlayer1Turn) {
    this->isPlayer1Turn = isPlayer1Turn;
}

void Board::selectCell(Cell& cell) {
    if (cell.getState() == (isPlayer1Turn ? CellState::Player1 : CellState::Player2)) {
        cell.setHighlightState(HighlightState::Selected);
        if (selectedCell != nullptr) {
            selectedCell->setHighlightState(HighlightState::None);
            clearHighlightedCells(availableCellsForCloning);
            clearHighlightedCells(availableCellsForMoving);
        }
        selectedCell = &cell;
    } else {
        if (selectedCell != nullptr) {
            selectedCell->setHighlightState(HighlightState::None);
            clearHighlightedCells(availableCellsForCloning);
            clearHighlightedCells(availableCellsForMoving);
        }
        selectedCell = nullptr;
    }
}

void Board::highlightAvailableForCloningCells() {
    if (selectedCell == nullptr) return;

    int x = selectedCell->getX();
    int y = selectedCell->getY();

    std::vector<Cell*> availableCells;

    if (y-1 >= 0 && cells[y-1][x].getState() == CellState::Empty) {
        availableCells.push_back(&cells[y-1][x]);   
    }
    if (y+1 < 9 && cells[y+1][x].getState() == CellState::Empty) {
        availableCells.push_back(&cells[y+1][x]);   
    }
    if (x+1 < 9 && cells[y][x+1].getState() == CellState::Empty) {
        availableCells.push_back(&cells[y][x+1]);   
    }
    if (x-1 >= 0 && cells[y][x-1].getState() == CellState::Empty) {
        availableCells.push_back(&cells[y][x-1]);   
    }
    int dy = (x % 2 == 0) ? -1 : 1;
    if (y+dy >= 0 && y+dy < 9 && x-1 >= 0 && cells[y+dy][x-1].getState() == CellState::Empty) {
        availableCells.push_back(&cells[y+dy][x-1]);   
    }
    if (y+dy >= 0 && y+dy < 9 && x+1 < 9 && cells[y+dy][x+1].getState() == CellState::Empty) {
        availableCells.push_back(&cells[y+dy][x+1]);   
    }

    for (auto* cell : availableCells) {
        cell->setHighlightState(HighlightState::AvailableForCloning);
    }

    availableCellsForCloning = availableCells;

}

void Board::highlightAvailableForMovingCells() {
    if (selectedCell == nullptr) return;

    int c_x = selectedCell->getX();
    int c_y = selectedCell->getY();

    std::vector<Cell*> availableCells;

    int dy;
    for (int x = c_x-1; x <= c_x+1; x++)
    {
        if (x >= 0 && x < 9)
        {
            dy = (c_x % 2 == 0 && x % 2 == 1) ? 1 : 0;
            if (c_y+2-dy < 9 && cells[c_y+2-dy][x].getState() == CellState::Empty)
            {
                availableCells.push_back(&cells[c_y+2-dy][x]);
            }
            dy = (c_x % 2 == 1 && x % 2 == 0) ? 1 : 0;
            if (c_y-2+dy >= 0 && c_y-2+dy < 9 && cells[c_y-2+dy][x].getState() == CellState::Empty)
            {
                availableCells.push_back(&cells[c_y-2+dy][x]);
            }
        }
    }
    for (int y = c_y-1; y <= c_y+1; y++)
    {
        if (y >= 0 && y < 9)
        {
            if (c_x+2 < 9 && cells[y][c_x+2].getState() == CellState::Empty)
            {
                availableCells.push_back(&cells[y][c_x+2]);
            }
            if (c_x-2 >= 0 && cells[y][c_x-2].getState() == CellState::Empty)
            {
                availableCells.push_back(&cells[y][c_x-2]);
            }
        }
    }
    

    for (auto* cell : availableCells) {
        cell->setHighlightState(HighlightState::AvailableForMoving);
    }

    availableCellsForMoving = availableCells;
}

void Board::clearHighlightedCells(std::vector<Cell*> cells) {
    for (auto* cell : cells) {
        cell->setHighlightState(HighlightState::None);
    }

    cells.clear();
}

void Board::cloneToCell(Cell& cell) {
    if (selectedCell == nullptr) return;

    if (cell.getState() == CellState::Empty) {
        cell.setState(selectedCell->getState());
        cell.setColors(selectedCell->getState()); 
        selectedCell->setHighlightState(HighlightState::None);
        clearHighlightedCells();
        
        capture(cell);

        cell.setIndentsAndSize(0, 0);
        cell.startAnimation(5, 35);
    }
}

void Board::moveToCell(Cell& cell) {
    if (selectedCell == nullptr) return;

    if (cell.getState() == CellState::Empty) {
        cell.setState(selectedCell->getState());
        cell.setColors(selectedCell->getState());
        
        selectedCell->setState(CellState::Empty);
        selectedCell->setColors(CellState::Empty);
        selectedCell->setHighlightState(HighlightState::None);
        
        clearHighlightedCells();
        capture(cell);

        cell.setIndentsAndSize(0, 0);
        cell.startAnimation(5, 35);
    }
}

void Board::capture(Cell& cell) {
    int x = cell.getX();
    int y = cell.getY();
    CellState target = cell.getState() == CellState::Player1 ? CellState::Player2 : CellState::Player1;

    std::vector<Cell*> availableCells;

    if (y-1 >= 0 && cells[y-1][x].getState() == target) {
        availableCells.push_back(&cells[y-1][x]);
    }
    if (y+1 < 9 && cells[y+1][x].getState() == target) {
        availableCells.push_back(&cells[y+1][x]);
    }
    if (x+1 < 9 && cells[y][x+1].getState() == target) {
        availableCells.push_back(&cells[y][x+1]);
    }
    if (x-1 >= 0 && cells[y][x-1].getState() == target) {
        availableCells.push_back(&cells[y][x-1]);
    }
    int dy = (x % 2 == 0) ? -1 : 1;
    if (y+dy >= 0 && y+dy < 9 && x-1 >= 0 && cells[y+dy][x-1].getState() == target) {
        availableCells.push_back(&cells[y+dy][x-1]);   
    }
    if (y+dy >= 0 && y+dy < 9 && x+1 < 9 && cells[y+dy][x+1].getState() == target) {
        availableCells.push_back(&cells[y+dy][x+1]);   
    }

    for (auto* cell : availableCells) {
        cell->oldColor = cell->currentColor;
        cell->animationTime = 0.0f;
        cell->targetColor = isPlayer1Turn ? Palette::p1Color : Palette::p2Color;
        
        cell->setState( isPlayer1Turn ? CellState::Player1 : CellState::Player2 );
    }
}

void Board::clearHighlightedCells() {
    clearHighlightedCells(availableCellsForCloning);
    clearHighlightedCells(availableCellsForMoving);
    selectedCell = nullptr;
}

std::vector<Cell> Board::getCellsWithState(CellState state) const {
    std::vector<Cell> playerCells;

    for (auto& cellInRow : cells) {
        for (auto& cell : cellInRow) {
            if (cell.getState() == state) {
                playerCells.push_back(cell);
            }
        }
    }
    return playerCells;
}

void Board::gameIsOver() {
    int p1CellsCount = getCellsWithState(CellState::Player1).size();
    int p2CellsCount = getCellsWithState(CellState::Player2).size();
    int emptyCellsCount = getCellsWithState(CellState::Empty).size();

    if (p1CellsCount == 0 || p2CellsCount == 0 || emptyCellsCount == 0) {
        if (p1CellsCount > p2CellsCount) {
            isPlayer1Win = true;
        } else {
            isPlayer1Win = false;
        }

        isGameOver = true;
    }
}

int Board::cloneFromTo(Cell& from, Cell& to) {
    int oldCount = getCellsWithState(from.getState()).size();
    
    selectedCell = &from;
    
    cloneToCell(to);
    
    int newCount = getCellsWithState(to.getState()).size();
    
    return newCount - oldCount;
}

int Board::moveFromTo(Cell& from, Cell& to) {
    int oldCount = getCellsWithState(from.getState()).size();
    
    selectedCell = &from;
    
    moveToCell(to);
    
    capture(to);

    int newCount = getCellsWithState(to.getState()).size();
    
    return newCount - oldCount;
}



std::vector<std::pair<int, int>> BoardState::getCellsWithState(CellState state) const {
    std::vector<std::pair<int, int>> playerCells;

    for (auto& cellInRow : cells) {
        for (auto& cell : cellInRow) {
            if (cell.getState() == state) {
                playerCells.push_back(std::make_pair(cell.getY(), cell.getX()));
            }
        }
    }
    return playerCells;
}

std::vector<std::pair<int, int>> BoardState::getAvailableForCloningCells(int fromRow, int fromCol) const {
    int x = fromCol;
    int y = fromRow;

    std::vector<std::pair<int, int>> availableCells;

    if (y-1 >= 0 && cells[y-1][x].getState() == CellState::Empty) {
        availableCells.push_back(std::make_pair(y-1, x));   
    }
    if (y+1 < 9 && cells[y+1][x].getState() == CellState::Empty) {
        availableCells.push_back(std::make_pair(y+1, x));   
    }
    if (x+1 < 9 && cells[y][x+1].getState() == CellState::Empty) {
        availableCells.push_back(std::make_pair(y, x+1));   
    }
    if (x-1 >= 0 && cells[y][x-1].getState() == CellState::Empty) {
        availableCells.push_back(std::make_pair(y, x-1));   
    }
    int dy = (x % 2 == 0) ? -1 : 1;
    if (y+dy >= 0 && y+dy < 9 && x-1 >= 0 && cells[y+dy][x-1].getState() == CellState::Empty) {
        availableCells.push_back(std::make_pair(y+dy, x-1));   
    }
    if (y+dy >= 0 && y+dy < 9 && x+1 < 9 && cells[y+dy][x+1].getState() == CellState::Empty) {
        availableCells.push_back(std::make_pair(y+dy, x+1));   
    }

    return availableCells;
}

std::vector<std::pair<int, int>> BoardState::getAvailableForMovingCells(int fromRow, int fromCol) const {
    int c_y = fromRow;
    int c_x = fromCol;

    std::vector<std::pair<int, int>> availableCells;

    int dy;
    for (int x = c_x-1; x <= c_x+1; x++)
    {
        if (x >= 0 && x < 9)
        {
            dy = (c_x % 2 == 0 && x % 2 == 1) ? 1 : 0;
            if (c_y+2-dy < 9 && cells[c_y+2-dy][x].getState() == CellState::Empty)
            {
                availableCells.push_back(std::make_pair(c_y+2-dy, x));
            }
            dy = (c_x % 2 == 1 && x % 2 == 0) ? 1 : 0;
            if (c_y-2+dy >= 0 && c_y-2+dy < 9 && cells[c_y-2+dy][x].getState() == CellState::Empty)
            {
                availableCells.push_back(std::make_pair(c_y-2+dy, x));
            }
        }
    }
    for (int y = c_y-1; y <= c_y+1; y++)
    {
        if (y >= 0 && y < 9)
        {
            if (c_x+2 < 9 && cells[y][c_x+2].getState() == CellState::Empty)
            {
                availableCells.push_back(std::make_pair(y, c_x+2));
            }
            if (c_x-2 >= 0 && cells[y][c_x-2].getState() == CellState::Empty)
            {
                availableCells.push_back(std::make_pair(y, c_x-2));
            }
        }
    }

    return availableCells;
}

int BoardState::cloneFromTo(int fromRow, int fromCol, int toRow, int toCol) {
    Cell& from = cells[fromRow][fromCol];
    Cell& to = cells[toRow][toCol];

    int oldCount = getCellsWithState(from.getState()).size();
    
    if (from.getState() == CellState::Player1) {
        to.setState(CellState::Player1);
    } else {
        to.setState(CellState::Player2);
    }

    capture(to);

    int newCount = getCellsWithState(to.getState()).size();
    
    return newCount - oldCount;
}

int BoardState::moveFromTo(int fromRow, int fromCol, int toRow, int toCol) {
    Cell& from = cells[fromRow][fromCol];
    Cell& to = cells[toRow][toCol];
    
    int oldCount = getCellsWithState(from.getState()).size();
    
    if (from.getState() == CellState::Player1) {
        to.setState(CellState::Player1);
    } else {
        to.setState(CellState::Player2);
    }
    from.setState(CellState::Empty);
    
    capture(to);

    int newCount = getCellsWithState(to.getState()).size();
    
    return newCount - oldCount;
}

void BoardState::capture(Cell& cell) {
    int x = cell.getX();
    int y = cell.getY();
    CellState target = CellState::Player1;

    std::vector<Cell*> availableCells;

    if (y-1 >= 0 && cells[y-1][x].getState() == target) {
        availableCells.push_back(&cells[y-1][x]);
    }
    if (y+1 < 9 && cells[y+1][x].getState() == target) {
        availableCells.push_back(&cells[y+1][x]);
    }
    if (x+1 < 9 && cells[y][x+1].getState() == target) {
        availableCells.push_back(&cells[y][x+1]);
    }
    if (x-1 >= 0 && cells[y][x-1].getState() == target) {
        availableCells.push_back(&cells[y][x-1]);
    }
    int dy = (x % 2 == 0) ? -1 : 1;
    if (y+dy >= 0 && y+dy < 9 && x-1 >= 0 && cells[y+dy][x-1].getState() == target) {
        availableCells.push_back(&cells[y+dy][x-1]);   
    }
    if (y+dy >= 0 && y+dy < 9 && x+1 < 9 && cells[y+dy][x+1].getState() == target) {
        availableCells.push_back(&cells[y+dy][x+1]);   
    }

    for (auto* cell : availableCells) {
        cell->setState( CellState::Player2 );
    }
}
