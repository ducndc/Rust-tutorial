#include "LogAnalyzer.h"
#include "FileUtils.h"
#include "PCA.h"

#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include <QProcess>

LogAnalyzer::LogAnalyzer(
    QObject *parent) : QObject(parent)
{
    connect(&pca, &PCA::detectionFinished, this, &LogAnalyzer::analysisFinished);
    connect(&pca, &PCA::detectionError, this, &LogAnalyzer::analysisError);
}

void
LogAnalyzer::analyze(
    const QString &filepath,
    const QString &device)
{
    QString path = FileUtils::removeFilePrefix(filepath);
    QFileInfo info(path);

    if (!info.exists()) {
        emit analysisError("Selected file not found.");
        return;
    }

    QString finalLogPath;
    QString extractDir = "./tmp/extracted_log";
    QString outDir = "./tmp/analyzed_result";

    if (path.endsWith(".tar") || path.endsWith(".tar.gz") || path.endsWith(".tgz")) {
        QString extractedLog;
        if (!FileUtils::extractArchive(path, extractDir, extractedLog)) {
            emit analysisError("Failed to extract or find .log file.");
            return;
        }
        finalLogPath = extractedLog;
    } else {
        finalLogPath = path;
    }

    if (!FileUtils::cleanDirectory(outDir)) {
        emit analysisError("Failed to prepare output directory.");
        return;
    }

    QFileInfo logInfo(finalLogPath);
    QString baseName = logInfo.baseName();

    // 1. Parse Log
    QString log_format = "<Day> <Month> <Date> <Time> <Year> <Level> <Component>: <Line> <Content>";

    QVector<QRegularExpression> regexList = {
                                             QRegularExpression("blk_(|-)[0-9]+"),
                                             QRegularExpression("(/|)([0-9]+\\.){3}[0-9]+(:[0-9]+|)(:|)"),
                                             QRegularExpression("(?<=[^A-Za-z0-9])[-+]?[0-9]+(?=[^A-Za-z0-9])|[0-9]+$"),
                                             QRegularExpression("\\[[0-9]+\\]"),
                                             QRegularExpression("\\[[A-Z]+\\]"),
                                             QRegularExpression("\\[\\d+\\]"),
                                             QRegularExpression("\\[[A-Za-z0-9]+\\]"),
                                             QRegularExpression("(?<= )\\d+(?= )"),
                                             QRegularExpression("\\[[^\\]]+\\]"),
                                             QRegularExpression("(?<=[^\\w])\\d+(?=[^\\w])"),
                                             QRegularExpression("(\\d+\\.){3}\\d+"),
                                             };

    ParseLog parser(log_format, logInfo.absolutePath(), outDir, 6, 0.5, 100, regexList);
    parser.parse(logInfo.fileName());

    QString csvOut = outDir + "/" + baseName + "_structured.csv";

    //2. PCA
    pca.run(csvOut, device);
}
