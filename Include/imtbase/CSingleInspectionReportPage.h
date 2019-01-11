#pragma once


// ImtCore includes
#include <imtbase/IReportPage.h>


namespace imtbase
{


class CSingleInspectionReportPage: virtual public IReportPage
{
public:
	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive & archive);
};


} // namespace imtbase


