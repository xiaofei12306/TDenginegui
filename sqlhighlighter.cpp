#include "sqlhighlighter.h"

SqlHighlighter::SqlHighlighter(QTextDocument *parent): QSyntaxHighlighter(parent){
    initializeRules();
}

void SqlHighlighter::highlightBlock(const QString &text)
{
    foreach (const HighlightingRule &rule, highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}

void SqlHighlighter::initializeRules(){
    // 设置格式
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);

    commentFormat.setForeground(Qt::darkGreen);

    doubleQuotedStringFormat.setForeground(Qt::darkRed);

    singleQuotedStringFormat.setForeground(Qt::darkRed);

    numericLiteralFormat.setForeground(Qt::darkMagenta);

    // SQL 关键字
    QStringList keywordPatterns = {
        "\\bADD\\b",
        "\\bALL\\b",
        "\\bALTER\\b",
        "\\bAND\\b",
        "\\bANY\\b",
        "\\bAS\\b",
        "\\bASC\\b",
        "\\bBY\\b",
        "\\bBETWEEN\\b",
        "\\bCASE\\b",
        "\\bCAST\\b",
        "\\bCHECK\\b",
        "\\bCOLUMN\\b",
        "\\bCONSTRAINT\\b",
        "\\bCREATE\\b",
        "\\bCROSS\\b",
        "\\bDATABASE\\b",
        "\\bDATABASES\\b",
        "\\bDEFAULT\\b",
        "\\bDELETE\\b",
        "\\bDESC\\b",
        "\\bDISTINCT\\b",
        "\\bDROP\\b",
        "\\bELSE\\b",
        "\\bEND\\b",
        "\\bESCAPE\\b",
        "\\bEXCEPT\\b",
        "\\bEXISTS\\b",
        "\\bEXPLAIN\\b",
        "\\bFALSE\\b",
        "\\bFETCH\\b",
        "\\bFOR\\b",
        "\\bFOREIGN\\b",
        "\\bFROM\\b",
        "\\bFULL\\b",
        "\\bGRANT\\b",
        "\\bGROUP\\b",
        "\\bHAVING\\b",
        "\\bIN\\b",
        "\\bINDEX\\b",
        "\\bINNER\\b",
        "\\bINSERT\\b",
        "\\bINTERSECT\\b",
        "\\bINTO\\b",
        "\\bIS\\b",
        "\\bJOIN\\b",
        "\\bKEY\\b",
        "\\bLEFT\\b",
        "\\bLIKE\\b",
        "\\bLIMIT\\b",
        "\\bNATURAL\\b",
        "\\bNOT\\b",
        "\\bNULL\\b",
        "\\bOFFSET\\b",
        "\\bON\\b",
        "\\bOR\\b",
        "\\bORDER\\b",
        "\\bOUTER\\b",
        "\\bPRIMARY\\b",
        "\\bREFERENCES\\b",
        "\\bRIGHT\\b",
        "\\bSELECT\\b",
        "\\bSET\\b",
        "\\bTABLE\\b",
        "\\bTHEN\\b",
        "\\bTO\\b",
        "\\bTRUE\\b",
        "\\bUNION\\b",
        "\\bUNIQUE\\b",
        "\\bUPDATE\\b",
        "\\bUSING\\b",
        "\\bVALUES\\b",
        "\\bWHEN\\b",
        "\\bWHERE\\b",
        "\\bWITH\\b",
        "\\bSHOW\\b",
        "\\bDESCRIBE\\b",
    };

    // 注释
    highlightingRules.append(HighlightingRule(QRegularExpression("--[^\n]*"), commentFormat));
    highlightingRules.append(HighlightingRule(QRegularExpression("/\\*(?:.|\\n|\\r)*\\*/"), commentFormat));

    // 字符串
    highlightingRules.append(HighlightingRule(QRegularExpression("\".*\""), doubleQuotedStringFormat));
    highlightingRules.append(HighlightingRule(QRegularExpression("'.*'"), singleQuotedStringFormat));

    // 数字
    highlightingRules.append(HighlightingRule(QRegularExpression("\\b[0-9]+\\b"), numericLiteralFormat));

    // 添加关键字格式
    foreach (const QString &pattern, keywordPatterns) {
        highlightingRules.append(HighlightingRule(QRegularExpression(pattern, QRegularExpression::CaseInsensitiveOption), keywordFormat));
    }
}
