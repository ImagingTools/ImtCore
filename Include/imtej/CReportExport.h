#pragma once


// Qt includes
#include <QtCore/QMap>

// ImtCore includes
#include <imtej/IReportExport.h>


namespace imtej
{


class CReportExport: virtual public IReportExport
{
public:
	virtual bool exportReport(EjDocument* ejDocument) override;
private:
	EjConvertorDocx m_convertorDocx;
	EjConvertorPdf m_convertorPdf;
	EjConvertorExcel m_convertorExcel;

};


} // namespace imtej


