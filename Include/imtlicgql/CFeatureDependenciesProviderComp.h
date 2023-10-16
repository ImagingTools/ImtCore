#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtlic/IFeatureDependenciesProvider.h>
#include <imtgql/CGqlRequestHandlerCompBase.h>


namespace imtlicgql
{


class CFeatureDependenciesProviderComp: public imtgql::CGqlRequestHandlerCompBase, virtual public imtlic::IFeatureDependenciesProvider
{
public:
	typedef imtgql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CFeatureDependenciesProviderComp);
		I_ASSIGN(m_featureCollectionCompPtr, "FeatureCollection", "Feature collection", true, "FeatureCollection");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationDataControllerComp)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

	// reimplemented (imtlic::IFeatureDependenciesProvider)
	virtual QByteArrayList GetFeatureDependencies(const QByteArray& featureId) const override;
	virtual const imtlic::IFeatureInfoProvider* GetDependencyContainer(const QByteArray& dependencyId) const override;

protected:
	I_REF(imtbase::IObjectCollection, m_featureCollectionCompPtr);
};


} // namespace imtlicgql


