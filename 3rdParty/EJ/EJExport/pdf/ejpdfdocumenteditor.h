#ifndef PDFDOCUMENTEDITOR_H
#define PDFDOCUMENTEDITOR_H
#include "ejpdfdocument.h"
#include "imageblock.h"


class COMMONSHARED_EXPORT EjPdfDocumentWriter
{
public:
	virtual void edit(EjPdfDocument& document, EjBlock* block, QPainter* painter = 0) = 0;
	~EjPdfDocumentWriter();
};

class TextWriterPdf: public EjPdfDocumentWriter {
public:
	void edit(EjPdfDocument& document, EjBlock* block, QPainter* painter);
};

class SpaceWriterPdf: public EjPdfDocumentWriter {
public:
	void edit(EjPdfDocument& document, EjBlock* block, QPainter* painter);
};

class CellWriterPdf: public EjPdfDocumentWriter {
public:
	void edit(EjPdfDocument& document, EjBlock* block, QPainter* painter);
};

class ImageWriterPdf: public EjPdfDocumentWriter {
public:
	void edit(EjPdfDocument& document, EjBlock* block, QPainter* painter);
};

class ByteArrayWriterPdf: public EjPdfDocumentWriter {
public:
	void edit(EjPdfDocument& document, EjBlock* block, QPainter* painter);
};

class EndGroupWriterPdf: public EjPdfDocumentWriter {
public:
	void edit(EjPdfDocument& document, EjBlock*, QPainter* painter);
};

class StyleWriterPdf: public EjPdfDocumentWriter {
public:
	void edit(EjPdfDocument& document, EjBlock* block, QPainter* painter);
};

#endif // PDFDOCUMENTEDITOR_H
