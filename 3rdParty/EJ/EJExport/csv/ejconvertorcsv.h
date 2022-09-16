#ifndef EJCONVERTORCSV_H
#define EJCONVERTORCSV_H

#include <QString>
#include <QDomDocument>

#include "../convertor.h"
#include "../include/ejcommon.h"
#include "ejcsvdocument.h"
#include "ejcsvdocumenteditor.h"

class EjConvertorCSV: public Convertor
{
	EjCSVDocument document;
	QMap<quint8, EjCSVDocumentWriter*> editors;
    bool write(EjDocument *doc);
public:
	EjConvertorCSV();
    const static QString format;    
    bool convert(EjDocument *doc, QString const& file_name);
	virtual ~EjConvertorCSV();
};

#endif // EJCONVERTORCSV_H
