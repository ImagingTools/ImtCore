#ifndef EJCONVERTORPDF_H
#define EJCONVERTORPDF_H

#include <QString>
#include <QDomDocument>

#include "../convertor.h"
#include "../include/ejcommon.h"
#include "ejpdfdocument.h"
#include "ejpdfdocumenteditor.h"

#include <QPainter>
#include <QPrinter>

class EjConvertorPdf: public Convertor
{
	EjPdfDocument document;
    QMap<quint8, EjPdfDocumentWriter*> editors;
    bool write(EjDocument *doc,  QString file_name, QPrinter *printer = nullptr);
    void setSettings(EjDocument *doc, QPrinter *printer);
public:
    const static QString format;
	EjConvertorPdf();
	~EjConvertorPdf();
    bool convert(EjDocument *doc, QString const& file_name);
    bool print(EjDocument *doc, QPrinter *printer);
};

#endif // EJCONVERTORPDF_H
