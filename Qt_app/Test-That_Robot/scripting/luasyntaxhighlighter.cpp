#include "luasyntaxhighlighter.h"
#include <QDebug> // Для отладки

LuaSyntaxHighlighter::LuaSyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    // --- Правила форматирования ---

    // 1. Ключевые слова и литералы (true, false, nil)
    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor(86, 156, 214)); // Синий
    keywordFormat.setFontWeight(QFont::Bold);
    const QString keywordPatterns[] = {
        "\\bfunction\\b", "\\bend\\b", "\\blocal\\b", "\\bif\\b", "\\bthen\\b",
        "\\belse\\b", "\\belseif\\b", "\\bwhile\\b", "\\bdo\\b", "\\breturn\\b",
        "\\bfor\\b", "\\bin\\b", "\\band\\b", "\\bor\\b", "\\bnot\\b",
        "\\bnil\\b", "\\btrue\\b", "\\bfalse\\b"
    };
    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // 2. Строки в одинарных и двойных кавычках (нежадные)
    QTextCharFormat quotationFormat;
    quotationFormat.setForeground(QColor(206, 145, 120)); // Коричневый
    rule.pattern = QRegularExpression("\".*?\""); // Для "строк"
    rule.format = quotationFormat;
    highlightingRules.append(rule);
    rule.pattern = QRegularExpression("'.*?'"); // Для 'строк'
    highlightingRules.append(rule);

    // 3. Числа
    QTextCharFormat numberFormat;
    numberFormat.setForeground(QColor(181, 206, 168)); // Светло-зеленый
    rule.pattern = QRegularExpression("\\b[0-9]+(?:\\.[0-9]*)?\\b");
    rule.format = numberFormat;
    highlightingRules.append(rule);

    // 4. Названия функций (слово, за которым идет открывающая скобка)
    QTextCharFormat functionFormat;
    functionFormat.setForeground(QColor(120, 120, 170)); // Желтый
    rule.pattern = QRegularExpression("\\b[A-Za-z_][A-Za-z0-9_]*(?=\\s*\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    // 5. Однострочные комментарии. Важно, чтобы это правило шло после строк!
    QTextCharFormat singleLineCommentFormat;
    singleLineCommentFormat.setForeground(QColor(106, 153, 85)); // Темно-зеленый
    singleLineCommentFormat.setFontItalic(true);
    rule.pattern = QRegularExpression("--[^\\[].*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    // 6. Многострочные комментарии (обрабатываются отдельно)
    multiLineCommentFormat.setForeground(QColor(106, 153, 85)); // Темно-зеленый
    multiLineCommentFormat.setFontItalic(true);
    commentStartExpression = QRegularExpression("--\\[\\[");
    commentEndExpression = QRegularExpression("\\]\\]");
}

void LuaSyntaxHighlighter::highlightBlock(const QString &text)
{
    // Сначала применяем все простые правила из нашего списка
    for (const HighlightingRule &rule : highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    // Теперь отдельно обрабатываем многострочные комментарии,
    // так как они могут "перетекать" из одного блока в другой.
    // Эта логика будет перекрашивать текст, даже если он уже был покрашен по другим правилам.
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1) {
        startIndex = text.indexOf(commentStartExpression);
    }

    while (startIndex >= 0) {
        QRegularExpressionMatch endMatch = commentEndExpression.match(text, startIndex);
        int endIndex = endMatch.capturedStart();
        int commentLength;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + endMatch.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
