#include "PCA.h"

#include <Eigen/Dense>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QFileInfo>

using namespace Eigen;

PCA::PCA(QObject *parent) : QObject(parent) {}

void
PCA::run(
    const QString &filePath,
    const QString &device)
{
    QString path = filePath;

    if (path.startsWith("file://"))
        path = path.mid(7);

    QFileInfo info(path);

    if (!info.exists()) {
        emit detectionError("File not found.");
        return;
    }

    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit detectionError("Failed to open file.");
        return;
    }

    QVector<VectorXd> rows;
    int cols = -1;
    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();

        if (line.isEmpty()) {
            continue;
        }

        QStringList parts = line.split(",", Qt::SkipEmptyParts);

        if (cols == -1) {
            cols = parts.size();
        }

        VectorXd vec(cols);

        for (int i = 0; i < cols; ++i) {
            vec[i] = parts[i].toDouble();
        }

        rows.append(vec);
    }

    file.close();

    if (rows.isEmpty()) {
        emit detectionError("Empty dataset.");
        return;
    }

    MatrixXd data(rows.size(), cols);

    for (int i = 0; i < rows.size(); ++i) {
        data.row(i) = rows[i];
    }

    RowVectorXd mean = data.colwise().mean();
    MatrixXd centered = data.rowwise() - mean;
    MatrixXd cov = (centered.adjoint() * centered) / double(data.rows() - 1);

    SelfAdjointEigenSolver<MatrixXd> eig(cov);

    if (eig.info() != Success) {
        emit detectionError("Eigen decomposition failed.");
        return;
    }

    VectorXd eigenValues = eig.eigenvalues().reverse();
    MatrixXd eigenVectors = eig.eigenvectors().rowwise().reverse();

    QString result = "PCA Result (Top 3 eigenvalues):\n";

    for (int i = 0; i < std::min(3, int(eigenValues.size())); ++i) {
        result += QString("  %1. Eigenvalue: %2\n").arg(i + 1).arg(eigenValues[i]);
    }

    emit detectionFinished(result);
}
