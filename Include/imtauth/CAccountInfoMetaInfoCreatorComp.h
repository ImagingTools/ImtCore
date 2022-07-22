#pragma once


// ImtCore includes
#include <imtbase/CObjectMetaInfoCreatorCompBase.h>


namespace imtauth
{


/**
	Meta-info creator for account information object.
*/
class CAccountInfoMetaInfoCreatorComp: public imtbase::CObjectMetaInfoCreatorCompBase
{
public:
	typedef imtbase::CObjectMetaInfoCreatorCompBase BaseClass;

	I_BEGIN_COMPONENT(CAccountInfoMetaInfoCreatorComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtbase::IMetaInfoCreator)
	virtual bool CreateMetaInfo(const istd::IChangeable* dataPtr, const QByteArray& typeId, idoc::MetaInfoPtr& metaInfoPtr) const override;
};


} // namespace imtauth


