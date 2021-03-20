#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IMetaInfoCreator.h>


namespace imtbase
{


class CObjectMetaInfoCreatorCompBase:
			public ilog::CLoggerComponentBase,
			virtual public imtbase::IMetaInfoCreator
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CObjectMetaInfoCreatorCompBase);
		I_REGISTER_INTERFACE(imtbase::IMetaInfoCreator);
		I_ASSIGN(m_objectTypeIdAttrPtr, "ObjectTypeId", "Type-ID of the object supported by this meta info creator", true, "ObjectTypeId");
	I_END_COMPONENT;

protected:
	// reimplemented (imtbase::IMetaInfoCreator)
	virtual TypeIds GetSupportedTypeIds() const override;

protected:
	I_ATTR(QByteArray, m_objectTypeIdAttrPtr);
};


} // namespace imtlic


