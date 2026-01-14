#ifndef UJFDOCUMENTEDITOR_H
#define UJFDOCUMENTEDITOR_H

#include <QDomDocument>
#include "ejdocxtoujfdocument.h"
#include "imageblock.h"


class WordDocumentReader {
public:
    virtual bool edit(EjDocxToUjfDocument& document, QDomNode node,  QMap<QString, WordDocumentReader*> *editors) = 0;
    virtual ~WordDocumentReader();
};

class ParagraphDocumentReader: public WordDocumentReader{
public:
    bool edit(EjDocxToUjfDocument& document, QDomNode node, QMap<QString, WordDocumentReader*> *editors);
};

class pPrDocumentReader: public ParagraphDocumentReader {
public:
     bool edit(EjDocxToUjfDocument& document, QDomNode node, QMap<QString, WordDocumentReader*> *editors = nullptr);
};

class TableDocumentReader: public WordDocumentReader{
public:
    bool edit(EjDocxToUjfDocument& document, QDomNode node, QMap<QString, WordDocumentReader*> *editors);
};

class tblPrDocumentReader: public TableDocumentReader {
public:
     bool edit(EjDocxToUjfDocument& document, QDomNode node, QMap<QString, WordDocumentReader*> *editors = nullptr);
};

class tblGridDocumentReader: public TableDocumentReader {
public:
     bool edit(EjDocxToUjfDocument& document, QDomNode node, QMap<QString, WordDocumentReader*> *editors = nullptr);
};

class TableRowDocumentReader: public TableDocumentReader{
public:
    bool edit(EjDocxToUjfDocument& document, QDomNode node, QMap<QString, WordDocumentReader*> *editors);
};

class trPrDocumentReader: public TableRowDocumentReader {
public:
     bool edit(EjDocxToUjfDocument& document, QDomNode node, QMap<QString, WordDocumentReader*> *editors = nullptr);
};

class TableCellDocumentReader: public TableDocumentReader{
public:
    bool edit(EjDocxToUjfDocument& document, QDomNode node, QMap<QString, WordDocumentReader*> *editors);
};

class tcPrDocumentReader: public TableCellDocumentReader {
public:
     bool edit(EjDocxToUjfDocument& document, QDomNode node, QMap<QString, WordDocumentReader*> *editors = nullptr);
};

class RDocumentReader: public WordDocumentReader{
public:
    bool edit(EjDocxToUjfDocument& document, QDomNode node, QMap<QString, WordDocumentReader*> *editors);
};

class rPrDocumentReader: public RDocumentReader{
public:
    bool edit(EjDocxToUjfDocument& document, QDomNode node, QMap<QString, WordDocumentReader*> *editors);
};

class TextDocumentReader: public RDocumentReader {
public:
     bool edit(EjDocxToUjfDocument& document, QDomNode node, QMap<QString, WordDocumentReader*> *editors = nullptr);
};

class TabDocumentReader: public RDocumentReader {
public:
     bool edit(EjDocxToUjfDocument& document, QDomNode node, QMap<QString, WordDocumentReader*> *editors = nullptr);
};


#endif // UJFDOCUMENTEDITOR_H
