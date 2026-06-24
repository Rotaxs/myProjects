#pragma once

#include "logic/Block.h"
#include "utils/Config.h"

#include <QString>
#include <QColor>

static const Block ROTATION_SHAPE[7][4][4] = {
    // 0: Shape_I
    {
        {Block(0, 2), Block(1, 2), Block(2, 2), Block(3, 2)},
        {Block(2, 0), Block(2, 1), Block(2, 2), Block(2, 3)},
        {Block(0, 1), Block(1, 1), Block(2, 1), Block(3, 1)},
        {Block(1, 0), Block(1, 1), Block(1, 2), Block(1, 3)}
    },
    // 1: Shape_O
    {
        {Block(1, 1), Block(2, 1), Block(1, 2), Block(2, 2)},
        {Block(1, 1), Block(2, 1), Block(1, 2), Block(2, 2)},
        {Block(1, 1), Block(2, 1), Block(1, 2), Block(2, 2)},
        {Block(1, 1), Block(2, 1), Block(1, 2), Block(2, 2)}
    },
    // 2: Shape_T
    {
        {Block(1, 1), Block(0, 2), Block(1, 2), Block(2, 2)}, 
        {Block(1, 1), Block(1, 2), Block(2, 2), Block(1, 3)},
        {Block(0, 2), Block(1, 2), Block(2, 2), Block(1, 3)},
        {Block(1, 1), Block(0, 2), Block(1, 2), Block(1, 3)}
    },
    // 3: Shape_L
    {
        {Block(0, 1), Block(0, 2), Block(1, 2), Block(2, 2)}, 
        {Block(1, 1), Block(2, 1), Block(1, 2), Block(1, 3)},
        {Block(0, 2), Block(1, 2), Block(2, 2), Block(2, 3)},
        {Block(1, 1), Block(1, 2), Block(0, 3), Block(1, 3)}
    },
    // 4: Shape_J
    {
        {Block(2, 1), Block(0, 2), Block(1, 2), Block(2, 2)},
        {Block(1, 1), Block(1, 2), Block(1, 3), Block(2, 3)},
        {Block(0, 2), Block(1, 2), Block(2, 2), Block(0, 3)}, 
        {Block(0, 1), Block(1, 1), Block(1, 2), Block(1, 3)}
    },
    // 5: Shape_S
    {
        {Block(1, 1), Block(2, 1), Block(0, 2), Block(1, 2)},
        {Block(1, 1), Block(1, 2), Block(2, 2), Block(2, 3)},
        {Block(1, 2), Block(2, 2), Block(0, 3), Block(1, 3)},
        {Block(0, 1), Block(0, 2), Block(1, 2), Block(1, 3)}
    },
    // 6: Shape_Z
    {
        {Block(0, 1), Block(1, 1), Block(1, 2), Block(2, 2)},
        {Block(2, 1), Block(1, 2), Block(2, 2), Block(1, 3)},
        {Block(0, 2), Block(1, 2), Block(1, 3), Block(2, 3)},
        {Block(1, 1), Block(0, 2), Block(1, 2), Block(0, 3)}  
    }
};

class BlockGroup
{
public:
    enum class Shape
    {
        SHAPE_I,
        SHAPE_O,
        SHAPE_T,
        SHAPE_L,
        SHAPE_J,
        SHAPE_S,
        SHAPE_Z,   
    };
private:
    int m_posX, m_posY;
    Shape m_shape;
    Block m_blocks[4];
    QColor m_blockColor, m_frameColor;
    int m_rotate_state;
public:
    BlockGroup(int posX = 0, int posY = 0, Shape shape = Shape::SHAPE_I, int rotation_state = 0);
    ~BlockGroup();
    const Shape& getShape() const { return m_shape; };
    const int getRotateState() const { return m_rotate_state; }
    const int getPosX() const { return m_posX; }
    const int getPosY() const { return m_posY; }
    int toNextRotationState();
    void addPosBy(const int &dx, const int &dy);
    Block& operator[](const int &index) { return m_blocks[index]; }
    const Block& operator[](int index) const { return m_blocks[index]; }
    static QString getShapeName(const Shape &shape);
};
