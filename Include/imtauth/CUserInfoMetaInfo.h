#pragma once


// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>


namespace imtauth
{


/**
	Meta-info creator for user information object.
*/
class CUserInfoMetaInfo: public idoc::CStandardDocumentMetaInfo
{
public:
	typedef idoc::CStandardDocumentMetaInfo BaseClass;

	// reimplemented (idoc::IDocumentMetaInfo)
	virtual QString GetMetaInfoName(int metaInfoType) const override;
};


} // namespace imtauth


