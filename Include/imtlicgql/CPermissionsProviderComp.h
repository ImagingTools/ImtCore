#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtservergql/CGqlRequestHandlerCompBase.h>


namespace imtlicgql
{


class CPermissionsProviderComp: public imtservergql::CGqlRequestHandlerCompBase
{
public:
	typedef imtservergql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CPermissionsProviderComp)
		I_ASSIGN(m_featurePackageRepresentationCompPtr, "PackageRepresentation", "Representation for feature package", true, "PackageRepresentation");
		I_ASSIGN(m_featureContainerCompPtr, "FeatureContainer", "FeatureContainer", true, "FeatureContainer");
	I_END_COMPONENT

protected:
	// reimplemented (imtservergql::CGqlRequestHandlerCompBase)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtbase::IRepresentationController, m_featurePackageRepresentationCompPtr);
	I_REF(istd::IChangeable, m_featureContainerCompPtr);
};


} // namespace imtlicgql


