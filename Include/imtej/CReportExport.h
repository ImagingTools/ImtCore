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
	virtual bool exportReport(EjDocument* ejDocument, QString const& file_name) override;
private:


};


} // namespace imtej


