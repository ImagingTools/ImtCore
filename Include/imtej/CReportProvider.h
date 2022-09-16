#pragma once


// ImtCore includes
#include <imtej/IReportProvider.h>


namespace imtej
{


class CReportProvider: virtual public IReportProvider
{
public:
	virtual EjDocument* getReport(int reportId) override;
};


} // namespace imtej


