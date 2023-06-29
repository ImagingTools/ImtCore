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
#include <imtdb/CSqlDatabaseObjectCollectionComp.h>
#include <imtbase/CCollectionInfo.h>



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
        //imtbase::CCollectionFilter filterOnParentId;
        imtbase::CCollectionFilter filterOnParentIds;
        imtbase::CCollectionFilter filterOnTypeId;
        int offset = 0, count = -1;
        //QString parentId = "";
        QString parentIds = "";
        QString filterText = "";
        QString typeId = "";
        if (viewParamsGql != nullptr){
            offset = viewParamsGql->GetFieldArgumentValue("Offset").toInt();
            count = viewParamsGql->GetFieldArgumentValue("Count").toInt();

            QByteArray filterBA = viewParamsGql->GetFieldArgumentValue("FilterModel").toByteArray();
            //qDebug() << "filterBA:: " << filterBA;
            if (!filterBA.isEmpty()){
                imtbase::CTreeItemModel generalModel;
                generalModel.CreateFromJson(filterBA);

                //parentIds = "{00000817-0000-0000-0000-000000000000}";//generalModel.GetData("ParentId").toString();

                parentIds = generalModel.GetData("ParentIds").toString();
                //qDebug()<< "filter::: parentIds::: " << parentIds;

                if (!parentIds.isEmpty()){
                    QStringList parentIdList = parentIds.split(",");
                    QJsonArray jsonArray;
                    QJsonDocument json_document;
                    for(int i = 0; i < parentIdList.count(); i++){
                        jsonArray.append(parentIdList.at(i));
                    }
                    json_document = QJsonDocument(jsonArray);

                    //qDebug() << "jsonParents::: " << qPrintable(QJsonDocument(jsonArray).toJson(QJsonDocument::Compact));

                    QByteArrayList filteringOnParentIdsInfoIds;
                    filteringOnParentIdsInfoIds << "ParentIds";
                    filterOnParentIds.SetFilteringInfoIds(filteringOnParentIdsInfoIds);
                    filterOnParentIds.SetTextFilter(qPrintable(QJsonDocument(jsonArray).toJson(QJsonDocument::Compact)));
                }

//                if (!parentId.isEmpty()){
//                    QByteArrayList filteringOnParentIdInfoIds;
//                    filteringOnParentIdInfoIds << "ParentId";
//                    filterOnParentId.SetFilteringInfoIds(filteringOnParentIdInfoIds);
//                    filterOnParentId.SetTextFilter(parentId);
//                }


                typeId = generalModel.GetData("TypeId").toString();

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

                filterText = generalModel.GetData("TextFilter").toString();
                if (!filterText.isEmpty()){

                    filterText = filterText.trimmed();
                    if(filterText.startsWith(",")){
                        filterText = filterText.replace(0,1,"");
                        filterText = filterText.trimmed();
                    }
                    if(filterText.endsWith(",")){
                        filterText = filterText.replace(filterText.length() -1, 1,"");
                        filterText = filterText.trimmed();
                    }

                    //qDebug() << "filter::: Text::" << filterText;
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
        filterParams.SetEditableParameter("ParentIds", &filterOnParentIds);
        filterParams.SetEditableParameter("TypeId", &filterOnTypeId);

        int pagesCount = std::ceil(m_objectCollectionCompPtr->GetElementsCount(&filterParams) / (double)count);
        if (pagesCount < 0){
            pagesCount = 1;
        }

        notificationModel->SetData("PagesCount", pagesCount);


        imtdb::CSqlDatabaseObjectCollectionComp* objectCollectionCompPtr = dynamic_cast<imtdb::CSqlDatabaseObjectCollectionComp*>(m_objectCollectionCompPtr.GetPtr());
        if (objectCollectionCompPtr == nullptr){
            return nullptr;

        }
//        int countElement = objectCollectionCompPtr->GetElementsCount(&filterParams);
//        if (!countElement){
//            return nullptr;
//        }

        istd::TDelPtr<imtbase::IObjectCollectionIterator> objectCollectionIterator(objectCollectionCompPtr->CreateObjectCollectionIterator(offset, count, &filterParams));

        if (objectCollectionIterator != nullptr){
            int whileCount = 0;
            while (objectCollectionIterator->Next()){
                imtbase::IObjectCollection::DataPtr objectDataPtr;
                if (objectCollectionIterator->GetObjectData(objectDataPtr)){
                    int itemIndex = itemsModel->InsertNewItem();
                    if (itemIndex >= 0){

                        const imtgeo::IAddressElementInfo* addressElementInfoPtr = nullptr;
                        addressElementInfoPtr = dynamic_cast<const imtgeo::IAddressElementInfo*>(objectDataPtr.GetPtr());

                        QString fullAddress = addressElementInfoPtr->GetAddress();
                        QString name = addressElementInfoPtr->GetName();
                        QByteArray addressId = addressElementInfoPtr->GetId();
                        QByteArray typeAddressId = addressElementInfoPtr->GetAddressTypeId();
                        QList<QByteArray> parentsList = addressElementInfoPtr->GetParentIds();
                        QByteArray addressParentId = parentsList.isEmpty() ? QByteArray() : parentsList.last();
                        QString parentsStr = QString();
                        for(int i = 0; i < parentsList.count(); i++){
                            parentsStr.append(QString(parentsList.at(i)));
                            if(i < parentsList.count() -1){
                                parentsStr.append(",");
                            }
                        }

                        //qDebug() << "fullAddress::: "  << fullAddress;

                        itemsModel->SetData("FullAddress", fullAddress, itemIndex);
                        itemsModel->SetData("Id", addressId, itemIndex);
                        itemsModel->SetData("Name", name, itemIndex);
                        itemsModel->SetData("TypeId", typeAddressId, itemIndex);
                        itemsModel->SetData("ParentId", addressParentId, itemIndex);
                        itemsModel->SetData("ParentIds", parentsStr, itemIndex);

                    }

                }
                else{
                    //qDebug() << "тест" << " return nullptr";
                    //return nullptr;
                }
                whileCount++;
                //qDebug() << "тест" << whileCount;



            }//while
            //qDebug() << "тест" << " while exit";

        }//ITERATOR

        if(!itemsModel->GetItemsCount() && !parentIds.isEmpty() && filterText.isEmpty()){
            notificationModel->SetData("Close",true);
        }

        itemsModel->SetIsArray(true);
        dataModel->SetExternTreeModel("items", itemsModel);
        dataModel->SetExternTreeModel("notification", notificationModel);
    }
    rootModelPtr->SetExternTreeModel("data", dataModel);

    return rootModelPtr.PopPtr();
}


} // namespace imtgeo



