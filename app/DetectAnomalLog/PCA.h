#ifndef PCA_H
#define PCA_H

#include <QObject>
#include <QString>

class PCA : public QObject {
    Q_OBJECT
public:
    explicit PCA(QObject *parent = nullptr);

    Q_INVOKABLE void run(const QString &filePath, const QString &device);

signals:
    void detectionFinished(const QString &result);
    void detectionError(const QString &error);
};

#endif // PCA_H
