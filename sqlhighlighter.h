#ifndef SQLHIGHLIGHTER_H
#define SQLHIGHLIGHTER_H

#include <QRegularExpression>
#include <QSyntaxHighlighter>


class SqlHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    SqlHighlighter(QTextDocument *parent = nullptr);
    SqlHighlighter(QObject *parent);
private:
    class HighlightingRule {
    public:
        QRegularExpression pattern;
        QTextCharFormat format;
        HighlightingRule(QRegularExpression pattern, QTextCharFormat format): pattern(pattern), format(format){};
    };
    QVector<HighlightingRule> highlightingRules;

    QTextCharFormat keywordFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat doubleQuotedStringFormat;
    QTextCharFormat singleQuotedStringFormat;
    QTextCharFormat numericLiteralFormat;
    void highlightBlock(const QString &text) override;
    void initializeRules();
};

#endif // SQLHIGHLIGHTER_H
