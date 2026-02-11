// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtrest/IBinaryDataProvider.h>
#include <imtrest/IBinaryDataController.h>


namespace imtlic
{


class CProductControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtrest::IBinaryDataProvider,
			virtual public imtrest::IBinaryDataController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CProductControllerComp);
		I_REGISTER_INTERFACE(imtrest::IBinaryDataProvider);
		I_REGISTER_INTERFACE(imtrest::IBinaryDataController);
		I_ASSIGN(m_productCollectionCompPtr, "ProductCollection", "Product collection", true, "ProductCollection");
		I_ASSIGN(m_featureCollectionCompPtr, "FeatureCollection", "Feature collection", true, "FeatureCollection");
		I_ASSIGN(m_versionInfoCompPtr, "VersionInfo", "Version info", true, "VersionInfo");
	I_END_COMPONENT;

	// reimplemented (imtrest::IBinaryDataProvider)
	virtual bool GetData(
				QByteArray& data,
				const QByteArray& dataId,
				qint64 readFromPosition = 0,
				qint64 readMaxLength = -1) const override;

	// reimplemented (imtrest::IBinaryDataController)
	virtual bool SetData(const QByteArray& data, QByteArray& dataId) const override;
	virtual bool RemoveData(const QByteArray& dataId) const override;
	virtual bool EnsureDataExists(const QByteArray& dataId) const override;

private:
	I_REF(imtbase::IObjectCollection, m_productCollectionCompPtr);
	I_REF(imtbase::IObjectCollection, m_featureCollectionCompPtr);
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);
};


} // namespace imtlic


