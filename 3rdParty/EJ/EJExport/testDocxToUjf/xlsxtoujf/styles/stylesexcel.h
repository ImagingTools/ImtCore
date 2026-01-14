#ifndef STYLESEXCEL_H
#define STYLESEXCEL_H
#include <QMap>
#include "models/font.h"
#include "models/fill.h"
#include "models/borders.h"
#include "models/cellxfs.h"


class StylesExcel
{
    QList<Font> fonts;
    QList<Fill> fills;
    QList<Borders> borders;
    QList<CellXfs> cellXfs;
    QMap<QString, int> widths;
public:
    StylesExcel();
    int getWidthByString(const QString value);
    void addFont(Font);
    void addFill(Fill);
    void addBorder(Borders);
    void addCellXfs(CellXfs);

    Font getFontByIndex(int index);
    Fill getFillByIndex(int index);
    Borders getBorderByIndex(int index);
    CellXfs getCellXfsByIndex(int index);

};

#endif // STYLESEXCEL_H
