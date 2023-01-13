#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtlic/IFeaturePackage.h>
#include <imtgql/CGqlRepresentationDataControllerComp.h>


namespace imtlic
{


class CPermissionsProviderComp: public imtgql::CGqlRequestHandlerCompBase
{
public:
	typedef imtgql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CPermissionsProviderComp)
		I_ASSIGN(m_featurePackageCompPtr, "FeaturePackage", "Feature package", true, "FeaturePackage");
		I_ASSIGN(m_featurePackageRepresentationCompPtr, "PackageRepresentation", "Representation for feature package", true, "PackageRepresentation");
	I_END_COMPONENT

protected:
	// reimplemented (imtgql::CGqlRepresentationDataControllerComp)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtlic::IFeaturePackage, m_featurePackageCompPtr);
	I_REF(imtbase::IRepresentationController, m_featurePackageRepresentationCompPtr);
};


} // namespace imtlic


