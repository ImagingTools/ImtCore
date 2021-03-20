#pragma once


// ImtCore includes
#include <imtbase/CObjectMetaInfoCreatorCompBase.h>


namespace imtauth
{


/**
	Meta-info creator for the person information object.
*/
class CPersonInfoMetaInfoCreatorComp: public imtbase::CObjectMetaInfoCreatorCompBase
{
public:
	typedef imtbase::CObjectMetaInfoCreatorCompBase BaseClass;

	I_BEGIN_COMPONENT(CPersonInfoMetaInfoCreatorComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtbase::IMetaInfoCreator)
	virtual bool CreateMetaInfo(const istd::IChangeable* dataPtr, const QByteArray& typeId, MetaInfoPtr& metaInfoPtr) const override;
};


} // namespace imtauth


