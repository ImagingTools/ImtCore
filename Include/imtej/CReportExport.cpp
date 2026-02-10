// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
		bool result = docxConvertorPtr->convert(ejDocument, file_name);
		delete docxConvertorPtr;
		return result;
	}
	if(file_name.endsWith("pdf"))
    {
        EjConvertorPdf* pdfConvertorPtr = new EjConvertorPdf();
		bool result = pdfConvertorPtr->convert(ejDocument, file_name);
		delete pdfConvertorPtr;
		return result;
	}
	if(file_name.endsWith("xlsx"))
    {
        EjConvertorExcel* excelConvertorPtr = new EjConvertorExcel();
		bool result = excelConvertorPtr->convert(ejDocument, file_name);
		delete excelConvertorPtr;
		return result;
	}
    return false;
}


} // namespace imtej


