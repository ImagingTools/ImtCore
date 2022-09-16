#include <imtej/CReportExport.h>


// Qt includes
#include <QtCore/QUuid>

namespace imtej
{


// public methods

bool CReportExport::exportReport(EjDocument* ejDocument, QString const& file_name)
{	bool state = false;
	bool docx = m_convertorDocx.convert(ejDocument, "C:/ImagingTools/Tmp/tests.docx");
	bool pdf = m_convertorPdf.convert(ejDocument, "C:/ImagingTools/Tmp/tests.pdf");
	bool excel = m_convertorExcel.convert(ejDocument, "C:/ImagingTools/Tmp/tests.xlsx");
	if(docx)
	{
		state = true;
	}
	return state;
}


} // namespace imtej


