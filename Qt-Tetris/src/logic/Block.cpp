#include "Block.h"

#include <QDebug>

Block::Block(int posX, int posY, int width, QColor color, QColor frameColor, bool isOccupied): 
    m_posX(posX), m_posY(posY), m_width(width), m_color(color), m_frameColor(frameColor), m_isOccupied(isOccupied)
{
    
}

Block::~Block() { }

Block& Block::setColor(const QColor &color)
{
    m_color = color;
    return *this;
}

Block& Block::setFrameColor(const QColor &color)
{
    m_frameColor = color;
    return *this;
}

Block& Block::setPosX(const int &posX)
{
    m_posX = posX;
    return *this;
}

Block& Block::setPosY(const int &posY)
{
    m_posY = posY;
    return *this;
}

Block& Block::setOccupied(const bool &state)
{
    m_isOccupied = state;
    return *this;
}

Block& Block::addPosXBy(const int &dx)
{
    setPosX(m_posX + dx);
    return *this;
}

Block& Block::addPosYBy(const int &dy)
{
    setPosY(m_posY + dy);
    return *this;
}

Block& Block::operator=(const Block &other)
{
    m_posX = other.m_posX;
    m_posY = other.m_posY;
    m_width = other.m_width;
    m_color = other.m_color;
    m_frameColor = other.m_frameColor;
    m_isOccupied = other.m_isOccupied;
    return *this;
}
