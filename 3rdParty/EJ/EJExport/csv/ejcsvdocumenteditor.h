#ifndef CSVDOCUMENTEDITOR_H
#define CSVDOCUMENTEDITOR_H

#include <QString>
#include "ejcsvdocument.h"

class EjCSVDocumentWriter
{
public:
	virtual ~EjCSVDocumentWriter();
	virtual int edit(EjCSVDocument& document, QList<EjBlock*> blocks, int index = 0) = 0;
};

class TextWriterCSV: public EjCSVDocumentWriter {
public:
	int edit(EjCSVDocument& document, QList<EjBlock*> blocks, int index);
};

class SpaceWriterCSV: public EjCSVDocumentWriter{
public:
	int edit(EjCSVDocument& document, QList<EjBlock*> blocks, int index);
};

class EnterWriterCSV: public EjCSVDocumentWriter {
public:
	int edit(EjCSVDocument& document, QList<EjBlock*> blocks, int index);
};

class TableWriterCSV: public EjCSVDocumentWriter {
public:
	int edit(EjCSVDocument& document, QList<EjBlock*> blocks, int index);
};

class CellWriterCSV: public EjCSVDocumentWriter {
public:
	int edit(EjCSVDocument& document, QList<EjBlock*> blocks, int index);
};

class EndGroupWriterCSV: public EjCSVDocumentWriter {
public:
	int edit(EjCSVDocument& document, QList<EjBlock*> blocks, int index);
};

class MaxIndexColumnCSV: public EjCSVDocumentWriter {
public:
	int edit(EjCSVDocument& document, QList<EjBlock*> blocks, int index);
};

class EndEditorCSV: public EjCSVDocumentWriter {
public:
	int edit(EjCSVDocument& document, QList<EjBlock*> blocks, int index);
};
#endif // CSVDOCUMENTEDITOR_H
