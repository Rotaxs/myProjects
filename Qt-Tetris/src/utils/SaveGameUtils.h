#pragma once

#include "logic/BlockGroup.h"

#include <QColor>
#include <QString>
#include <QVector>

namespace SaveGameUtils
{
struct SavedCell
{
    int row;
    int col;
    QColor color;
    QColor frameColor;
};

struct SavedGameData
{
    int version = 1;
    int rows = 0;
    int cols = 0;
    int score = 0;
    int elapsedMs = 0;
    int fallGap = 0;
    QString stateName;
    BlockGroup currentGroup;
    BlockGroup nextGroup;
    QVector<SavedCell> boardCells;
};

QString defaultSaveFilePath();
bool writeSaveFile(const QString &path, const SavedGameData &data);
bool readSaveFile(const QString &path, SavedGameData &data);
bool clearSaveFile(const QString &path);
}
