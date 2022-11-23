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
	Interface for export EjDocument format to Office formats.
*/

class IEjDocumentProvider: virtual public istd::IPolymorphic
{
public:
	virtual EjDocument* getReport(iprm::IParamsSet* reportParams, const QTranslator* translatorPtr) = 0;

    virtual QByteArrayList getReportIds() = 0;
};


} // namespace imtej


