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
	virtual EjDocument* getReport(iprm::IParamsSet* reportParams) = 0;
};


} // namespace imtej


