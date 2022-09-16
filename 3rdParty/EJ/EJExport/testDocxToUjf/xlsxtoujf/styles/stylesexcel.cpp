#include "stylesexcel.h"

StylesExcel::StylesExcel()
{
    widths.insert("thin", 100);
    widths.insert("thick", 300);
    widths.insert("middle", 200);
}

int StylesExcel::getWidthByString(const QString value)
{
    return widths.value(value);
}

void StylesExcel::addFont(Font font)
{
    fonts.append(font);
}

void StylesExcel::addFill(Fill fill)
{
    fills.append(fill);
}

void StylesExcel::addBorder(Borders border)
{
    borders.append(border);
}

void StylesExcel::addCellXfs(CellXfs cx)
{
    cellXfs.append(cx);
}

Font StylesExcel::getFontByIndex(int index)
{
    return fonts.at(index);
}

Fill StylesExcel::getFillByIndex(int index)
{
    return fills.at(index);
}

Borders StylesExcel::getBorderByIndex(int index)
{
    return borders.at(index);
}

CellXfs StylesExcel::getCellXfsByIndex(int index)
{
    return cellXfs.at(index);
}
