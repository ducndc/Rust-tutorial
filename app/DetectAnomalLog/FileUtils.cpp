#include "FileUtils.h"

#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QDebug>

QString
FileUtils::removeFilePrefix(
    const QString& path)
{
    return path.startsWith("file://") ? path.mid(7) : path;
}

bool
FileUtils::cleanDirectory(
    const QString& path)
{
    QDir dir(path);

    if (dir.exists()) {
        if (!dir.removeRecursively()) {
            qWarning() << "Failed to remove directory:" << path;
            return false;
        }
    }

    return QDir().mkpath(path);
}

bool
FileUtils::extractArchive(
    const QString& archivePath,
    const QString& destDir,
    QString& firstLogFileFound)
{
    if (!cleanDirectory(destDir)) {
        qWarning() << "Failed to clean extract directory.";
        return false;
    }

    QStringList args;

    if (archivePath.endsWith(".tar.gz") || archivePath.endsWith(".tgz")) {
        args << "-xzf" << archivePath << "-C" << destDir;
    } else {
        args << "-xf" << archivePath << "-C" << destDir;
    }

    QProcess tar;

    tar.start("tar", args);
    if (!tar.waitForFinished() || tar.exitStatus() != QProcess::NormalExit) {
        qWarning() << "Tar extraction failed.";
        return false;
    }

    QDir dir(destDir);

    if (dir.exists("logmesh")) {
        firstLogFileFound = dir.absoluteFilePath("logmesh");
        return true;
    }

    if (dir.exists("logmesh.old")) {
        firstLogFileFound = dir.absoluteFilePath("logmesh.old");
        return true;
    }

    QStringList logFiles = dir.entryList(QStringList() << "*.log" << "*.txt", QDir::Files, QDir::Name);

    if (!logFiles.isEmpty()) {
        firstLogFileFound = dir.absoluteFilePath(logFiles.first());
        return true;
    }

    qWarning() << "No logmesh, logmesh.old or .log/.txt file found.";
    return false;
}
