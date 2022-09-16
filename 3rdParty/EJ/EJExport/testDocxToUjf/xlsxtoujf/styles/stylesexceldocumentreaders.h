#ifndef STYLESEXCELDOCUMENTREADER_H
#define STYLESEXCELDOCUMENTREADER_H
#include <QDomDocument>
#include "stylesexcel.h"

class StylesExcelDocumentReader
{
public:
    virtual bool read(StylesExcel& document, QDomNode node) = 0;
    virtual ~StylesExcelDocumentReader();
};

class FontsStyleDocumentReader: public StylesExcelDocumentReader
{
public:
     bool read(StylesExcel& document, QDomNode node);
};

class FillsStyleDocumentReader: public StylesExcelDocumentReader
{
public:
     bool read(StylesExcel& document, QDomNode node);
};

class BordersStyleDocumentReader: public StylesExcelDocumentReader
{
public:
     bool read(StylesExcel& document, QDomNode node);
};

class CellXfsStyleDocumentReader: public StylesExcelDocumentReader
{
public:
     bool read(StylesExcel& document, QDomNode node);
};

#endif // STYLESEXCELDOCUMENTREADER_H
