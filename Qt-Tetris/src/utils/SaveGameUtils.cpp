#include "utils/SaveGameUtils.h"

#include "utils/Config.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QStandardPaths>

namespace SaveGameUtils
{
/**
 * @brief 将 BlockGroup 转为 json 对象
 * @param group 要转化的 blockGroup
 * @return 转化后的 json 对象
 */
QJsonObject serializeGroup(const BlockGroup &group)
{
    QJsonObject obj;
    obj["shape"] = static_cast<int>(group.getShape());
    obj["rotation"] = group.getRotateState();
    obj["posX"] = group.getPosX();
    obj["posY"] = group.getPosY();
    return obj;
}

/**
 * @brief 将 json 对象转化为 BlockGroup 对象
 * @param obj 要转化的 json 对象
 * @param group 接收转化后的 BlockGroup 对象
 * @return 如果 json 数据不完整或不合法，则转化失败，返回 false，否则返回 true
 */
bool deserializeGroup(const QJsonObject &obj, BlockGroup &group)
{
    if (!obj.contains("shape") || !obj.contains("rotation") ||
        !obj.contains("posX") || !obj.contains("posY"))
        return false;

    // toInt: 如果数据不完整，返回 -1 或默认的 0
    const int shape = obj.value("shape").toInt(-1);
    const int rotation = obj.value("rotation").toInt(-1);
    const int posX = obj.value("posX").toInt();
    const int posY = obj.value("posY").toInt();

    if (shape < 0 || shape >= 7 || rotation < 0 || rotation >= 4)
        return false;

    group = BlockGroup(
        posX,
        posY,
        static_cast<BlockGroup::Shape>(shape),
        rotation
    );
    return true;
}

/**
 * @brief 获取默认的保存路径，如果是调试模式，就在项目根目录下的 doc/ 下创建存档，否则在电脑的安全路径下创建存档
 * @return 返回保存存档的路径
 */
QString defaultSaveFilePath()
{
#ifdef QT_DEBUG
    // ======================= Debug 模式 ==========================
    // 获取可执行程序的绝对路径
    QDir dir(QCoreApplication::applicationDirPath());
    for (int depth = 0; depth < 6; depth++) {
        if (QFileInfo::exists(dir.filePath("CMakeLists.txt")) && dir.exists("src"))
            break;
        if (!dir.cdUp())
            break;
    }
    dir.mkpath("docs");
    return dir.filePath("docs/savegame.json");
#else
    // ======================== Release 模式 ======================
    // 获取系统安全应用目录
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    QDir dir(appDataPath);
    // 如果路径不存在，就创景路径
    if (!dir.exists()) dir.mkpath(".");
    return dir.filePath("savegame.json");
#endif
}

/**
 * @brief 将 SavedGameData 对象转化为 json 对象并写入文件
 * @param path 存盘的位置
 * @param date 要写入的数据
 * @return 写入成功返回 true，否则返回 false
 */
bool writeSaveFile(const QString &path, const SavedGameData &data)
{
    // 将 SavedGameData 转化为 json 对象
    QJsonObject root;
    root["version"] = data.version;
    root["rows"] = data.rows;
    root["cols"] = data.cols;
    root["score"] = data.score;
    root["elapsedMs"] = data.elapsedMs;
    root["fallGap"] = data.fallGap;
    root["state"] = data.stateName;
    root["currentGroup"] = serializeGroup(data.currentGroup);
    root["nextGroup"] = serializeGroup(data.nextGroup);
    QJsonArray boardCells;
    for (const SavedCell &cell : data.boardCells)
    {
        QJsonObject cellObj;
        cellObj["row"] = cell.row;
        cellObj["col"] = cell.col;
        cellObj["color"] = cell.color.name(QColor::HexRgb);
        cellObj["frameColor"] = cell.frameColor.name(QColor::HexRgb);
        boardCells.append(cellObj);
    }
    root["boardCells"] = boardCells;

    QSaveFile file(path);
    // 以只写和文本模式打开文件，如果打开失败会返回 false
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    // 将内存中的二进制 json 对象转化为 json 文本
    // QJsonDocument(root) 表示将 json 二进制流装进 json 文本工具中，进而调用 toJson() 方法
    const QByteArray json = QJsonDocument(root).toJson(QJsonDocument::Indented);
    file.write(json);
    // 如果成功保存，返回 true，否则返回 false，并且会自动 file.close()
    return file.commit();
}

/**
 * @brief 读取 json 文件中的数据，并将其转化为 SavedGameDate 对象
 * @param path 读盘的位置
 * @param data 接收数据的对象
 * @return 读取成功返回 true，否则返回 false
 */
bool readSaveFile(const QString &path, SavedGameData &data)
{
    QFile file(path);
    if (!file.exists()) return false;
    // 以只读的方式打开
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    // 将 json 数据转为内存中的二进制流
    const QByteArray raw = file.readAll();
    file.close();
    // 判断是否为空，trimmed 清空所有的空白字符
    if (raw.trimmed().isEmpty())
        return false;
    // error 用于记录错误信息
    QJsonParseError error;
    // 获取到 json 数据的文本工具，同时将错误信息记录到 error
    const QJsonDocument doc = QJsonDocument::fromJson(raw, &error);
    // 如果有错误数据，就返回 false
    if (error.error != QJsonParseError::NoError || !doc.isObject())
        return false;
    // 获取真正的 json 对象
    const QJsonObject root = doc.object();
    data.version = root.value("version").toInt();
    data.rows = root.value("rows").toInt();
    data.cols = root.value("cols").toInt();
    data.score = root.value("score").toInt();
    data.elapsedMs = root.value("elapsedMs").toInt();
    data.fallGap = root.value("fallGap").toInt();
    data.stateName = root.value("state").toString();
    // 将 json 对象转化为 BlockGroup 对象
    if (!deserializeGroup(root.value("currentGroup").toObject(), data.currentGroup) ||
        !deserializeGroup(root.value("nextGroup").toObject(), data.nextGroup))
        return false;
    // 获取锁定的方块的 QVector
    data.boardCells.clear();
    // 先将数据转化为 json 数组
    const QJsonArray boardCells = root.value("boardCells").toArray();
    // 预留空间，性能优化
    data.boardCells.reserve(boardCells.size());
    for (const QJsonValue &value : boardCells)
    {
        if (!value.isObject())
            return false;
        // 解析数组中的数据
        const QJsonObject cellObj = value.toObject();
        const QColor color(cellObj.value("color").toString());
        const QColor frameColor(cellObj.value("frameColor").toString());
        data.boardCells.append({
            cellObj.value("row").toInt(-1),
            cellObj.value("col").toInt(-1),
            color.isValid() ? color : QColor(Config::BOARD_BLOCK_COLOR),
            frameColor.isValid() ? frameColor : QColor(Config::BOARD_BLOCK_FRAME_COLOR)
        });
    }

    return true;
}

/**
 * @brief 清空文件
 * @param path 文件路径
 */
bool clearSaveFile(const QString &path)
{
    QFile file(path);
    // Truncate 清空文件
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        return false;
    file.close();
    return true;
}
}
