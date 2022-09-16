#ifndef EXCELDOCUMENTEDITOR_H
#define EXCELDOCUMENTEDITOR_H

#include <QString>

#include "ejcommon.h"
#include "documents/ejsheetdocument.h"
#include "documents/ejsharedstringsdocument.h"
#include "documents/ejstylesdocument.h"
#include "imageblock.h"
#include "ejutils.h"

class EjExcelDocumentWriter {
public:
    virtual int edit(EjSheetDocument* sheet_document, EjSharedStringsDocument* shared_document,
					 EjStylesDocument* styles_document, QList<EjBlock*> *lBlocks, int index) = 0;
	virtual ~EjExcelDocumentWriter();
};

class StyleExcelDocumentWriter {
public:
    virtual int edit(EjSheetDocument* sheet_document, EjSharedStringsDocument* shared_document,
                      EjStylesDocument* styles_document, EjNumStyleBlock *block) = 0;
    virtual ~StyleExcelDocumentWriter();
};

class TextWriterExcel: public EjExcelDocumentWriter {
public:
    int edit(EjSheetDocument* sheet_document, EjSharedStringsDocument* shared_document,
			 EjStylesDocument* styles_document, QList<EjBlock*> *lBlocks, int index);
};


class SpaceWriterExcel: public EjExcelDocumentWriter {
public:
    int edit(EjSheetDocument* sheet_document, EjSharedStringsDocument* shared_document,
			 EjStylesDocument* styles_document, QList<EjBlock*> *lBlocks, int index);
};

class EnterWriterExcel: public EjExcelDocumentWriter {
public:
    int edit(EjSheetDocument* sheet_document, EjSharedStringsDocument* shared_document,
			 EjStylesDocument* styles_document, QList<EjBlock*> *lBlocks, int index);
};

class TableWriterExcel: public EjExcelDocumentWriter {
public:
    int edit(EjSheetDocument* sheet_document, EjSharedStringsDocument* shared_document,
			 EjStylesDocument* styles_document, QList<EjBlock*> *lBlocks, int index);
};

class EndTableWriterExcel: public EjExcelDocumentWriter {
public:
    int edit(EjSheetDocument* sheet_document, EjSharedStringsDocument* shared_document,
			 EjStylesDocument* styles_document, QList<EjBlock*> *lBlocks, int index);
};

class CellWriterExcel: public EjExcelDocumentWriter {
public:
    int edit(EjSheetDocument* sheet_document, EjSharedStringsDocument* shared_document,
			 EjStylesDocument* styles_document, QList<EjBlock*> *lBlocks, int index);
};


class StyleWriterExcel: public EjExcelDocumentWriter {
    QMap<quint8, StyleExcelDocumentWriter*> style_editors;
public:
    StyleWriterExcel();
    int edit(EjSheetDocument* sheet_document, EjSharedStringsDocument* shared_document,
			 EjStylesDocument* styles_document, QList<EjBlock*> *lBlocks, int index);
    ~StyleWriterExcel();

};

class StyleParagraphWriterExcel: public StyleExcelDocumentWriter {
public:
    int edit(EjSheetDocument* sheet_document, EjSharedStringsDocument* shared_document,
              EjStylesDocument* styles_document, EjNumStyleBlock *block);
};

class StyleTextWriterExcel: public StyleExcelDocumentWriter {
public:
    int edit(EjSheetDocument* sheet_document, EjSharedStringsDocument* shared_document,
              EjStylesDocument* styles_document, EjNumStyleBlock *block);
};


#endif // EXCELDOCUMENTEDITOR_H
