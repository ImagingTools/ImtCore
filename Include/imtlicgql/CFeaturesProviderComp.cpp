#include <imtlicgql/CFeaturesProviderComp.h>


// ImtCore includes
#include <imtlic/CFeatureInfo.h>


namespace imtlicgql
{


// reimplemented (imtgql::CGqlRequestHandlerCompBase)

imtbase::CTreeItemModel* CFeaturesProviderComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	return nullptr;
}


} // namespace imtlicgql


