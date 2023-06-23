#include <imtauthgql/CPumaRoleInfoProviderComp.h>


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

// reimplemented (IRoleInfoProvider)

const imtbase::ICollectionInfo& CPumaRoleInfoProviderComp::GetRoleList() const
{
	static imtbase::CCollectionInfo emptyCollection;
	return emptyCollection;
}


const imtauth::IRole* CPumaRoleInfoProviderComp::GetRole(const QByteArray& roleId, const QByteArray& productId) const
{
	return nullptr;
}


const imtauth::IRole* CPumaRoleInfoProviderComp::GetRole(const QByteArray& objectId) const
{
	if (!m_roleInfoFactCompPtr.IsValid()){
		return nullptr;
	}

	imtgql::CGqlRequest request(imtgql::CGqlRequest::RT_QUERY, "RoleItem");
	imtgql::CGqlObject inputObject("input");
	inputObject.InsertField(QByteArray("Id"), QVariant(objectId));
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

		QByteArray roleJson = roleDataModelPtr->toJSON().toUtf8();

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


