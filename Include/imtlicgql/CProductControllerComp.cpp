#include <imtlicgql/CProductControllerComp.h>


// ACF includes
#include <iprm/CIdParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtlic/CProductInfo.h>


namespace imtlicgql
{


istd::IChangeable* CProductControllerComp::CreateObject(
			const imtgql::CGqlRequest& gqlRequest,
			QByteArray& objectId,
			QString& name,
			QString& description,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QT_TR_NOOP("Internal error.");
		SendErrorMessage(0, errorMessage, "Product controller");

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr == nullptr){
		errorMessage = QT_TR_NOOP("Unable to create object. GQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Product controller");

		return nullptr;
	}

	objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();

	QByteArray jsonItemData = inputParamPtr->GetFieldArgumentValue("Item").toByteArray();
	if (jsonItemData.isEmpty()){
		errorMessage = QT_TR_NOOP("Unable to create object from empty json.");
		SendErrorMessage(0, errorMessage, "Product controller");

		return nullptr;
	}

	imtbase::CTreeItemModel productModel;
	if (!productModel.CreateFromJson(jsonItemData)){
		errorMessage = QT_TR_NOOP(QString("Unable to create tree item model from json: %1.").arg(qPrintable(jsonItemData)));
		SendErrorMessage(0, errorMessage, "Product controller");

		return nullptr;
	}

	istd::TDelPtr<imtlic::CProductInfo> productInfoPtr;
	productInfoPtr.SetPtr(new imtlic::CProductInfo);

	QByteArray productId;
	if (productModel.ContainsKey("ProductId")){
		productId = productModel.GetData("ProductId").toByteArray();
	}

	if (productId.isEmpty()){
		errorMessage = QT_TR_NOOP("Unable to create product with an empty product-ID.");
		SendErrorMessage(0, errorMessage, "Product controller");

		return nullptr;
	}

	iprm::CIdParam idParam;
	idParam.SetId(productId);

	iprm::CParamsSet paramsSet1;
	paramsSet1.SetEditableParameter("ProductId", &idParam);

	iprm::CParamsSet filterParam;
	filterParam.SetEditableParameter("ObjectFilter", &paramsSet1);

	imtbase::ICollectionInfo::Ids collectionIds = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParam);
	if (!collectionIds.isEmpty()){
		QByteArray id = collectionIds[0];
		if (objectId != id){
			errorMessage = QT_TR_NOOP(QString("Product-ID: %1 already exists.")).arg(qPrintable(productId));

			return nullptr;
		}
	}

	productInfoPtr->SetProductId(productId);

	if (productModel.ContainsKey("ProductName")){
		name = productModel.GetData("ProductName").toString();
	}

	if (name.isEmpty()){
		errorMessage = QT_TR_NOOP("Unable to create product with an empty product name.");
		SendErrorMessage(0, errorMessage, "Product controller");

		return nullptr;
	}

	iprm::CIdParam nameParam;
	nameParam.SetId(name.toUtf8());

	iprm::CParamsSet paramsSet2;
	paramsSet2.SetEditableParameter("ProductName", &nameParam);

	iprm::CParamsSet filterParam2;
	filterParam2.SetEditableParameter("ObjectFilter", &paramsSet2);

	imtbase::ICollectionInfo::Ids collectionIds2 = m_objectCollectionCompPtr->GetElementIds(0, -1, &filterParam2);
	if (!collectionIds2.isEmpty()){
		QByteArray id = collectionIds2[0];
		if (objectId != id){
			errorMessage = QT_TR_NOOP(QString("Product Name: %1 already exists. Please rename.")).arg(qPrintable(name));

			return nullptr;
		}
	}

	productInfoPtr->SetName(name);

	if (productModel.ContainsKey("Description")){
		description = productModel.GetData("Description").toString();
	}

	QByteArray categoryId;
	if (productModel.ContainsKey("CategoryId")){
		categoryId = productModel.GetData("CategoryId").toByteArray();
	}

	productInfoPtr->SetCategoryId(categoryId);

	if (productModel.ContainsKey("Features")){
		QByteArray features = productModel.GetData("Features").toByteArray();
		if (!features.isEmpty()){
			QByteArrayList featureIds = features.split(';');

			productInfoPtr->SetFeatureIds(featureIds);
		}
	}

	return productInfoPtr.PopPtr();
}


imtbase::CTreeItemModel* CProductControllerComp::GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
		SendErrorMessage(0, errorMessage, "Product controller");

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr == nullptr){
		errorMessage = QT_TR_NOOP("Unable to get object. GQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "Product controller");

		return nullptr;
	}

	QByteArray objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		const imtlic::CProductInfo* productInfoPtr = dynamic_cast<const imtlic::CProductInfo*>(dataPtr.GetPtr());
		if (productInfoPtr != nullptr){
			istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
			imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

			dataModelPtr->SetData("Id", objectId);

			QByteArray productId = productInfoPtr->GetProductId();
			dataModelPtr->SetData("ProductId", productId);

			QString productName = productInfoPtr->GetName();
			dataModelPtr->SetData("ProductName", productName);

			dataModelPtr->SetData("Name", productName);

			QByteArray categoryId = productInfoPtr->GetCategoryId();
			dataModelPtr->SetData("CategoryId", categoryId);

			QByteArrayList featureIds = productInfoPtr->GetFeatureIds();
			dataModelPtr->SetData("Features", featureIds.join(';'));

			return rootModelPtr.PopPtr();
		}
	}

	errorMessage = QT_TR_NOOP(QString("Unable to get product by ID: %1.").arg(qPrintable(objectId)));
	SendErrorMessage(0, errorMessage, "Product controller");

	return nullptr;
}


imtbase::CTreeItemModel* CProductControllerComp::RenameObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
		SendErrorMessage(0, errorMessage, "Product controller");

		return nullptr;
	}

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr == nullptr){
		errorMessage = QT_TR_NOOP("Unable to get object. GQL input params is invalid.");
		SendErrorMessage(0, errorMessage, "CProductControllerComp");

		return nullptr;
	}

	QString oldName;

	QByteArray objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	QString newName = inputParamPtr->GetFieldArgumentValue("NewName").toByteArray();

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		imtlic::CProductInfo* productInfoPtr = dynamic_cast<imtlic::CProductInfo*>(dataPtr.GetPtr());
		if (productInfoPtr != nullptr){
			oldName = productInfoPtr->GetName();

			productInfoPtr->SetName(newName);
			productInfoPtr->SetProductId(newName.replace(QRegularExpression("/\s+/g"), "").toUtf8());

			if (!m_objectCollectionCompPtr->SetObjectData(objectId, *productInfoPtr)){
				errorMessage = QString("Error when trying rename product with ID %1").arg(qPrintable(objectId));
				SendErrorMessage(0, errorMessage, "CProductControllerComp");

				return nullptr;
			}
		}
	}

	SendInfoMessage(0, QString("The product %1 successfully renamed to %2").arg(oldName).arg(newName));

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	dataModelPtr->SetData("Id", objectId);
	dataModelPtr->SetData("Name", newName);

	return rootModelPtr.PopPtr();
}


} // namespace imtlicgql


