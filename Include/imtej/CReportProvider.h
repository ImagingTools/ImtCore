#pragma once


// ImtCore includes
#include <imtej/IEjDocumentProvider.h>


namespace imtej
{


class CReportProvider: virtual public IEjDocumentProvider
{
public:
	virtual EjDocument* getReport(iprm::IParamsSet* reportParams,  const QTranslator* translatorPtr) override;

    virtual QByteArrayList getReportIds() override;
};


} // namespace imtej


