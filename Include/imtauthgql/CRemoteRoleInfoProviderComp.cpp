#include <imtauthgql/CRemoteRoleInfoProviderComp.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <iser/CJsonMemReadArchive.h>

// ImtCore includes
#include <imtbase/CCollectionInfo.h>
#include <imtgql/CGqlRequest.h>
#include <imtbase/CTreeItemModel.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Roles.h>


namespace imtauthgql
{


// protected methods

// reimplemented (IRoleInfoProvider)

const imtbase::ICollectionInfo& CRemoteRoleInfoProviderComp::GetRoleList() const
{
	static imtbase::CCollectionInfo emptyCollection;
	return emptyCollection;
}


const imtauth::IRole* CRemoteRoleInfoProviderComp::GetRole(const QByteArray& /*roleId*/, const QByteArray& /*productId*/) const
{
	return nullptr;
}


const imtauth::IRole* CRemoteRoleInfoProviderComp::GetRole(const QByteArray& objectId) const
{
	if (!m_roleInfoFactCompPtr.IsValid()){
		return nullptr;
	}

	imtgql::CGqlRequest request(imtgql::CGqlRequest::RT_QUERY, sdl::imtauth::Roles::V1_0::CRoleItemGqlRequest::GetCommandId());
	imtgql::CGqlObject inputObject;
	inputObject.InsertField(QByteArray("Id"), QVariant(objectId));
	inputObject.InsertField(QByteArray("IsJsonSerialized"), QVariant(true));
	request.AddParam("input", inputObject);

	imtgql::CGqlObject itemObject;
	itemObject.InsertField("Id");
	request.AddField("item", itemObject);

	imtbase::CTreeItemModel responseModel;
	bool retVal = SendModelRequest(request, responseModel);
	if (retVal){
		imtbase::CTreeItemModel* roleDataModelPtr = responseModel.GetTreeItemModel("data");
		if (roleDataModelPtr == nullptr){
			return nullptr;
		}

		QByteArray roleJson = roleDataModelPtr->ToJson().toUtf8();

		istd::TDelPtr<imtauth::IRole> roleInfoPtr;
		roleInfoPtr.SetPtr(m_roleInfoFactCompPtr.CreateInstance());
		if (!roleInfoPtr.IsValid()){
			return nullptr;
		}

		iser::CJsonMemReadArchive archive(roleJson);
		if (!roleInfoPtr->Serialize(archive)){
			return nullptr;
		}

		return roleInfoPtr.PopPtr();
	}

	return nullptr;
}


} // namespace imtauthgql


