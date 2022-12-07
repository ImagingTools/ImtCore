#include <imtauthgql/CAuthorizationOptionsControllerComp.h>


// ACF includes
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>


namespace imtauthgql
{


imtbase::CTreeItemModel* CAuthorizationOptionsControllerComp::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_parameterCompPtr.IsValid()){
		return nullptr;
	}

	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	iprm::ISelectionParam* selectionParam = dynamic_cast<iprm::ISelectionParam*>(m_parameterCompPtr.GetPtr());
	if (selectionParam != nullptr){
		const iprm::IOptionsList* optionList = selectionParam->GetSelectionConstraints();

		Q_ASSERT(optionList != nullptr);

		int defaultIndex = selectionParam->GetSelectedOptionIndex();

		QByteArray userModeId = optionList->GetOptionId(defaultIndex);

		dataModelPtr->SetData("Value", userModeId);
	}

	if (m_userCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr userDataPtr;

		bool superUserExists = m_userCollectionCompPtr->GetObjectData("su", userDataPtr);

		dataModelPtr->SetData("SuperUserExists", superUserExists);
	}

	return rootModelPtr;
}


bool CAuthorizationOptionsControllerComp::GetOperationFromRequest(const imtgql::CGqlRequest &gqlRequest, imtgql::CGqlObject &gqlObject, QString &errorMessage, int &operationType) const
{
	const QList<imtgql::CGqlObject>* fieldList = gqlRequest.GetFields();
	if(fieldList == nullptr){
		return false;
	}

	int count = fieldList->count();
	for (int i = 0; i < count; i++){
		if (fieldList->at(i).GetId() == "AuthorizationInfo"){
			gqlObject = fieldList->at(i);
			operationType = OT_AUTHORIZATION_INFO;

			return true;
		}

		if (fieldList->at(i).GetId() == "SaveSuperUser"){
			gqlObject = fieldList->at(i);
			operationType = OT_UPDATE_SUPER_USER;

			return true;
		}
	}

	return false;
}


imtbase::CTreeItemModel *CAuthorizationOptionsControllerComp::GetAuthorizationInfo(const QList<imtgql::CGqlObject> &inputParams, const imtgql::CGqlObject &gqlObject, QString &errorMessage) const
{
	if (!m_parameterCompPtr.IsValid()){
		return nullptr;
	}

	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();

	iprm::ISelectionParam* selectionParam = dynamic_cast<iprm::ISelectionParam*>(m_parameterCompPtr.GetPtr());
	if (selectionParam != nullptr){
		const iprm::IOptionsList* optionList = selectionParam->GetSelectionConstraints();

		Q_ASSERT(optionList != nullptr);

		int defaultIndex = selectionParam->GetSelectedOptionIndex();

		QByteArray userModeId = optionList->GetOptionId(defaultIndex);

		rootModelPtr->SetData("Value", userModeId);
	}

	if (m_userCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr userDataPtr;

		bool superUserExists = m_userCollectionCompPtr->GetObjectData("su", userDataPtr);

		rootModelPtr->SetData("SuperUserExists", superUserExists);
	}

	return rootModelPtr;
}


imtbase::CTreeItemModel *CAuthorizationOptionsControllerComp::UpdateSuperUser(const QList<imtgql::CGqlObject> &inputParams, const imtgql::CGqlObject &gqlObject, QString &errorMessage) const
{
	QByteArray itemData;
	if (!inputParams.isEmpty()){
		itemData = inputParams.at(0).GetFieldArgumentValue("Item").toByteArray();
	}


	if (!itemData.isEmpty()){
		imtbase::CTreeItemModel itemModel;
		if (!itemModel.CreateFromJson(itemData)){
			return nullptr;
		}

		QByteArray superUserPassword;
		if (itemModel.ContainsKey("Password")){
			superUserPassword = itemModel.GetData("Password").toByteArray();
		}


	}

	return nullptr;
}


} // namespace imtauthgql


