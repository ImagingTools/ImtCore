#include <imtauthgql/CLogoutControllerComp.h>


// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtauth/CSessionInfo.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationControllerCompBase)

imtbase::CTreeItemModel* CLogoutControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	QByteArray token;
	const QList<imtgql::CGqlObject> paramList = gqlRequest.GetParams();
	if (!paramList.isEmpty()){
		if (paramList.at(0).GetFieldIds().contains("Token")){
			token = paramList.at(0).GetFieldArgumentValue("Token").toByteArray();
		}
	}

	if (m_gqlContextCacheControllerCompPtr.IsValid()){
		bool isCached = m_gqlContextCacheControllerCompPtr->TokenIsCached(token);
		if (isCached){
			if (!m_gqlContextCacheControllerCompPtr->RemoveCache(token)){
				return nullptr;
			}
		}
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtauthgql


