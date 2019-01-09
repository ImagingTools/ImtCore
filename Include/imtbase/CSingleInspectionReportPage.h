#pragma once


// ACF includes
#include <iser/ISerializable.h>


namespace imtbase
{


class CSingleInspectionReportPage: virtual public iser::ISerializable
{
public:
	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive & archive);
};


} // namespace imtbase


