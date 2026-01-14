#ifndef CONVERTOR_DOCX_H
#define CONVERTOR_DOCX_H

#include <QString>
#include <QDomDocument>
#include<QStandardPaths>
#include "../convertor.h"
#include "../include/ejcommon.h"
#include <zipper.h>
#include "ejworddocument.h"
#include "ejworddocumenteditors.h"

class EjConvertorDocx: public Convertor {
    Zipper zipper;
    QString temp_folder;
    QString subfolder;
	QMap<quint8, EjWordDocumentWriter*> editors;
	EjWordDocument document;
    bool unzip() const;
    bool zip(QString const& file_name) const;
    bool readDoc(EjDocument *doc);
    bool write();
    void clear();
public:
    const static QString format;
	EjConvertorDocx();
    bool convert(EjDocument *doc, QString const& file_name);
	virtual ~EjConvertorDocx();
};

#endif // CONVERTOR_DOCX_H
