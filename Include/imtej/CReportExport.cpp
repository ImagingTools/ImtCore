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
		return m_convertorDocx.convert(ejDocument, "C:/ImagingTools/Tmp/" + file_name);
	}
	if(file_name.endsWith("pdf"))
	{
		return m_convertorPdf.convert(ejDocument, "C:/ImagingTools/Tmp/" + file_name);
	}
	if(file_name.endsWith("xlsx"))
	{
		return m_convertorExcel.convert(ejDocument, "C:/ImagingTools/Tmp/" + file_name);;
	}
}


} // namespace imtej


