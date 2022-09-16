#ifndef SHEETEXCELDOCUMENTREADER_H
#define SHEETEXCELDOCUMENTREADER_H
#include <QDomDocument>
#include "../xlsxtoujf/xlsxtoujfdocument.h"
#include "styles/stylesexcel.h"
#include "sharedStrings/sharedstrings.h"

class SheetExcelDocumentReader
{
public:
    virtual bool read(XlsxToUjfDocument& document, StylesExcel& styles, SharedStrings& strings, QDomNode node, QMap<QString, SheetExcelDocumentReader*> *editorsSheet) = 0;
    virtual ~SheetExcelDocumentReader();
};

class DimensionExcelDocumentReader: public SheetExcelDocumentReader
{
public:
    bool read(XlsxToUjfDocument& document, StylesExcel& styles, SharedStrings& strings, QDomNode node, QMap<QString, SheetExcelDocumentReader*> *editorsSheet);
};

class SheetDataExcelDocumentReader: public SheetExcelDocumentReader
{
public:
    bool read(XlsxToUjfDocument& document, StylesExcel& styles, SharedStrings& strings, QDomNode node, QMap<QString, SheetExcelDocumentReader*> *editorsSheet);
};

class RowExcelDocumentReader: public SheetExcelDocumentReader
{
public:
    bool read(XlsxToUjfDocument& document, StylesExcel& styles, SharedStrings& strings, QDomNode node, QMap<QString, SheetExcelDocumentReader*> *editorsSheet);
};

class CellExcelDocumentReader: public SheetExcelDocumentReader
{
public:
    bool read(XlsxToUjfDocument& document, StylesExcel& styles, SharedStrings& strings, QDomNode node, QMap<QString, SheetExcelDocumentReader*> *editorsSheet);
};

class VExcelDocumentReader: public SheetExcelDocumentReader
{
public:
    bool read(XlsxToUjfDocument& document, StylesExcel& styles, SharedStrings& strings, QDomNode node, QMap<QString, SheetExcelDocumentReader*> *editorsSheet);
};

class MergeCellsExcelDocumentReader: public SheetExcelDocumentReader
{
public:
    bool read(XlsxToUjfDocument& document, StylesExcel& styles, SharedStrings& strings, QDomNode node, QMap<QString, SheetExcelDocumentReader*> *editorsSheet);
};

#endif // SHEETEXCELDOCUMENTREADER_H
