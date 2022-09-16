#pragma once


// ImtCore includes
#include <imtej/IReportProvider.h>


namespace imtej
{


class CReportProvider: virtual public IReportProvider
{
public:
	virtual EjDocument* getReport(iprm::IParamsSet* reportParams) override;
};


} // namespace imtej


