#include <imtgql/CGqlRepresentationDataControllerComp.h>


// ACF includer
#include <istd/TDelPtr.h>
#include <iprm/CParamsSet.h>
#include <iprm/CIdParam.h>
#include <imod/TModelWrap.h>


namespace imtgql
{


// protected methods

// reimplemented (imtgql::IGqlRepresentationController)

imtbase::CTreeItemModel* CGqlRepresentationDataControllerComp::CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (m_representationControllerCompPtr.IsValid() && m_dataModelCompPtr.IsValid()){
		istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

		imtbase::CTreeItemModel* representationPtr = rootModelPtr->AddTreeModel("data");
		Q_ASSERT(representationPtr != nullptr);

		istd::TDelPtr<iprm::IParamsSet> representationParamsPtr(CreateContextParams(gqlRequest));

		bool result = m_representationControllerCompPtr->GetRepresentationFromDataModel(*m_dataModelCompPtr, *representationPtr, representationParamsPtr.GetPtr());
		if (result){
			return rootModelPtr.PopPtr();
		}
	}

	return nullptr;
}


bool CGqlRepresentationDataControllerComp::UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const
{
	if (!m_representationControllerCompPtr.IsValid() || !m_dataModelCompPtr.IsValid()){
		return false;
	}

	bool retVal = m_representationControllerCompPtr->GetDataModelFromRepresentation(*representationPtr, *m_dataModelCompPtr);

	return retVal;
}


// reimplemented (imtgql::CGqlRequestHandlerCompBase)

imtbase::CTreeItemModel* CGqlRepresentationDataControllerComp::CreateInternalResponse(const CGqlRequest& gqlRequest, QString& errorMessage) const
{
	QByteArray commandId = gqlRequest.GetCommandId();

	imtgql::IGqlRequest::RequestType requestType = gqlRequest.GetRequestType();

	if (requestType == imtgql::IGqlRequest::RT_QUERY){
		return CreateRepresentationFromRequest(gqlRequest, errorMessage);
	}
	else if (requestType == imtgql::IGqlRequest::RT_MUTATION){
		const QList<CGqlObject> params = gqlRequest.GetParams();
		if (!params.isEmpty()){
			QByteArray itemData = params.at(0).GetFieldArgumentValue("Item").toByteArray();
			if (!itemData.isEmpty()){
				istd::TDelPtr<imtbase::CTreeItemModel> representationPtr(new imtbase::CTreeItemModel);
				if (representationPtr->CreateFromJson(itemData)){
					bool result = UpdateModelFromRepresentation(gqlRequest, representationPtr.GetPtr());
					if (result){
						return representationPtr.PopPtr();
					}
				}
			}
		}
	}

	Q_ASSERT(false);

	return nullptr;
}


iprm::IParamsSet* CGqlRepresentationDataControllerComp::CreateContextParams(const imtgql::CGqlRequest& gqlRequest) const
{
	const QList<imtgql::CGqlObject> requestParamsPtr = gqlRequest.GetParams();

	QByteArray productId;
	if (!requestParamsPtr.empty()){
		productId = requestParamsPtr.at(0).GetFieldArgumentValue("ProductId").toByteArray();
	}

	istd::TDelPtr<iprm::CParamsSet> paramsPtr = new imod::TModelWrap<iprm::CParamsSet>();
	if (gqlRequest.GetGqlContext() != nullptr){
		const imtauth::IUserInfo* userInfoPtr = gqlRequest.GetGqlContext()->GetUserInfo();
		Q_ASSERT(userInfoPtr != nullptr);
		if (userInfoPtr != nullptr){
			istd::TDelPtr<imtauth::IUserInfo> userInfoParamPtr;

			userInfoParamPtr.SetCastedOrRemove(userInfoPtr->CloneMe());
			Q_ASSERT(userInfoParamPtr.IsValid());

			paramsPtr->SetEditableParameter("UserInfo", userInfoParamPtr.PopPtr(), true);
		}

		istd::TDelPtr<iprm::IIdParam> languageIdParam = new iprm::CIdParam();
		languageIdParam->SetId(gqlRequest.GetGqlContext()->GetLanguageId());

		paramsPtr->SetEditableParameter("LanguageParam", languageIdParam.PopPtr(), true);

		istd::TDelPtr<iprm::IIdParam> productIdParam = new iprm::CIdParam();
		productIdParam->SetId(productId);

		paramsPtr->SetEditableParameter("ProductId", productIdParam.PopPtr(), true);
	}

	return paramsPtr.PopPtr();
}


} // namespace imtgql


