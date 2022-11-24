#pragma once

// ACF includes
#include <istd/IPolymorphic.h>
#include <iprm/IParamsSet.h>

//EjDoc includes
#include <ejdocument.h>
#include <ejtableblocks.h>

namespace imtej
{


class IObject2d;


/**
	Interface for a creating report in EjDocument format.
*/

class IReportProvider: virtual public istd::IPolymorphic
{
public:
    enum ReportInfo
    {
        RI_SUCCESS = 1,
        RI_NOT_SUCCESS = 0
    };

	virtual ReportInfo createReport(iprm::IParamsSet* reportParams, const QString& reportPath, const QByteArray& userLanguage) = 0;

    virtual QByteArrayList getReportIds() = 0;
};


} // namespace imtej


