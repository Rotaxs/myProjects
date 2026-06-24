#pragma once

#include "utils/Config.h"

#include <QColor>

class Block
{
public:
    
private:  
    QColor m_color;
    QColor m_frameColor;
    int m_posX, m_posY;
    int m_width;
    bool m_isOccupied;
public:
    Block(int posX = 0, int posY = 0, int width = Config::BLOCK_WIDTH,
        QColor color = Config::BOARD_BLOCK_COLOR, QColor frameColor = Config::BOARD_BLOCK_FRAME_COLOR,
        bool isOccupied = false
    );
    ~Block();

    int getX() const { return m_posX; }
    int getY() const { return m_posY; }
    int getWidth() const { return m_width; }
    QColor getColor() const { return m_color; }
    QColor getFrameColor() const { return m_frameColor; }
    bool isOccupied() const { return m_isOccupied; }
    Block& setPosX(const int &posX);
    Block& setPosY(const int &posY);
    Block& setOccupied(const bool &state);
    Block& addPosXBy(const int &dx);
    Block& addPosYBy(const int &dy);
    Block& setColor(const QColor &color);
    Block& setFrameColor(const QColor &color);
    Block& operator=(const Block &other);
};