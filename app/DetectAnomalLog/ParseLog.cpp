#include "ParseLog.h"

#include <QTextStream>
#include <QFile>
#include <QRegularExpressionMatch>
#include <QRegularExpressionMatchIterator>
#include <QDebug>

ParseLog::ParseLog(
    const QString& log_format,
    const QString& indir,
    const QString& outdir,
    int depth,
    double st,
    int maxChild,
    const QVector<QRegularExpression>& rex,
    bool keep_para)
    : m_path(indir), m_savePath(outdir), m_logFormat(log_format),
    m_depth(depth - 2), m_st(st), m_maxChild(maxChild), m_rex(rex), m_keepPara(keep_para) {}

bool
ParseLog::hasNumbers(
    const QString& s)
{
    for (auto ch : s) {
        if (ch.isDigit()) return true;
    }
    return false;
}

std::shared_ptr<LogCluster>
ParseLog::treeSearch(
    std::shared_ptr<Node> root,
    const QStringList& seq)
{
    int seqLen = seq.size();
    if (!root->childD.contains(QString::number(seqLen))) return nullptr;

    std::shared_ptr<Node> parent = root->childD[QString::number(seqLen)];
    int currentDepth = 1;

    for (const QString& token : seq) {
        if (currentDepth >= m_depth || currentDepth > seqLen) break;

        if (parent->childD.contains(token)) {
            parent = parent->childD[token];
        } else if (parent->childD.contains("<*>")) {
            parent = parent->childD["<*>"];
        } else {
            return nullptr;
        }
        ++currentDepth;
    }

    QVector<std::shared_ptr<LogCluster>>* clusters = reinterpret_cast<QVector<std::shared_ptr<LogCluster>>*>(parent->childD["__clusters"].get());
    return fastMatch(*clusters, seq);
}

void
ParseLog::addSeqToPrefixTree(
    std::shared_ptr<Node> root,
    std::shared_ptr<LogCluster> cluster)
{
    int seqLen = cluster->logTemplate.size();
    QString lenKey = QString::number(seqLen);
    std::shared_ptr<Node> parent;

    if (!root->childD.contains(lenKey)) {
        parent = std::make_shared<Node>(1, lenKey);
        root->childD[lenKey] = parent;
    } else {
        parent = root->childD[lenKey];
    }

    int currentDepth = 1;
    for (const QString& token : cluster->logTemplate) {
        if (currentDepth >= m_depth || currentDepth > seqLen) {
            if (!parent->childD.contains("__clusters")) {
                auto vec = new QVector<std::shared_ptr<LogCluster>>{cluster};
                parent->childD["__clusters"] = std::reinterpret_pointer_cast<Node>((std::shared_ptr<void>)vec);
            } else {
                auto vec = reinterpret_cast<QVector<std::shared_ptr<LogCluster>>*>(parent->childD["__clusters"].get());
                vec->append(cluster);
            }
            break;
        }

        QString key = hasNumbers(token) ? "<*>" : token;
        if (!parent->childD.contains(key)) {
            parent->childD[key] = std::make_shared<Node>(currentDepth + 1, key);
        }
        parent = parent->childD[key];
        ++currentDepth;
    }
}

double
ParseLog::seqDist(
    const QStringList& seq1,
    const QStringList& seq2,
    int& numOfPar)
{
    int simTokens = 0;
    numOfPar = 0;

    for (int i = 0; i < seq1.size(); ++i) {
        if (seq1[i] == "<*>") {
            ++numOfPar;
            continue;
        }
        if (seq1[i] == seq2[i]) ++simTokens;
    }
    return double(simTokens) / seq1.size();
}

std::shared_ptr<LogCluster>
ParseLog::fastMatch(
    const QVector<std::shared_ptr<LogCluster>>& clusters,
    const QStringList& seq)
{
    double maxSim = -1.0;
    int maxPar = -1;
    std::shared_ptr<LogCluster> bestMatch = nullptr;

    for (const auto& cluster : clusters) {
        int curPar = 0;
        double sim = seqDist(cluster->logTemplate, seq, curPar);
        if (sim > maxSim || (sim == maxSim && curPar > maxPar)) {
            maxSim = sim;
            maxPar = curPar;
            bestMatch = cluster;
        }
    }
    return (maxSim >= m_st) ? bestMatch : nullptr;
}

QStringList
ParseLog::getTemplate(
    const QStringList& seq1,
    const QStringList& seq2)
{
    QStringList result;
    for (int i = 0; i < seq1.size(); ++i) {
        result << ((seq1[i] == seq2[i]) ? seq1[i] : "<*>");
    }
    return result;
}

void
ParseLog::printTree(
    const std::shared_ptr<Node>& node,
    int dep)
{
    QString indent(dep, '\t');
    QString text = (node->depth == 0) ? "Root" : node->digitOrtoken;
    std::cout << indent.toStdString() << text.toStdString() << std::endl;

    if (node->depth == m_depth) return;
    for (const auto& child : node->childD.values()) {
        printTree(child, dep + 1);
    }
}

QString
ParseLog::preprocess(
    const QString& line)
{
    QString processed = line;
    for (const auto& re : m_rex) {
        processed = processed.replace(re, "<*>");
    }
    return processed;
}

void
ParseLog::generateLogFormatRegex()
{
    m_headers.clear();
    qWarning() << "log_format raw = " << m_logFormat;

    QString pattern = "^\\s*";

    QRegularExpression tagRe(R"(<[^<>]+>)");
    QRegularExpressionMatchIterator it = tagRe.globalMatch(m_logFormat);

    int lastPos = 0;

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        int start = match.capturedStart();
        int end = match.capturedEnd();

        // Static text
        QString staticText = m_logFormat.mid(lastPos, start - lastPos);
        QString esc = QRegularExpression::escape(staticText);
        pattern += esc;

        // Tag: <Field>
        QString tag = match.captured();
        QString header = tag.mid(1, tag.length() - 2);
        m_headers.append(header);
        pattern += R"(\s*(?P<)" + header + R"(>.*?)\s*)";
        lastPos = end;
    }

    QString tail = m_logFormat.mid(lastPos);
    QString esc = QRegularExpression::escape(tail);
    pattern += esc;

    pattern += "$";
    m_logRegex = QRegularExpression(pattern);
    qWarning() << "Generated pattern:" << m_logRegex.pattern();
}

void
ParseLog::loadData()
{
    generateLogFormatRegex();
    m_dfLog.clear();

    QFile file(m_path + "/" + m_logName);
    qWarning() << "Open log file:" << file.fileName();

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Cannot open log file:" << file.fileName();
        return;
    }

    QTextStream in(&file);
    int lineId = 1;
    int total = 0;
    int matched = 0;

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        total++;

        QRegularExpressionMatch match = m_logRegex.match(line);
        if (match.hasMatch()) {
            QMap<QString, QString> row;
            for (const QString& header : m_headers) {
                row[header] = match.captured(header);
            }
            row["LineId"] = QString::number(lineId++);
            m_dfLog.append(row);
            matched++;

            //qWarning() << "✅MATCH:" << line;
        } else {
            //qWarning() << "❌ NO MATCH:" << line;
        }
    }

    qWarning() << "✅ Parsed lines:" << matched << "/" << total;
    file.close();
}

QStringList
ParseLog::getParameterList(
    const QString& content,
    const QString& tmpl)
{
    QString regexStr = QRegularExpression::escape(tmpl).replace("\\<\\*\\>", "(.*?)");
    QRegularExpression re("^" + regexStr + "$", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatch match = re.match(content);
    if (!match.hasMatch()) return {};

    QStringList params;
    for (int i = 1; i < match.lastCapturedIndex() + 1; ++i) {
        params << match.captured(i);
    }
    return params;
}

void
ParseLog::outputResult(
    const QVector<std::shared_ptr<LogCluster>>& clusters)
{
    QFile file(m_savePath + "/" + m_logName + "_structured.csv");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream out(&file);
    out << "LineId,EventTemplate,EventId\n";

    QHash<QString, QString> templateToId;
    for (const auto& cluster : clusters) {
        QString tmplStr = cluster->logTemplate.join(" ");
        QByteArray hash = QCryptographicHash::hash(tmplStr.toUtf8(), QCryptographicHash::Md5);
        QString tmplId = hash.toHex().left(8);
        templateToId[tmplStr] = tmplId;

        for (int id : cluster->logIDL) {
            out << QString::number(id) << "," << tmplStr << "," << tmplId << "\n";
        }
    }
    file.close();
}

void
ParseLog::parse(
    const QString& name)
{
    m_logName = name;
    loadData();
    auto root = std::make_shared<Node>();
    QVector<std::shared_ptr<LogCluster>> clusters;
    qDebug() << "Loaded lines:" << m_dfLog.size() << "file name " << name;
    for (const auto& row : m_dfLog) {
        int id = row["LineId"].toInt();
        QStringList tokens = preprocess(row["Content"]).split(" ", Qt::SkipEmptyParts);
        auto matched = treeSearch(root, tokens);
        qDebug() << "id = " << id;
        if (!matched) {
            auto cluster = std::make_shared<LogCluster>(tokens, QVector<int>{id});
            clusters.append(cluster);
            addSeqToPrefixTree(root, cluster);
        } else {
            QStringList newTemplate = getTemplate(tokens, matched->logTemplate);
            matched->logIDL.append(id);
            if (newTemplate.join(" ") != matched->logTemplate.join(" ")) {
                matched->logTemplate = newTemplate;
            }
        }
    }
    qDebug() << "Loaded file name end";
    outputResult(clusters);
}
