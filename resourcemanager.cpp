#include "resourcemanager.h"
#include <QCoreApplication>
#include <QDir>

QString ResourceManager::getExecutableDir()
{
    return QCoreApplication::applicationDirPath();
}

QString ResourceManager::getImagePath(const QString &fileName)
{
    return getExecutableDir() + "/assets/images/" + fileName;
}

QString ResourceManager::getVideoPath(const QString &fileName)
{
    return getExecutableDir() + "/assets/videos/" + fileName;
}

QString ResourceManager::getAudioPath(const QString &fileName)
{
    return getExecutableDir() + "/assets/audio/" + fileName;
}
