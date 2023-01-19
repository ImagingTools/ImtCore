#include <imtauthgql/CUserCollectionControllerComp.h>


// STL includes
#include <cmath>

// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imtauth/CUserInfo.h>

// ImtCore includes
#include <imtbase/CCollectionFilter.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtguigql::CObjectCollectionControllerCompBase)

imtbase::CTreeItemModel* CUserCollectionControllerComp::ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModelPtr->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");
		imtbase::CTreeItemModel* itemsModel = new imtbase::CTreeItemModel();
		imtbase::CTreeItemModel* notificationModel = new imtbase::CTreeItemModel();

		const QList<imtgql::CGqlObject>* inputParams = gqlRequest.GetParams();

		const imtgql::CGqlObject* viewParamsGql = nullptr;
		if (inputParams->size() > 0){
			viewParamsGql = inputParams->at(0).GetFieldArgumentObjectPtr("viewParams");
		}

		iprm::CParamsSet filterParams;
		imtbase::CCollectionFilter m_filter;
		int offset = 0, count = -1;
		if (viewParamsGql != nullptr){
			offset = viewParamsGql->GetFieldArgumentValue("Offset").toInt();
			count = viewParamsGql->GetFieldArgumentValue("Count").toInt();

			QByteArray filterBA = viewParamsGql->GetFieldArgumentValue("FilterModel").toByteArray();
			if (!filterBA.isEmpty()){
				imtbase::CTreeItemModel generalModel;
				generalModel.CreateFromJson(filterBA);

				imtbase::CTreeItemModel* filterModel = generalModel.GetTreeItemModel("FilterIds");
				if (filterModel != nullptr){
					QByteArrayList filteringInfoIds;
					for (int i = 0; i < filterModel->GetItemsCount(); i++){
						QByteArray headerId = filterModel->GetData("Id", i).toByteArray();
						if (!headerId.isEmpty()){
							filteringInfoIds << headerId;
						}
					}
					m_filter.SetFilteringInfoIds(filteringInfoIds);
				}

				QString filterText = generalModel.GetData("TextFilter").toString();
				if (!filterText.isEmpty()){
					m_filter.SetTextFilter(filterText);
				}

				imtbase::CTreeItemModel* sortModel = generalModel.GetTreeItemModel("Sort");
				if (sortModel != nullptr){
					QByteArray headerId = sortModel->GetData("HeaderId").toByteArray();
					QByteArray sortOrder = sortModel->GetData("SortOrder").toByteArray();
					if (!headerId.isEmpty() && !sortOrder.isEmpty()){
						m_filter.SetSortingOrder(sortOrder == "ASC" ? imtbase::ICollectionFilter::SO_ASC : imtbase::ICollectionFilter::SO_DESC);
						m_filter.SetSortingInfoIds(QByteArrayList() << headerId);
					}
				}
			}

			filterParams.SetEditableParameter("Filter", &m_filter);
			this->SetAdditionalFilters(*viewParamsGql, &filterParams);
		}

		int pagesCount = std::ceil(m_objectCollectionCompPtr->GetElementsCount(&filterParams) / (double)count);
		if (pagesCount < 0){
			pagesCount = 1;
		}

		notificationModel->SetData("PagesCount", pagesCount);

		const imtgql::IGqlContext* contextPtr = gqlRequest.GetGqlContext();
		if (contextPtr != nullptr){
			const imtauth::IUserInfo* contextUserInfoPtr = contextPtr->GetUserInfo();
			if (contextUserInfoPtr != nullptr){
				imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds(offset, count, &filterParams);
				for (const QByteArray& collectionId : collectionIds){
					imtbase::IObjectCollection::DataPtr dataPtr;
					if (m_objectCollectionCompPtr->GetObjectData(collectionId, dataPtr)){
						imtauth::IUserInfo* userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(dataPtr.GetPtr());
						if (userInfoPtr != nullptr){
							bool add = false;

							if (!userInfoPtr->IsAdmin()){
								add = true;
							}
							else if (contextUserInfoPtr != nullptr){
								add = contextUserInfoPtr->IsAdmin();
							}

							if (add){
								int itemIndex = itemsModel->InsertNewItem();
								if (itemIndex >= 0){
									if (!SetupGqlItem(gqlRequest, *itemsModel, itemIndex, collectionId, errorMessage)){
										return nullptr;
									}
								}
							}
						}
					}
				}
			}
		}

		dataModel->SetExternTreeModel("items", itemsModel);
		dataModel->SetExternTreeModel("notification", notificationModel);
	}

	return rootModelPtr.PopPtr();
}


QVariant CUserCollectionControllerComp::GetObjectInformation(const QByteArray &informationId, const QByteArray &objectId) const
{
	idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetDataMetaInfo(objectId);

	if (metaInfo.IsValid()){
		if (informationId == QByteArray("Username")){
			return metaInfo->GetMetaInfo(imtauth::IUserInfo::MIT_USERNAME);
		}
		else if (informationId == QByteArray("Email")){
			return metaInfo->GetMetaInfo(imtauth::IUserInfo::MIT_EMAIL);
		}
		else if (informationId == QByteArray("Added")){
			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME);
		}
		else if (informationId == QByteArray("LastModified")){
			return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME);
		}
	}

	return QVariant();
}


imtbase::CTreeItemModel* CUserCollectionControllerComp::GetMetaInfo(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	QByteArray userId = GetObjectIdFromInputParams(*gqlRequest.GetParams());

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(userId, dataPtr)){
		imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");
		const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(dataPtr.GetPtr());
		if (userInfoPtr != nullptr){
			imtauth::IUserInfo::RoleIds rolesIds = userInfoPtr->GetRoles();

			int index = dataModel->InsertNewItem();
			dataModel->SetData("Name", "Roles", index);

			imtbase::CTreeItemModel* children = dataModel->AddTreeModel("Children", index);

			for (const QByteArray& productRoleId : rolesIds){
				QStringList data = QString(productRoleId).split(*m_separatorObjectIdAttrPtr);

				if (data.size() == 2){
					QByteArray roleId = data[0].toUtf8();
					QByteArray productId = data[1].toUtf8();

					int childrenIndex = children->InsertNewItem();
					children->SetData("Value", roleId + " (" + productId + ")", childrenIndex);
				}
			}
		}
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtauth


