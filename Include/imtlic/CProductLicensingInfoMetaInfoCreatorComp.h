#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IMetaInfoCreator.h>


namespace imtlic
{


class CProductLicensingInfoMetaInfoCreatorComp:
			public ilog::CLoggerComponentBase,
			virtual public imtbase::IMetaInfoCreator
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CProductLicensingInfoMetaInfoCreatorComp);
		I_REGISTER_INTERFACE(imtbase::IMetaInfoCreator);
		I_ASSIGN(m_objectTypeIdAttrPtr, "ObjectTypeId", "Type-ID of the document", true, "Application");
	I_END_COMPONENT;

protected:
	// reimplemented (imtbase::IMetaInfoCreator)
	virtual TypeIds GetSupportedTypeIds() const override;
	virtual bool CreateMetaInfo(const istd::IChangeable* dataPtr, const QByteArray& typeId, MetaInfoPtr& metaInfoPtr) const override;

private:
	I_ATTR(QByteArray, m_objectTypeIdAttrPtr);
};


} // namespace imtlic


