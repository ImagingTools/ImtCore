#include <imtej/CReportExport.h>


// Qt includes
#include <QtCore/QUuid>

namespace imtej
{


// public methods

bool CReportExport::exportReport(EjDocument* ejDocument, QString const& file_name)
{
	if(file_name.endsWith("docx"))
    {
        EjConvertorDocx* docxConvertorPtr = new EjConvertorDocx();
        return docxConvertorPtr->convert(ejDocument, file_name);
	}
	if(file_name.endsWith("pdf"))
    {
        EjConvertorPdf* pdfConvertorPtr = new EjConvertorPdf();
        return pdfConvertorPtr->convert(ejDocument, file_name);
	}
	if(file_name.endsWith("xlsx"))
    {
        EjConvertorExcel* excelConvertorPtr = new EjConvertorExcel();
        return excelConvertorPtr->convert(ejDocument, file_name);
	}
    return false;
}


} // namespace imtej


