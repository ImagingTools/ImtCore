// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CTableViewParamRepresentationControllerComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtqml/IPageGuiElementModel.h>
#include <imtbase/ITableViewParam.h>


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
		imtbase::CTreeItemModel& representation,
		const iprm::IParamsSet* /*paramsPtr*/) const
{
	Q_ASSERT(IsModelSupported(dataModel));

	const imtbase::ITableViewParam* tableViewParamPtr = dynamic_cast<const imtbase::ITableViewParam*>(&dataModel);
	if (tableViewParamPtr == nullptr){
		return false;
	}

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

	for (const QByteArray& headerId : headerList){
		imtbase::ITableViewParam::HeaderInfo headerInfo = tableViewParamPtr->GetHeaderInfo(headerId);

		int index = representation.InsertNewItem();
		representation.SetData("HeaderId", headerInfo.headerId, index);
		representation.SetData("Size", headerInfo.size, index);
		representation.SetData("Visible", headerInfo.visible, index);
		representation.SetData("Order", headerInfo.order, index);
	}

	return true;
}


bool CTableViewParamRepresentationControllerComp::GetDataModelFromRepresentation(
		const imtbase::CTreeItemModel& representation,
		istd::IChangeable& dataModel) const
{
	Q_ASSERT(IsModelSupported(dataModel));

	imtbase::ITableViewParam* tableViewParamPtr = dynamic_cast<imtbase::ITableViewParam*>(&dataModel);
	if (tableViewParamPtr == nullptr){
		return false;
	}

	for (int i = 0; i < representation.GetItemsCount(); i++){
		imtbase::ITableViewParam::HeaderInfo headerInfo;

		if (representation.ContainsKey("HeaderId", i)){
			QByteArray headerId = representation.GetData("HeaderId", i).toByteArray();
			headerInfo.headerId = headerId;
		}

		if (representation.ContainsKey("Size", i)){
			double size = representation.GetData("Size", i).toDouble();
			headerInfo.size = size;
		}

		if (representation.ContainsKey("Visible", i)){
			bool visible = representation.GetData("Visible", i).toBool();
			headerInfo.visible = visible;
		}

		if (representation.ContainsKey("Order", i)){
			int order = representation.GetData("Order", i).toInt();
			headerInfo.order = order;
		}

		tableViewParamPtr->SetHeaderInfo(headerInfo.headerId, headerInfo);
	}

	return true;
}


} // namespace imtserverapp


