#include <imtauthgql/CRemoteUserGroupInfoProviderComp.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <iser/CJsonMemReadArchive.h>

// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtgql/CGqlRequest.h>
#include <imtbase/CTreeItemModel.h>


namespace imtauthgql
{


// protected methods

// reimplemented (IUserGroupInfoProvider)

const imtbase::ICollectionInfo& CRemoteUserGroupInfoProviderComp::GetUserGroupList() const
{
	static imtbase::CCollectionInfo emptyCollection;
	return emptyCollection;
}


const imtauth::IUserGroupInfo* CRemoteUserGroupInfoProviderComp::GetUserGroup(const QByteArray& groupId) const
{
	if (!m_userGroupInfoFactCompPtr.IsValid()){
		return nullptr;
	}

	imtgql::CGqlRequest request(imtgql::CGqlRequest::RT_QUERY, "GroupItem");
	imtgql::CGqlObject inputObject("input");
	inputObject.InsertField(QByteArray("Id"), QVariant(groupId));
	inputObject.InsertField(QByteArray("IsJsonSerialized"), QVariant(true));
	request.AddParam(inputObject);

	imtgql::CGqlObject itemObject("item");
	itemObject.InsertField("Id");
	request.AddField(itemObject);

	imtbase::CTreeItemModel responseModel;
	bool retVal = SendModelRequest(request, responseModel);
	if (retVal){
		imtbase::CTreeItemModel* roleDataModelPtr = responseModel.GetTreeItemModel("data");
		if (roleDataModelPtr == nullptr){
			return nullptr;
		}

		QByteArray json = roleDataModelPtr->toJSON().toUtf8();

		istd::TDelPtr<imtauth::IUserGroupInfo> groupInfoPtr;
		groupInfoPtr.SetPtr(m_userGroupInfoFactCompPtr.CreateInstance());
		if (!groupInfoPtr.IsValid()){
			return nullptr;
		}

		iser::CJsonMemReadArchive archive(json);
		if (!groupInfoPtr->Serialize(archive)){
			return nullptr;
		}

		return groupInfoPtr.PopPtr();
	}

	return nullptr;
}


} // namespace imtauthgql


