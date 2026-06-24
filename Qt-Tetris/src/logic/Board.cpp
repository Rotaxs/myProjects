#include "logic/Board.h"

BoardRow::BoardRow(int col, int blockWidth, QColor blockColor, QColor frameColor) : m_col(col)
{
    m_boardRow.resize(col);
    for (int c = 0; c < col; c++)
    {
        m_boardRow[c] = Block(c, 0, blockWidth, blockColor, frameColor);
    }
}

BoardRow::~BoardRow() { }

Block& BoardRow::operator[](const int &index)
{
    return m_boardRow[index];
}

const Block& BoardRow::operator[](const int &index) const
{
    return m_boardRow[index];
}

Board::Board(int row, int col, int blockWidth, QColor blockColor, QColor frameColor) : m_row(row), m_col(col)
{
    m_board.resize(row);
    for (int r = 0; r < row; r++)
    {
        m_board[r] = BoardRow(col, blockWidth, blockColor, frameColor);
        for (int c = 0; c < col; c++)
        {
            m_board[r][c].setPosY(r);
        }
    }
}

Board::~Board() { }

BoardRow& Board::operator[](const int &index)
{
    return m_board[index];
}

const BoardRow& Board::operator[](const int &index) const
{
    return m_board[index];
}
