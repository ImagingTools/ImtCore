#include <imtgeo/CAddressCollectionControllerComp.h>

#include <cmath>

// ACF includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <idoc/IDocumentMetaInfo.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtbase/CCollectionFilter.h>
#include <imtgeo/CAddressElementInfo.h>
#include <imtgeo/CAddressTypeInfo.h>


namespace imtgeo
{


QVariant CAddressCollectionControllerComp::GetObjectInformation(
        const QByteArray &informationId,
        const QByteArray &objectId) const
{
    idoc::MetaInfoPtr metaInfo = m_objectCollectionCompPtr->GetElementMetaInfo(objectId);
    if (metaInfo.IsValid()){
        if (informationId == QByteArray("Added")){
            return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_CREATION_TIME);
        }
        else if (informationId == QByteArray("LastModified")){
            return metaInfo->GetMetaInfo(idoc::IDocumentMetaInfo::MIT_MODIFICATION_TIME);
        }
    }

    return QVariant();
}


imtbase::CTreeItemModel* CAddressCollectionControllerComp::GetMetaInfo(
        const imtgql::CGqlRequest& gqlRequest,
        QString& errorMessage) const
{
    istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
    return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CAddressCollectionControllerComp::ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
    istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
    imtbase::CTreeItemModel* dataModel = nullptr;
    imtbase::CTreeItemModel* itemsModel = nullptr;
    imtbase::CTreeItemModel* notificationModel = nullptr;

    if (!m_objectCollectionCompPtr.IsValid()){
        errorMessage = QObject::tr("Internal error").toUtf8();
    }

    if (!errorMessage.isEmpty()){
        imtbase::CTreeItemModel* errorsItemModel = rootModelPtr->AddTreeModel("errors");
        errorsItemModel->SetData("message", errorMessage);
    }
    else{
        dataModel = new imtbase::CTreeItemModel();
        itemsModel = new imtbase::CTreeItemModel();
        notificationModel = new imtbase::CTreeItemModel();

        const imtgql::CGqlObject* viewParamsGql = nullptr;
        const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();
        if (inputParams.size() > 0){
            viewParamsGql = inputParams.at(0).GetFieldArgumentObjectPtr("viewParams");
        }

        imtbase::CCollectionFilter filter;
        imtbase::CCollectionFilter filterOnParentId;
        imtbase::CCollectionFilter filterOnTypeId;
        int offset = 0, count = -1;
        QString parentId = "";
        QString typeId = "";
        if (viewParamsGql != nullptr){
            offset = viewParamsGql->GetFieldArgumentValue("Offset").toInt();
            count = viewParamsGql->GetFieldArgumentValue("Count").toInt();

            QByteArray filterBA = viewParamsGql->GetFieldArgumentValue("FilterModel").toByteArray();
            if (!filterBA.isEmpty()){
                imtbase::CTreeItemModel generalModel;
                generalModel.CreateFromJson(filterBA);
                parentId = generalModel.GetData("ParentId").toString();
                typeId = generalModel.GetData("TypeId").toString();
                if (!parentId.isEmpty()){
                    QByteArrayList filteringOnParentIdInfoIds;
                    filteringOnParentIdInfoIds << "ParentId";
                    filterOnParentId.SetFilteringInfoIds(filteringOnParentIdInfoIds);
                    filterOnParentId.SetTextFilter(parentId);
                }
                if (!typeId.isEmpty()){
                    QByteArrayList filteringOnTypeIdInfoIds;
                    filteringOnTypeIdInfoIds << "TypeId";
                    filterOnTypeId.SetFilteringInfoIds(filteringOnTypeIdInfoIds);
                    filterOnTypeId.SetTextFilter(typeId);
                }

                imtbase::CTreeItemModel* filterModel = generalModel.GetTreeItemModel("FilterIds");
                if (filterModel != nullptr){
                    QByteArrayList filteringInfoIds;
                    for (int i = 0; i < filterModel->GetItemsCount(); i++){
                        QByteArray headerId = filterModel->GetData("Id", i).toByteArray();
                        if (!headerId.isEmpty()){
                            filteringInfoIds << headerId;
                        }
                    }
                    filter.SetFilteringInfoIds(filteringInfoIds);
                }

                QString filterText = generalModel.GetData("TextFilter").toString();
                if (!filterText.isEmpty()){
                    filter.SetTextFilter(filterText);
                }
                imtbase::CTreeItemModel* sortModel = generalModel.GetTreeItemModel("Sort");
                if (sortModel != nullptr){
                    QByteArray headerId = sortModel->GetData("HeaderId").toByteArray();
                    QByteArray sortOrder = sortModel->GetData("SortOrder").toByteArray();
                    if (!headerId.isEmpty() && !sortOrder.isEmpty()){
                        filter.SetSortingOrder(sortOrder == "ASC" ? imtbase::ICollectionFilter::SO_ASC : imtbase::ICollectionFilter::SO_DESC);
                        filter.SetSortingInfoIds(QByteArrayList() << headerId);
                    }
                }
            }
        }
        iprm::CParamsSet filterParams;
        filterParams.SetEditableParameter("Filter", &filter);
        filterParams.SetEditableParameter("ParentId", &filterOnParentId);
        filterParams.SetEditableParameter("TypeId", &filterOnTypeId);

        int pagesCount = std::ceil(m_objectCollectionCompPtr->GetElementsCount(&filterParams) / (double)count);
        if (pagesCount < 0){
            pagesCount = 1;
        }

        notificationModel->SetData("PagesCount", pagesCount);

        imtbase::IObjectCollection* subCollection = m_objectCollectionCompPtr->CreateSubCollection(offset, count, &filterParams);
        imtbase::ICollectionInfo::Ids collectionIds = subCollection->GetElementIds(0, -1);

        for (const QByteArray& collectionId : collectionIds){
            int itemIndex = itemsModel->InsertNewItem();

            const imtgeo::IAddressElementInfo* addressElementInfoPtr = dynamic_cast<const imtgeo::IAddressElementInfo*>(subCollection->GetObjectPtr(collectionId));

            QByteArray addressId = addressElementInfoPtr->GetId();
            QByteArray typeAddressId = addressElementInfoPtr->GetAddressTypeId();
            QByteArray parentId;
            QList<QByteArray> parents =addressElementInfoPtr->GetParentsId();
            if(!parents.isEmpty()){
                parentId = *(parents.end());
            }
            QString name = addressElementInfoPtr->GetName();
            QString address = addressElementInfoPtr->GetAddress();
            if(address == QString(""))
            {
                for(QByteArray elemId: parents)
                {
                    imtbase::IObjectCollection::DataPtr dataElementPtr;
                    if (m_objectCollectionCompPtr->GetObjectData(elemId, dataElementPtr)){
                        const imtgeo::IAddressElementInfo* addressInfoPtr =
                                dynamic_cast<const imtgeo::IAddressElementInfo*>(dataElementPtr.GetPtr());
                        imtbase::IObjectCollection::DataPtr dataTypePtr;
                        if (m_addressTypeCollectionPtr->GetObjectData(addressInfoPtr->GetAddressTypeId(), dataTypePtr)){
                            const imtgeo::IAddressTypeInfo* typeInfoPtr =
                                    dynamic_cast<const imtgeo::IAddressTypeInfo*>(dataTypePtr.GetPtr());
                            QString shortName = typeInfoPtr->GetShortName();
                            QString name = addressInfoPtr->GetName();
                            address += shortName+ " "+name + ",";
                        }
                    }
                }
                imtbase::IObjectCollection::DataPtr dataTypePtr;
                if (m_addressTypeCollectionPtr->GetObjectData(typeAddressId, dataTypePtr)){
                    const imtgeo::IAddressTypeInfo* typeInfoPtr =
                            dynamic_cast<const imtgeo::IAddressTypeInfo*>(dataTypePtr.GetPtr());
                    address += typeInfoPtr->GetShortName() + " " +name;
                }
            }

            itemsModel->SetData("Id", addressId, itemIndex);
            itemsModel->SetData("Name", name, itemIndex);
            itemsModel->SetData("FullAddress", address, itemIndex);
            itemsModel->SetData("TypeId", typeAddressId, itemIndex);
            itemsModel->SetData("ParentId", parentId, itemIndex);
        }
        itemsModel->SetIsArray(true);
        dataModel->SetExternTreeModel("items", itemsModel);
        dataModel->SetExternTreeModel("notification", notificationModel);
    }
    rootModelPtr->SetExternTreeModel("data", dataModel);

    return rootModelPtr.PopPtr();
}


} // namespace imtgeo



