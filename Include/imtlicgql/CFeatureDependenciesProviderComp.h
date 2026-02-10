// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlic/IFeatureDependenciesProvider.h>
#include <imtservergql/CGqlRequestHandlerCompBase.h>


namespace imtlicgql
{


class CFeatureDependenciesProviderComp: public imtservergql::CGqlRequestHandlerCompBase, virtual public imtlic::IFeatureDependenciesProvider
{
public:
	typedef imtservergql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CFeatureDependenciesProviderComp);
		I_ASSIGN(m_featureCollectionCompPtr, "FeatureCollection", "Feature collection", true, "FeatureCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

	// reimplemented (imtlic::IFeatureDependenciesProvider)
	virtual QByteArrayList GetFeatureDependencies(const QByteArray& featureId) const override;
	virtual const imtlic::IFeatureInfoProvider* GetDependencyContainer(const QByteArray& dependencyId) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_featureCollectionCompPtr);
};


} // namespace imtlicgql


