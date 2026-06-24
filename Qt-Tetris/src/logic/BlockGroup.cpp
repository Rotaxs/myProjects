#include "logic/BlockGroup.h"

BlockGroup::BlockGroup(int posX, int posY, Shape shape, int rotate_state) :
    m_posX(posX), m_posY(posY), m_shape(shape), m_rotate_state(rotate_state)
{    
    switch (m_shape)
    {
    case Shape::SHAPE_I: m_blockColor = QColor(Config::SHAPE_I_COLOR); break;
    case Shape::SHAPE_O: m_blockColor = QColor(Config::SHAPE_O_COLOR); break;
    case Shape::SHAPE_T: m_blockColor = QColor(Config::SHAPE_T_COLOR); break;
    case Shape::SHAPE_L: m_blockColor = QColor(Config::SHAPE_L_COLOR); break;
    case Shape::SHAPE_J: m_blockColor = QColor(Config::SHAPE_J_COLOR); break;
    case Shape::SHAPE_S: m_blockColor = QColor(Config::SHAPE_S_COLOR); break;
    case Shape::SHAPE_Z: m_blockColor = QColor(Config::SHAPE_Z_COLOR); break;
    default: m_blockColor = Qt::white; break;
    }
    m_frameColor = Config::BLOCK_FRAME_COLOR;
    for (int i = 0; i < 4; i++)
    {
        m_blocks[i] = ROTATION_SHAPE[static_cast<int>(m_shape)][m_rotate_state][i];
        m_blocks[i].setColor(m_blockColor).setFrameColor(m_frameColor);
        m_blocks[i].addPosXBy(m_posX).addPosYBy(m_posY);
        m_blocks[i].setOccupied(true);
    }
}

BlockGroup::~BlockGroup() { };

/**
 * @brief 获取形状的名称
 * @param shape 形状的枚举值
 * @return 形状的名称
 */
QString BlockGroup::getShapeName(const Shape &shape)
{
    QString shapeName;
    switch (shape)
    {
    case Shape::SHAPE_I: shapeName = "Shape_I"; break;
    case Shape::SHAPE_O: shapeName = "Shape_O"; break;
    case Shape::SHAPE_T: shapeName = "Shape_T"; break;
    case Shape::SHAPE_L: shapeName = "Shape_L"; break;
    case Shape::SHAPE_J: shapeName = "Shape_J"; break;
    case Shape::SHAPE_S: shapeName = "Shape_S"; break;
    case Shape::SHAPE_Z: shapeName = "Shape_Z"; break;
    }
    return shapeName;
}

/**
 * @brief 将当前的 BlockGroup 对象的旋转状态转为一下个旋转状态
 * @return 旋转的状态 (0, 1, 2, 3)
 */
int BlockGroup::toNextRotationState()
{
    int shape_id = static_cast<int>(m_shape);
    int nextRotationState = (m_rotate_state + 1) % 4;
    for (int i = 0; i < 4; i++)
    {
        const Block &block = ROTATION_SHAPE[shape_id][nextRotationState][i];
        m_blocks[i].setPosX(m_posX + block.getX());
        m_blocks[i].setPosY(m_posY + block.getY());
    }
    return m_rotate_state = nextRotationState;
}

/**
 * @brief 移动当前的 BlockGroup
 * @param dx, dy 方向向量
 */
void BlockGroup::addPosBy(const int &dx, const int &dy)
{
    m_posX += dx;
    m_posY += dy;
    for (int i = 0; i < 4; i++)
    {
        m_blocks[i].addPosXBy(dx);
        m_blocks[i].addPosYBy(dy);
    }
}
