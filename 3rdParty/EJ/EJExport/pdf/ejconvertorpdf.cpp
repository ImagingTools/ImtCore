#include "ejconvertorpdf.h"

#include "imageblock.h"
#include "ejtableblocks.h"
#include "export_global.h"
#include "ejstoragehelper.h"

#include <QtCore>

const QString EjConvertorPdf::format = "pdf";

/*НОВАЯ ТЕСТОВАЯ ИНФОРМАЦИЯ
FFFFFFFFFFFFFFFFFFFFFF*/

bool EjConvertorPdf::convert(EjDocument *doc, const QString &file_name)
{
    bool result = write(doc, file_name);
    return result;
}

bool EjConvertorPdf::print(EjDocument *doc, QPrinter *printer)
{
    bool result = true;
    result &= write(doc, "", printer);
    return result;
}

bool EjConvertorPdf::write(EjDocument *doc, QString file_name, QPrinter *print)
{
    QPrinter *printer = new QPrinter();
    if (print != nullptr)
    {
       printer = print;
       printer->setOutputFormat(QPrinter::NativeFormat);
#if QT_VERSION < 0x060000
       printer->setPageMargins (0, 15, 0, 0, QPrinter::Unit::Millimeter);
#else
	   printer->setPageMargins (QMarginsF(0, 15, 0, 0));
#endif
    }else
    {
        printer->setOutputFileName(file_name);
        setSettings(doc, printer);
    }

    QPainter painter(printer);
    painter.begin(printer);

    bool result = true;
	const QList<EjBlock*> &list = *doc->lBlocks;

    int page_index = 0;
    for (int i = 0; i < list.size() && result; i++)
    {
		EjBlock* block = list[i];
        if (page_index < doc->lPages->size() - 1 && doc->lPages->at(page_index)->endBlock == i)
        {
              printer->newPage();
              page_index++;
        }
//        qDebug() << block->type;
		EjPdfDocumentWriter *editor = editors.value(block->type, nullptr);
        if (editor != nullptr)
        {
            editor->edit(document, block, &painter);
        }
    }
    painter.end();
    return result;
}

void EjConvertorPdf::setSettings(EjDocument *doc, QPrinter *printer)
{
    printer->setOutputFormat(QPrinter::PdfFormat);
#if QT_VERSION < 0x060000
    printer->setOrientation(QPrinter::Portrait);
    printer->setPageMargins (0, 15, 0, 0, QPrinter::Unit::Millimeter);
#else
	printer->setPageOrientation(QPageLayout::Portrait);
	printer->setPageMargins (QMarginsF(0, 15, 0, 0));
#endif
    printer->setFullPage(false);
}

EjConvertorPdf::EjConvertorPdf()
{
    editors.insert(e_typeBlocks::TEXT, new TextWriterPdf());
    editors.insert(e_typeBlocks::SPACE, new SpaceWriterPdf());
    editors.insert(e_typeBlocks::BASECELL, new CellWriterPdf());
    editors.insert(e_typeBlocks::EXT_IMAGE, new ImageWriterPdf());
    editors.insert(e_typeBlocks::NUM_STYLE, new StyleWriterPdf());
    editors.insert(e_typeBlocks::PROP_BA, new ByteArrayWriterPdf());
}

EjConvertorPdf::~EjConvertorPdf()
{
	for (QMap<quint8, EjPdfDocumentWriter*>::iterator i = editors.begin(); i != editors.end(); ++i)
    {
        delete *i;
    }
}
