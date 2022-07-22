#pragma once


// ImtCore includes
#include <imtbase/CObjectMetaInfoCreatorCompBase.h>


namespace imtimg
{


/**
	Meta-info creator for the bitmap object.
*/
class CBitmapMetaInfoCreatorComp: public imtbase::CObjectMetaInfoCreatorCompBase
{
public:
	typedef imtbase::CObjectMetaInfoCreatorCompBase BaseClass;

	I_BEGIN_COMPONENT(CBitmapMetaInfoCreatorComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtbase::IMetaInfoCreator)
	virtual bool CreateMetaInfo(
				const istd::IChangeable* dataPtr,
				const QByteArray& typeId,
				idoc::MetaInfoPtr& metaInfoPtr) const override;
};


} // namespace imtimg


