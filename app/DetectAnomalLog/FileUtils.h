#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <QString>
#include <QStringList>

class FileUtils {
public:
    static bool cleanDirectory(const QString& path);
    static bool extractArchive(const QString& archivePath, const QString& destDir, QString& firstLogFileFound);
    static QString removeFilePrefix(const QString& path);
};

#endif // FILEUTILS_H
