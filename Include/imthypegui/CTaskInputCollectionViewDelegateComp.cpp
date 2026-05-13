// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imthypegui/CTaskInputCollectionViewDelegateComp.h>


// Qt includes
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QInputDialog>

// ImtCore includes
#include <imtgui/CCollectionNumericItemDelegate.h>
#include <imtwidgets/CNoEditableItemDelegate.h>


namespace imthypegui
{


// reimplemented (ICollectionViewDelegate)

bool CTaskInputCollectionViewDelegateComp::GetSummaryInformation(
			const QByteArray& objectId,
			const QVector<QByteArray>& fieldIds,
			ObjectMetaInfo& objectMetaInfo) const
{
	if (m_collectionPtr == nullptr){
		return false;
	}
	imtbase::IObjectCollection::DataPtr objectPtr;
	if (m_collectionPtr->GetObjectData(objectId, objectPtr)){
		for (const QByteArray& informationId: fieldIds){
			SummaryInformation summaryInformation;
			summaryInformation.infoId = informationId;

			if (informationId == "Preview" && m_paramSetRepresentationControllerCompPtr.IsValid()){
				QString text;
				if (m_paramSetRepresentationControllerCompPtr->CreateSummaryRepresentation(*objectPtr, text)){
					text = text.replace(";", "\n");
					summaryInformation.sortValue = text;
					summaryInformation.text = text;
					summaryInformation.infoId = informationId;
					objectMetaInfo.append(summaryInformation);
				}

				continue;
			}

			ObjectMetaInfo baseInfo;
			if (BaseClass::GetSummaryInformation(objectId, { informationId }, baseInfo)) {
				summaryInformation.icon = baseInfo[0].icon;
				summaryInformation.sortValue = baseInfo[0].sortValue;
				summaryInformation.text = baseInfo[0].text;

				objectMetaInfo.append(summaryInformation);
			}
			else {
				I_CRITICAL();

				return false;
			}
		}
	}

	return true;
}


QAbstractItemDelegate* CTaskInputCollectionViewDelegateComp::GetColumnItemDelegate(const QByteArray& columnId) const
{
	if (columnId == "Name"){
		imtgui::CCollectionNumericItemDelegate* collectionNumericItemDelegatePtr = new imtgui::CCollectionNumericItemDelegate();

		collectionNumericItemDelegatePtr->SetMaximum(255);
		collectionNumericItemDelegatePtr->SetCollectionViewDelegate(this);
		collectionNumericItemDelegatePtr->SetColumnId(columnId);

		return collectionNumericItemDelegatePtr;
	}

	return new imtwidgets::CNoEditableItemDelegate();
}


void CTaskInputCollectionViewDelegateComp::OnColumnItemDelegateEditFinished(const QByteArray& objectId, const QByteArray& columnId, const QVariant& newValue) const
{
	if (columnId == "Name"){
		imtbase::ICollectionInfo::Ids ids = m_collectionPtr->GetElementIds();
		for (int i = 0; i < ids.count(); i++){
			if (m_collectionPtr->GetElementInfo(ids[i],imtbase::ICollectionInfo::EIT_NAME) == newValue){
				return;
			}
		}

		m_collectionPtr->SetElementName(objectId,newValue.toString());
	}
}


// protected methods

// reimplemented (imtbase::CObjectCollectionViewDelegate)

void CTaskInputCollectionViewDelegateComp::SetupSummaryInformation()
{
	m_summaryInformationTypes.ResetData();
	m_summaryInformationHeaders.clear();

	m_summaryInformationTypes.InsertItem("Name", tr("Acquisition-ID"), "");
	m_summaryInformationHeaders["Name"] = HeaderInfo(true);

	m_summaryInformationTypes.InsertItem("Preview", *m_previewColumnNameAttrPtr, "");
	m_summaryInformationHeaders["Preview"] = HeaderInfo(false);

	m_summaryInformationTypes.InsertItem("Description", tr("Description"), "");
	m_summaryInformationHeaders["Description"] = HeaderInfo(false, HeaderInfo::IF_FILTERABLE | HeaderInfo::IF_SORTABLE);
}


bool CTaskInputCollectionViewDelegateComp::IsEditorEnabled(const QByteArray& columnId) const
{
	if (columnId == "Name"){
		return false;
	}
	return true;
}


void CTaskInputCollectionViewDelegateComp::UpdateItemSelection(const imtbase::ICollectionInfo::Ids& selectedItems, const QByteArray& selectedTypeId)
{
	BaseClass::UpdateItemSelection(selectedItems, selectedTypeId);

	m_editContentsCommand.setEnabled(selectedItems.count() == 1);
}


QByteArray CTaskInputCollectionViewDelegateComp::CreateNewObject(
			const QByteArray& typeId,
			const QString& /*objectName*/,
			const QString& description,
			const istd::IChangeable* defaultDataPtr) const
{
	QVector<int> names;

	imtbase::ICollectionInfo::Ids ids = m_collectionPtr->GetElementIds();
	for (const QByteArray& id : ids){
		bool ok;
		int intId = m_collectionPtr->GetElementInfo(id, imtbase::ICollectionInfo::EIT_NAME).toString().toInt(&ok);
		if (ok){
			names.append(intId);
		}
	}

	int maxId = -1;
	if (!names.isEmpty()){
		std::sort(names.begin(), names.end());

		maxId = names.last();
	}
	maxId++;

	if (maxId > 255){
		do {
			maxId--;
		} while (names.contains(maxId));
	}

	QByteArray objectId;
	if (maxId >= 0){
		objectId = BaseClass::CreateNewObject(typeId, QString::number(maxId), description, defaultDataPtr);
	}

	return objectId;
}


QString CTaskInputCollectionViewDelegateComp::RenameObject(const QByteArray& objectId, const QString& newName) const
{
	if (objectId.isEmpty()){
		return QString();
	}

	if (m_collectionPtr != nullptr){
		QString name = newName;

		if (name.isEmpty()){
			int oldName = m_collectionPtr->GetElementInfo(objectId, imtbase::ICollectionInfo::EIT_NAME).toString().toInt();
			bool ok;
			name = QString::number(QInputDialog::getInt(nullptr, tr("Enter new object name"), tr("Name"), oldName, 0, 255, 1, &ok));
			if (!ok){
				return QString();
			}
		}

		bool ok;
		int value = name.toInt(&ok);
		if (!ok || value <= 0 || value > 255){
			QMessageBox::critical(nullptr, tr("Error"), tr("Incorrect object name. Object name must be a number between 0 and 255"));

			return QString();
		}

		imtbase::ICollectionInfo::Ids ids = m_collectionPtr->GetElementIds();
		for (int i = 0; i < ids.count(); i++){
			if (m_collectionPtr->GetElementInfo(ids[i], imtbase::ICollectionInfo::EIT_NAME) == name){
				return QString();
			}
		}

		return BaseClass::RenameObject(objectId, name);
	}

	return QString();
}


bool CTaskInputCollectionViewDelegateComp::IsCommandSupported(int commandId) const
{
	if (commandId == CI_IMPORT || commandId == CI_EXPORT){
		return false;
	}

	return BaseClass::IsCommandSupported(commandId);
}


} // namespace imthypegui


