#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtgql/CGqlRepresentationDataControllerComp.h>


namespace imtlic
{


class CPermissionsProviderComp: public imtgql::CGqlRequestHandlerCompBase
{
public:
	typedef imtgql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CPermissionsProviderComp)
		I_ASSIGN(m_featurePackageRepresentationCompPtr, "PackageRepresentation", "Representation for feature package", true, "PackageRepresentation");
		I_ASSIGN(m_featureContainerCompPtr, "FeatureContainer", "FeatureContainer", true, "FeatureContainer");
	I_END_COMPONENT

protected:
	// reimplemented (imtgql::CGqlRepresentationDataControllerComp)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtbase::IRepresentationController, m_featurePackageRepresentationCompPtr);
	I_REF(istd::IChangeable, m_featureContainerCompPtr);
};


} // namespace imtlic


