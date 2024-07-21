#include <imtbase/CTableViewParamRepresentationControllerComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>
#include <imtqml/IPageGuiElementModel.h>
#include <imtgui/IGuiElementContainer.h>
#include <imtbase/ITableViewParam.h>


namespace imtbase
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
	if (guiElementPtr != nullptr) {
		return true;
	}

	return false;
}


bool CTableViewParamRepresentationControllerComp::GetRepresentationFromDataModel(
			const istd::IChangeable& dataModel,
			imtbase::CTreeItemModel& representation,
			const iprm::IParamsSet* paramsPtr) const
{
	Q_ASSERT(IsModelSupported(dataModel));

	const imtbase::ITableViewParam* tableViewParamPtr = dynamic_cast<const imtbase::ITableViewParam*>(&dataModel);
	if (tableViewParamPtr == nullptr) {
		return false;
	}

	QByteArrayList headerList = tableViewParamPtr->GetHeaderIds();
	for (const QByteArray& headerId : headerList){
		imtbase::ITableViewParam::HeaderInfo headerInfo = tableViewParamPtr->GetHeaderInfo(headerId);

		int index = representation.InsertNewItem();
		representation.SetData("HeaderId", headerInfo.headerId, index);
		representation.SetData("Size", headerInfo.size, index);
		representation.SetData("Visible", headerInfo.visible, index);
	}

	return true;
}


bool CTableViewParamRepresentationControllerComp::GetDataModelFromRepresentation(
			const imtbase::CTreeItemModel& representation,
			istd::IChangeable& dataModel) const
{
	Q_ASSERT(IsModelSupported(dataModel));

	imtbase::ITableViewParam* tableViewParamPtr = dynamic_cast<imtbase::ITableViewParam*>(&dataModel);
	if (tableViewParamPtr == nullptr) {
		return false;
	}

	for (int i = 0; i < representation.GetItemsCount(); i++){
		imtbase::ITableViewParam::HeaderInfo headerInfo;

		if (representation.ContainsKey("HeaderId", i)){
			QByteArray headerId = representation.GetData("HeaderId", i).toByteArray();
			headerInfo.headerId = headerId;
		}

		if (representation.ContainsKey("Size", i)){
			int size = representation.GetData("Size", i).toInt();
			headerInfo.size = size;
		}

		if (representation.ContainsKey("Visible", i)){
			bool visible = representation.GetData("Visible", i).toBool();
			headerInfo.visible = visible;
		}

		tableViewParamPtr->SetHeaderInfo(headerInfo.headerId, headerInfo);
	}

	return true;
}


} // namespace imtbase


