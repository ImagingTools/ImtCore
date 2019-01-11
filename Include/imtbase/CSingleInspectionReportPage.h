#pragma once


// ImtCore includes
#include <imtbase/CReportPage.h>


namespace imtbase
{


class CSingleInspectionReportPage: public CReportPage
{
public:
	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive & archive);
};


} // namespace imtbase


