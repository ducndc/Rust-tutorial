#ifndef PARSELOG_H
#define PARSELOG_H

#include <QString>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QRegularExpression>
#include <QVector>
#include <QMap>
#include <QHash>
#include <QDateTime>
#include <QCryptographicHash>
#include <iostream>
#include <memory>

struct LogCluster {
    QStringList logTemplate;
    QVector<int> logIDL;

    LogCluster(const QStringList& templateStr = {}, const QVector<int>& ids = {})
        : logTemplate(templateStr), logIDL(ids) {}
};

struct Node {
    QMap<QString, std::shared_ptr<Node>> childD;
    int depth;
    QString digitOrtoken;
    QVector<std::shared_ptr<LogCluster>> clusters;
    Node(int d = 0, const QString& token = "") : depth(d), digitOrtoken(token) {}
};

class ParseLog {
public:
    ParseLog(
        const QString& log_format,
        const QString& indir = "./",
        const QString& outdir = "./result/",
        int depth = 4,
        double st = 0.4,
        int maxChild = 100,
        const QVector<QRegularExpression>& m_rex = {},
        bool keep_para = true
        );

    void parse(const QString& m_logName);

private:
    QString m_path;
    QString m_logFormat;
    QString m_savePath;
    QString m_logName;
    QVector<QRegularExpression> m_rex;
    bool m_keepPara;
    int m_depth;
    double m_st;
    int m_maxChild;

    QVector<QMap<QString, QString>> m_dfLog;  // simple log DataFrame representation
    QVector<QString> m_headers;
    QRegularExpression m_logRegex;

    // Core logic
    bool hasNumbers(const QString& s);
    std::shared_ptr<LogCluster> treeSearch(std::shared_ptr<Node> root, const QStringList& seq);
    void addSeqToPrefixTree(std::shared_ptr<Node> root, std::shared_ptr<LogCluster> cluster);
    double seqDist(const QStringList& seq1, const QStringList& seq2, int& numOfPar);
    std::shared_ptr<LogCluster> fastMatch(const QVector<std::shared_ptr<LogCluster>>& clusters, const QStringList& seq);
    QStringList getTemplate(const QStringList& seq1, const QStringList& seq2);
    void printTree(const std::shared_ptr<Node>& node, int dep);
    void loadData(void);
    QString preprocess(const QString& line);
    void generateLogFormatRegex(void);
    QStringList getParameterList(const QString& content, const QString& templateStr);

    // Output
    void outputResult(const QVector<std::shared_ptr<LogCluster>>& clusters);
};
#endif // PARSELOG_H
