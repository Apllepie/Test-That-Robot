#ifndef LUASYNTAXHIGHLIGHTER_H
#define LUASYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>

class LuaSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    LuaSyntaxHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;
    QTextCharFormat multiLineCommentFormat;
};

#endif // LUASYNTAXHIGHLIGHTER_H
