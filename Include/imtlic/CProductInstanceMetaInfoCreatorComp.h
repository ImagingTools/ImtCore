#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <idoc/CStandardDocumentMetaInfo.h>

// ImtCore includes
#include <imtbase/IMetaInfoCreator.h>
#include <imtlic/IProductInstanceInfoProvider.h>


namespace imtlic
{


/**
	Meta-info creator for the product instance object.
*/
class CProductInstanceMetaInfoCreatorComp:
			public ilog::CLoggerComponentBase,
			virtual public imtbase::IMetaInfoCreator
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CProductInstanceMetaInfoCreatorComp);
		I_REGISTER_INTERFACE(imtbase::IMetaInfoCreator);
		I_ASSIGN(m_objectTypeIdAttrPtr, "ObjectTypeId", "Type-ID of the document", true, "ProductLicensingInfo");
	I_END_COMPONENT;

protected:
	// reimplemented (imtbase::IMetaInfoCreator)
	virtual TypeIds GetSupportedTypeIds() const override;
	virtual bool CreateMetaInfo(const istd::IChangeable* dataPtr, const QByteArray& typeId, MetaInfoPtr& metaInfoPtr) const override;

private:
	class MetaInfo: public idoc::CStandardDocumentMetaInfo
	{
	public:
		typedef idoc::CStandardDocumentMetaInfo BaseClass;

		// reimplemented (idoc::IDocumentMetaInfo)
		virtual QString GetMetaInfoName(int metaInfoType) const override
		{
			switch (metaInfoType){
			case IProductInstanceInfoProvider::MIT_LICENSE_INSTANCE_INFO_LIST:
				return QObject::tr("Licenses");
			}

			return BaseClass::GetMetaInfoName(metaInfoType);
		}
	};

private:
	I_ATTR(QByteArray, m_objectTypeIdAttrPtr);
};


} // namespace imtlic


