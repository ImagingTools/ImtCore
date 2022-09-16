#ifndef WORD_DOCUMENT_EDITORS_H
#define WORD_DOCUMENT_EDITORS_H

#include <QString>

#include "ejworddocument.h"
#include "imageblock.h"

class EjWordDocumentWriter {
public:
	virtual bool edit(EjWordDocument& document, EjBlock* block) = 0;
	virtual ~EjWordDocumentWriter();
};

class StyleDocumentWriter {
public:
    virtual bool edit(EjWordDocument& document, EjNumStyleBlock *block) = 0;
    virtual ~StyleDocumentWriter();
};

class TextWriterWord: public EjWordDocumentWriter {
public:
	bool edit(EjWordDocument& document, EjBlock* block);
};

class SpaceWriterWord: public EjWordDocumentWriter {
public:
	bool edit(EjWordDocument& document, EjBlock*);
};

class EnterWriterWord: public EjWordDocumentWriter {
public:
	bool edit(EjWordDocument& document, EjBlock*);
};

class TableWriterWord: public EjWordDocumentWriter {
public:
	bool edit(EjWordDocument& document, EjBlock* block);
};

class CellWriterWord: public EjWordDocumentWriter {
public:
	bool edit(EjWordDocument& document, EjBlock* block);
};

class ImageWriterWord: public EjWordDocumentWriter {
public:
	bool edit(EjWordDocument& document, EjBlock* block);
};

class ByteArrayWriterWord: public EjWordDocumentWriter {
public:
	bool edit(EjWordDocument& document, EjBlock* block);
};

class EndGroupWriterWord: public EjWordDocumentWriter {
public:
	bool edit(EjWordDocument& document, EjBlock*);
};

class StyleWriterWord: public EjWordDocumentWriter {
    QMap<quint8, StyleDocumentWriter*> style_editors;
public:
    StyleWriterWord();
	bool edit(EjWordDocument& document, EjBlock* block);
    ~StyleWriterWord();
};

class StyleParagraphWriterWord: public StyleDocumentWriter {
public:
    bool edit(EjWordDocument& document, EjNumStyleBlock *block);
};

class StyleTextWriterWord: public StyleDocumentWriter {
public:
    bool edit(EjWordDocument& document, EjNumStyleBlock *block);
};

#endif // WORD_DOCUMENT_EDITORS_H
