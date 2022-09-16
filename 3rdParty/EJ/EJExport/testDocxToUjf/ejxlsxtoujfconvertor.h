#ifndef EJXLSXTOUJFCONVERTOR_H
#define EJXLSXTOUJFCONVERTOR_H
#include <QString>
#include<QStandardPaths>
#include "xlsxtoujf/styles/stylesexceldocumentreaders.h"
#include "xlsxtoujf/sharedStrings/stringsexceldocumentreaders.h"
#include "xlsxtoujf/sheetexceldocumentreaders.h"
#include "xlsxtoujf/xlsxtoujfdocument.h"
#include "ejcommon.h"
#include "ejbaseujfconvertor.h"
class EjXlsxToUjfConvertor: public EjBaseUjfConvertor
{
    Zipper zipper;
    QString temp_folder;
    QString subfolder;
    QMap<QString, StylesExcelDocumentReader*> editors;
    QMap<QString, StringsExcelDocumentReader*> editorsStrings;
    QMap<QString, SheetExcelDocumentReader*> editorsSheet;
    StylesExcel stylesDocument;
    SharedStrings sharedStrings;
    XlsxToUjfDocument document;
    bool readStylesDocument(QString fileName);
    bool readStringsDocument(QString fileName);
    bool readSheetDocument(EjDocument *doc, QString fileName);
public:
	EjXlsxToUjfConvertor();
    bool readDoc(EjDocument *doc, QString fileName);

};

#endif // EJXLSXTOUJFCONVERTOR_H
