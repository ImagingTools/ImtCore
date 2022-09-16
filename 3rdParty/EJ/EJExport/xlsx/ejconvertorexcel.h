#ifndef CONVERTOR_EXCEL_H
#define CONVERTOR_EXCEL_H

#include <QString>
#include <QDomDocument>
#include<QStandardPaths>
#include "../EJExport/convertor.h"
#include "ejcommon.h"
#include "../EJExport/mini_zipper/zipper.h"
#include "ejexceldocumenteditors.h"
#include "documents/ejsheetdocument.h"
#include "documents/ejsharedstringsdocument.h"
#include "documents/ejstylesdocument.h"

class EjConvertorExcel: public Convertor {
    Zipper zipper;
    QString temp_folder;
    QString subfolder;
	QMap<quint8, EjExcelDocumentWriter*> editors;
	EjSheetDocument sheet_document;
	EjSharedStringsDocument sharedStrings_document;
	EjStylesDocument styles_document;


    bool unzip() const;
    bool zip(QString const& file_name) const;
    bool readDoc(EjDocument *doc);
    bool write();
    void clear();

public:
    const static QString format;
	EjConvertorExcel();
    bool convert(EjDocument *doc, const QString &fileName);
	EjSheetDocument getSheetDocument();
	EjSharedStringsDocument getWorksheetsDocument();
	virtual ~EjConvertorExcel();
};

#endif // CONVERTOR_EXCEL_H
