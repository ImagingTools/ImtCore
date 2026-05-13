// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CTableViewParamRepresentationControllerComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtqml/IPageGuiElementModel.h>
#include <imtbase/ITableViewParam.h>

// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>


namespace imtserverapp
{


// protected methods

// reimplemented (imtbase::IRepresentationController)

QByteArray CTableViewParamRepresentationControllerComp::GetModelId() const
{
	if (m_modelIdAttrPtr.IsValid()){
		return *m_modelIdAttrPtr;
	}

	return QByteArray();
}


bool CTableViewParamRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtbase::ITableViewParam* guiElementPtr = dynamic_cast<const imtbase::ITableViewParam*>(&dataModel);

	return guiElementPtr != nullptr;
}


bool CTableViewParamRepresentationControllerComp::GetRepresentationFromDataModel(
		const istd::IChangeable& dataModel,
		QJsonObject& representation,
		const iprm::IParamsSet* /*paramsPtr*/) const
{
	Q_ASSERT(IsModelSupported(dataModel));

	const imtbase::ITableViewParam* tableViewParamPtr = dynamic_cast<const imtbase::ITableViewParam*>(&dataModel);
	if (tableViewParamPtr == nullptr){
		return false;
	}

	representation = QJsonObject();
	QByteArrayList headerList = tableViewParamPtr->GetHeaderIds();

	// sort headerList by order
	for (int i = 0; i < headerList.size(); i++){
		bool flag = true;
		for (int j = 0; j < headerList.size() - (i + 1); j++){
			imtbase::ITableViewParam::HeaderInfo headerInfo1 = tableViewParamPtr->GetHeaderInfo(headerList[j]);
			imtbase::ITableViewParam::HeaderInfo headerInfo2 = tableViewParamPtr->GetHeaderInfo(headerList[j + 1]);
			if (headerInfo1.order > headerInfo2.order){
				flag = false;

				headerList.swapItemsAt(j, j + 1);
			}
		}

		if (flag){
			break;
		}
	}

	QJsonArray itemsArray;
	for (const QByteArray& headerId : headerList){
		imtbase::ITableViewParam::HeaderInfo headerInfo = tableViewParamPtr->GetHeaderInfo(headerId);
		QJsonObject itemObj;
		itemObj.insert(QStringLiteral("HeaderId"), QString::fromUtf8(headerInfo.headerId));
		itemObj.insert(QStringLiteral("Size"), headerInfo.size);
		itemObj.insert(QStringLiteral("Visible"), headerInfo.visible);
		itemObj.insert(QStringLiteral("Order"), headerInfo.order);
		itemsArray.append(itemObj);
	}

	representation.insert(QStringLiteral("items"), itemsArray);
	return true;
}


bool CTableViewParamRepresentationControllerComp::GetDataModelFromRepresentation(
		const QJsonObject& representation,
		istd::IChangeable& dataModel) const
{
	Q_ASSERT(IsModelSupported(dataModel));

	imtbase::ITableViewParam* tableViewParamPtr = dynamic_cast<imtbase::ITableViewParam*>(&dataModel);
	if (tableViewParamPtr == nullptr){
		return false;
	}

	QJsonArray itemsArray;
	if (representation.contains(QStringLiteral("items")) && representation.value(QStringLiteral("items")).isArray()){
		itemsArray = representation.value(QStringLiteral("items")).toArray();
	}

	for (int i = 0; i < itemsArray.size(); i++){
		QJsonObject itemObj = itemsArray.at(i).toObject();
		imtbase::ITableViewParam::HeaderInfo headerInfo;

		if (itemObj.contains(QStringLiteral("HeaderId"))){
			headerInfo.headerId = itemObj.value(QStringLiteral("HeaderId")).toVariant().toByteArray();
		}

		if (itemObj.contains(QStringLiteral("Size"))){
			headerInfo.size = itemObj.value(QStringLiteral("Size")).toDouble();
		}

		if (itemObj.contains(QStringLiteral("Visible"))){
			headerInfo.visible = itemObj.value(QStringLiteral("Visible")).toBool();
		}

		if (itemObj.contains(QStringLiteral("Order"))){
			headerInfo.order = itemObj.value(QStringLiteral("Order")).toInt();
		}

		tableViewParamPtr->SetHeaderInfo(headerInfo.headerId, headerInfo);
	}

	return true;
}


} // namespace imtserverapp
