// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


