// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CRemotePermissionCheckerComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>


namespace imtauthgql
{


// public methods

// reimplemented (imtauth::IPermissionChecker)

bool CRemotePermissionCheckerComp::CheckPermission(const imtauth::IUserInfo::FeatureIds& userPermissions, const QByteArrayList& permissions)
{
	qDebug() << "CheckPermission" << userPermissions << permissions;

	if (!m_gqlRequestHandlerCompPtr.IsValid() || !m_permissionCheckerCompPtr.IsValid()){
		SendErrorMessage(0, QString("Unable to check user permissions. Internal error."), "CRemotePermissionCheckerComp");

		return false;
	}

	bool ok = m_permissionCheckerCompPtr->CheckPermission(userPermissions, permissions);
	if (ok){
		return true;
	}

	imtgql::CGqlRequest gqlRequest(imtgql::CGqlRequest::RT_QUERY, "GetFeatureDependencies");
	imtgql::CGqlParamObject inputParam;
	inputParam.InsertParam(QByteArray("FeatureIds"), QVariant(permissions.join(';')));
	gqlRequest.AddParam("input", inputParam);

	QString errorMessage;
	imtbase::CTreeItemModel* responseModelPtr = m_gqlRequestHandlerCompPtr->CreateResponse(gqlRequest, errorMessage);
	if (responseModelPtr == nullptr){
		SendErrorMessage(0, errorMessage, "CRemotePermissionCheckerComp");

		return false;
	}

	QByteArrayList result;

	result << permissions;

	if (responseModelPtr->ContainsKey("FeaturesDependencies")){
		QByteArray featureDependencies = responseModelPtr->GetData("FeaturesDependencies").toByteArray();
		if (!featureDependencies.isEmpty()){
			QByteArrayList dependencyIds = featureDependencies.split(';');

			qDebug() << "\n";
			qDebug() << "Features: " << permissions;
			qDebug() << "Dependencies: " << dependencyIds;
			qDebug() << "\n";

			result << dependencyIds;
		}
	}

	ok = m_permissionCheckerCompPtr->CheckPermission(userPermissions, result);

	return ok;
}


} // namespace imtauthgql


