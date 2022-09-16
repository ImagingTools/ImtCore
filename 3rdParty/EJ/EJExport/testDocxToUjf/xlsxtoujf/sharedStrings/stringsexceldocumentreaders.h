#ifndef STRINGSEXCELDOCUMENTREADER_H
#define STRINGSEXCELDOCUMENTREADER_H
#include "sharedstrings.h"
#include <QDomDocument>
#include <QMap>

class StringsExcelDocumentReader
{
public:
    virtual bool read(SharedStrings& document, QDomNode node, QMap<QString, StringsExcelDocumentReader*> *editorsStrings) = 0;
    virtual ~StringsExcelDocumentReader();
};

class TagSiExcelDocumentReader: public StringsExcelDocumentReader
{
public:
    bool read(SharedStrings& document, QDomNode node, QMap<QString, StringsExcelDocumentReader*> *editorsStrings);
};


class TagRExcelDocumentReader: public StringsExcelDocumentReader
{
public:
    bool read(SharedStrings& document, QDomNode node, QMap<QString, StringsExcelDocumentReader*> *editorsStrings);
};

class TagRprExcelDocumentReader: public StringsExcelDocumentReader
{
public:
    bool read(SharedStrings& document, QDomNode node, QMap<QString, StringsExcelDocumentReader*> *editorsStrings);
};

class TagTExcelDocumentReader: public StringsExcelDocumentReader
{
public:
    bool read(SharedStrings& document, QDomNode node, QMap<QString, StringsExcelDocumentReader*> *editorsStrings);
};



#endif // STRINGSEXCELDOCUMENTREADER_H
