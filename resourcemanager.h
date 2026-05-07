#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <QString>

class ResourceManager
{
public:
    static QString getImagePath(const QString &fileName);
    static QString getVideoPath(const QString &fileName);
    static QString getAudioPath(const QString &fileName);
    static QString getExecutableDir();
};

#endif // RESOURCEMANAGER_H
