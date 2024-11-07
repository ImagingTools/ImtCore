#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IBinaryDataProvider.h>
#include <imtbase/IBinaryDataController.h>
#include <imtbase/IObjectCollection.h>


namespace imtlic
{


class CProductControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtbase::IBinaryDataProvider,
			virtual public imtbase::IBinaryDataController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CProductControllerComp);
		I_REGISTER_INTERFACE(imtbase::IBinaryDataProvider);
		I_REGISTER_INTERFACE(imtbase::IBinaryDataController);
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Product collection", true, "ProductCollection");
		I_ASSIGN(m_featureCollectionCompPtr, "FeatureCollection", "Feature collection", true, "FeatureCollection");
		I_ASSIGN(m_versionInfoCompPtr, "VersionInfo", "Version info", true, "VersionInfo");
	I_END_COMPONENT;

	// reimplemented (imtbase::IBinaryDataProvider)
	virtual bool GetData(
				QByteArray& data,
				const QByteArray& dataId,
				qint64 readFromPosition = 0,
				qint64 readMaxLength = -1) const override;

	// reimplemented (imtbase::IBinaryDataController)
	virtual bool SetData(const QByteArray& data, QByteArray& dataId) const override;
	virtual bool RemoveData(const QByteArray& dataId) const override;
	virtual bool EnsureDataExists(const QByteArray& dataId) const override;

private:
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_featureCollectionCompPtr);
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);
};


} // namespace imtlic


