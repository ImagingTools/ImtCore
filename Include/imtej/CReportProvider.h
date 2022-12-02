#pragma once


// ImtCore includes
#include <imtej/IEjDocumentProvider.h>


namespace imtej
{


class CReportProvider: virtual public IEjDocumentProvider
{
public:
    virtual EjDocument* getReport(iprm::IParamsSet* reportParams, const istd::IChangeable* dataObjectPtr, const iqt::ITranslationManager* translatorPtr, const QByteArray& userLanguage) override;

    virtual QByteArrayList getReportIds() override;
};


} // namespace imtej


