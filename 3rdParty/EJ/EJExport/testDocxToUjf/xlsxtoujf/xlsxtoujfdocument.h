#ifndef XLSXTOUJFDOCUMENT_H
#define XLSXTOUJFDOCUMENT_H
#include "ejcommon.h"
#include "../ejbaseujfdocument.h"

class XlsxToUjfDocument: public EjBaseUjfDocument
{
public:
    XlsxToUjfDocument();
    const static QString alphabet;
    QString startCoord;
    QString endCoord;
    void calcCoord(const QString name, int &i, int &j);
    void removeWidthCells(int row, int col);
    void setCursorOnCell(int i, int j);
    void setFormula(const QString value);

};

#endif // XLSXTOUJFDOCUMENT_H
