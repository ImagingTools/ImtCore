#include "exportdoc.h"

#include <QDebug>

//#include "imageplug.h"
#include "xlsx/convertorexcel.h"
#include "docx/convertordocx.h"
#include "csv/convertorcsv.h"
#include "pdf/convertorpdf.h"
//#include "docxtoujf/convertorujf.h"
#include "testDocxToUjf/docxtoujfconvertor.h"
#include "testDocxToUjf/xlsxtoujfconvertor.h"


ExportDoc::ExportDoc()
{
}

bool ExportDoc::openFile(QString fName)
{
    return QDesktopServices::openUrl(QUrl("file:///" + fName));
}

bool ExportDoc::exportDocument(Document *doc, QString fileName, QString ext)
{
//    qDebug() << "Export document" << fileName << ext;
//    qDebug() << "Count Styles" << doc->lStyles->count();
//    qDebug() << "Count Pages" << doc->lPages->count();
//    qDebug() << "Count Strings" << doc->lStrings->count();
//    qDebug() << "Count blocks" << doc->lBlocks->count();

    bool result = false;

    if(fileName.contains("file://"))
    {
        QUrl urlPath(fileName);
        fileName = urlPath.toLocalFile();
    }
    if (ConvertorDocx::format == ext)
    {
        result = ConvertorDocx().convert(doc, fileName);
    } else if (ConvertorExcel::format == ext)
    {
        result = ConvertorExcel().convert(doc, fileName);
    } else if (ConvertorCSV::format == ext) {
         result = ConvertorCSV().convert(doc
                                         , fileName);
    } else if (ConvertorPdf::format == ext)
    {
        result = ConvertorPdf().convert(doc, fileName);
    } else
    {
         qDebug() << "unknown format" << ext;
    }
    if (result)
    {
        openFile(fileName);
    }
    qDebug() << "endConvert: " << result;
    return result;
}

bool ExportDoc::importToDocument(Document *doc, QString fileName, QString ext)
{
    qDebug() << "Import to document" << fileName << ext;
    qDebug() << "Real Import";
    bool result = false;
    if(fileName.contains("file://"))
    {
        QUrl urlPath(fileName);
        fileName = urlPath.toLocalFile();
    }

    if(ConvertorDocx::format == ext)
    {
        result = DocxToUjfConvertor().readDoc(doc, fileName);
    } else if(ConvertorExcel::format == ext)
    {
        result = XlsxToUjfConvertor().readDoc(doc, fileName);
    }

    return result;
}

bool ExportDoc::printDocument(Document *doc, QPrinter *printer)
{
    bool result = true;
    result &= ConvertorPdf().print(doc, printer);
    return result;
}
