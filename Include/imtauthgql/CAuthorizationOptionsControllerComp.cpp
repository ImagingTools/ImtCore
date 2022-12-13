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


} // namespace imtauthgql


