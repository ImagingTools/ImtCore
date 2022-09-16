#include "cellxfs.h"

CellXfs::CellXfs()
{

}

CellXfs::CellXfs(Font font, Fill fill, Borders borders)
{
    this->font = font;
    this->fill = fill;
    this->border = borders;
}

void CellXfs::setFont(Font font)
{
    this->font = font;
}

void CellXfs::setFill(Fill fill)
{
    this->fill = fill;
}

void CellXfs::setBorder(Borders border)
{
    this->border = border;
}

QString CellXfs::getHAlign() const
{
    return horizontal;
}

QString CellXfs::getVAlign() const
{
    return vertical;
}

void CellXfs::setHAlign(QString align)
{
    this->horizontal = align;
}

void CellXfs::setVAlign(QString align)
{
    this->vertical = align;
}

Font CellXfs::getFont()
{
    return font;
}

Fill CellXfs::getFill()
{
    return fill;
}

Borders CellXfs::getBorders()
{
    return border;
}
