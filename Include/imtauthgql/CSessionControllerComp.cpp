#include <imtauthgql/CSessionControllerComp.h>

// ImtCore includes
#include <imtauth/ISession.h>


namespace imtauthgql
{


// protected methods

// reimplemented (CObjectCollectionControllerCompBase)

imtbase::CTreeItemModel* CSessionControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& /*errorMessage*/) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");

	QByteArray sessionId = GetObjectIdFromInputParams(gqlRequest.GetParams());

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(sessionId, dataPtr)){
		const imtauth::ISession* sessionInfoPtr = dynamic_cast<const imtauth::ISession*>(dataPtr.GetPtr());
		if (sessionInfoPtr != nullptr){
			QByteArray token = sessionInfoPtr->GetToken();
			QByteArray userId = sessionInfoPtr->GetUserId();

			dataModel->SetData("UserId", userId);
			dataModel->SetData("Token", token);
		}
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtauthgql


