#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <ifile/IFilePersistence.h>
#include <iprm/IIdParam.h>

// ImtCore includes
#include <imtbase/IBinaryDataProvider.h>
#include <imtbase/IObjectCollection.h>
#include <imtcrypt/IEncryptionKeysProvider.h>
#include <imtlic/IFeatureInfoProvider.h>
#include <imtlic/IFeatureDependenciesProvider.h>


namespace imtlicgql
{


/**
	This component accepts data and returns it in encrypted form
*/
class CFeaturesExportProviderComp:
			public imtbase::IBinaryDataProvider,
			public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CFeaturesExportProviderComp);
		I_REGISTER_INTERFACE(imtbase::IBinaryDataProvider);
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Product collection", true, "ProductCollection");
		I_ASSIGN(m_packageCollectionCompPtr, "PackageCollection", "Package collection", true, "PackageCollection");
	I_END_COMPONENT;

	// reimplemented (imtbase::IBinaryDataProvider)
	virtual bool GetData(QByteArray& data, const QByteArray& dataId) const override;

private:
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_packageCollectionCompPtr);
};


} // namespace imtqml


