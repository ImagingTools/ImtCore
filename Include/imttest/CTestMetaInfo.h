#pragma once


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imttest
{


class CTestMetaInfo: public idoc::CStandardDocumentMetaInfo
{
public:
	typedef idoc::CStandardDocumentMetaInfo BaseClass;

	// reimplemented (idoc::IDocumentMetaInfo)
	virtual QString GetMetaInfoName(int metaInfoType) const override;
};


} // namespace imtlic


