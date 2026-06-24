#pragma once

#include "utils/Config.h"
#include "logic/Block.h"

#include <vector>
#include <QColor>

class BoardRow
{
private:
    int m_col;
    std::vector<Block> m_boardRow;
public:
    BoardRow(
        int col = Config::BOARD_COLS, int blockWidth = Config::BLOCK_WIDTH,
        QColor blockColor = Config::BOARD_BLOCK_COLOR, QColor lineColor = Config::BOARD_BLOCK_FRAME_COLOR
    );
    ~BoardRow();
    int getCols() const { return m_col; }
    Block& operator[](const int &index);
    const Block& operator[](const int &index) const;
};

class Board
{
private:
    std::vector<BoardRow> m_board;
    int m_row, m_col;
public:
    Board(
        int row = Config::BOARD_ROWS, int col = Config::BOARD_COLS, int blockWidth = Config::BLOCK_WIDTH,
        QColor blockColor = Config::BOARD_BLOCK_COLOR, QColor lineColor = Config::BOARD_BLOCK_FRAME_COLOR
    );
    ~Board();
    int getRows() const { return m_row; }
    int getCols() const { return m_col; }
    BoardRow& operator[](const int &index);
    const BoardRow& operator[](const int &index) const;
};
