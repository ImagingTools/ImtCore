#pragma once

// ACF includes
#include <istd/IPolymorphic.h>

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
	virtual EjDocument* getReport(int reportId) = 0;
};


} // namespace imtej


