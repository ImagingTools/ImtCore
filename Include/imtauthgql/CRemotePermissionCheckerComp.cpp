#include <imtauthgql/CRemotePermissionCheckerComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtauthgql
{


// public methods

// reimplemented (imtauth::IPermissionChecker)

bool CRemotePermissionCheckerComp::CheckPermission(const imtauth::IUserInfo::FeatureIds& userPermissions, const QByteArrayList& permissions)
{
	if (!m_gqlRequestHandlerCompPtr.IsValid() || !m_permissionCheckerCompPtr.IsValid()){
		SendErrorMessage(0, QString("Unable to check user permissions. Internal error."), "CRemotePermissionCheckerComp");

		return false;
	}

	bool ok = m_permissionCheckerCompPtr->CheckPermission(userPermissions, permissions);
	if (ok){
		return true;
	}

	QByteArrayList result;
	for (const QByteArray& featureId : userPermissions){
		result << featureId;

		imtgql::CGqlRequest gqlRequest(imtgql::CGqlRequest::RT_QUERY, "GetFeatureDependencies");
		imtgql::CGqlObject inputParam("input");
		inputParam.InsertField(QByteArray("FeatureId"), QVariant(featureId));
		gqlRequest.AddParam(inputParam);

		QString errorMessage;
		imtbase::CTreeItemModel* responseModelPtr = m_gqlRequestHandlerCompPtr->CreateResponse(gqlRequest, errorMessage);
		if (responseModelPtr == nullptr){
			SendErrorMessage(0, errorMessage, "CRemotePermissionCheckerComp");

			return false;
		}

		if (responseModelPtr->ContainsKey("FeatureDependencies")){
			QByteArray featureDependencies = responseModelPtr->GetData("FeatureDependencies").toByteArray();
			if (!featureDependencies.isEmpty()){
				QByteArrayList dependencyIds = featureDependencies.split(';');

				result << dependencyIds;
			}
		}

		bool ok = m_permissionCheckerCompPtr->CheckPermission(result, permissions);
		if (ok){
			return true;
		}
	}

	return false;
}


} // namespace imtauthgql


