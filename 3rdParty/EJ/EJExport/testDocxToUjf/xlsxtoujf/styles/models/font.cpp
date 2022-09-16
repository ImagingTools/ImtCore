#include "font.h"

Font::Font()
{
    color = QColor("black");
}

int Font::getSize()
{
    return font.pixelSize();
}

QColor Font::getColor()
{
    return color;;
}

bool Font::isBold()
{
    return font.bold();
}

bool Font::isItalic()
{
    return font.italic();
}

bool Font::isUnderline()
{
    return font.underline();
}

bool Font::isStrikeOut()
{
    return font.strikeOut();
}

void Font::setSize(int sz)
{
    font.setPixelSize(sz);
}

void Font::setColor(QColor color)
{
    this->color = color;
}

void Font::setBold()
{
    font.setBold(true);
}

void Font::setItalic()
{
    font.setItalic(true);
}

void Font::setUnderline()
{
    font.setUnderline(true);
}

void Font::setStrikeOut()
{
    font.setStrikeOut(true);
}
