#ifndef FONT_H
#define FONT_H
#include <QFont>
#include <QColor>

class Font
{
    QFont font;
    QColor color;
public:
    Font();
    int getSize();
    QColor getColor();
    bool isBold();
    bool isItalic();
    bool isUnderline();
    bool isStrikeOut();

    void setSize(int sz);
    void setColor(QColor color);
    void setBold();
    void setItalic();
    void setUnderline();
    void setStrikeOut();
};

#endif // FONT_H
