#ifndef LOGANALYZER_H
#define LOGANALYZER_H

#include <QObject>
#include "ParseLog.h"
#include "PCA.h"

class LogAnalyzer : public QObject {
    Q_OBJECT
public:
    explicit LogAnalyzer(QObject *parent = nullptr);

    Q_INVOKABLE void analyze(const QString &filepath, const QString &device);

signals:
    void analysisFinished(const QString &message);
    void analysisError(const QString &error);

private:
    PCA pca;
};

#endif // LOGANALYZER_H
