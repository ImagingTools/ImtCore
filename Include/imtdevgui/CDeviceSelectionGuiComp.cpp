// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtdevgui/CDeviceSelectionGuiComp.h>


// ImtCore includes
#include <imtbase/imtbase.h>
#include <iprm/IOptionsList.h>


namespace imtdevgui
{


// public methods

CDeviceSelectionGuiComp::CDeviceSelectionGuiComp()
	: m_deviceListObserver(*this),
	  m_deviceStateObserver(*this)
{
	connect(
		this,
		&CDeviceSelectionGuiComp::EmitAutoSelect,
		this,
		&CDeviceSelectionGuiComp::OnAutoSelect,
		Qt::QueuedConnection);
}


// protected methods

void CDeviceSelectionGuiComp::OnDeviceListUpdated(
	const istd::IChangeable::ChangeSet& /*changeSet*/, const imtbase::ICollectionInfo* objectPtr)
{
	static QIcon emptyIcon("");

	QByteArray selectedId;
	QSet<QByteArray> selectedIds = GetObservedObject()->GetSelectedIds();
	if (!selectedIds.isEmpty()){
		selectedId = selectedIds.values().first();
	}

	DeviceListComboBox->blockSignals(true);
	DeviceListComboBox->clear();

	imtbase::ICollectionInfo::Ids deviceIds = objectPtr->GetElementIds();
	int currentIndex = -1;
	int count = imtbase::narrow_cast<int>(deviceIds.count());
	for (int i = 0; i < count; ++i) {
		DeviceListComboBox->insertItem(i, emptyIcon,
			objectPtr->GetElementInfo(deviceIds[i], imtbase::ICollectionInfo::EIT_NAME).toString());
		DeviceListComboBox->setItemData(i, deviceIds[i]);

		if (deviceIds[i] == selectedId){
			currentIndex = i;
		}
	}

	DeviceListComboBox->setCurrentIndex(currentIndex);
	DeviceListComboBox->blockSignals(false);

	if (deviceIds.isEmpty()){
		DoUpdateModel();
	}

	if (currentIndex < 0) {
		Q_EMIT EmitAutoSelect();
	}

	if (m_measurementDeviceStateCompPtr.IsValid()) {
		OnDeviceStateUpdated(istd::IChangeable::ChangeSet(), nullptr);
	}
}


void CDeviceSelectionGuiComp::OnDeviceStateUpdated(
	const istd::IChangeable::ChangeSet& /*changeSet*/, const imtdev::IDeviceStateProvider* /*objectPtr*/)
{
	UpdateDeviceStateIcons();
}


void CDeviceSelectionGuiComp::UpdateDeviceStateIcons()
{
	if (m_measurementDeviceStateCompPtr.IsValid()) {
		for (int i = 0; i < DeviceListComboBox->count(); ++i) {
			bool isConnected =
				m_measurementDeviceStateCompPtr->GetDeviceState(DeviceListComboBox->itemData(i).toByteArray()) != imtdev::IDeviceStateProvider::DS_NONE;
			DeviceListComboBox->setItemIcon(i, isConnected ? m_iconConnected : m_iconNotConnected);
		}
	}
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CDeviceSelectionGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	const imtbase::ICollectionInfo* deviceListPtr = GetObservedObject()->GetSelectionConstraints();
	Q_ASSERT(deviceListPtr != nullptr);

	QByteArray selectedId;
	QSet<QByteArray> selectedIds = GetObservedObject()->GetSelectedIds();
	if (!selectedIds.isEmpty()){
		selectedId = selectedIds.values().first();
	}

	for (int i = 0; i < DeviceListComboBox->count(); i++) {
		if (DeviceListComboBox->itemData(i) == selectedId) {
			DeviceListComboBox->setCurrentIndex(i);

			break;
		}
	}
}


// reimplemented (imod::IModelEditor)

void CDeviceSelectionGuiComp::UpdateModel() const
{
	imtbase::ISelection* selectionPtr = GetObservedObject();
	Q_ASSERT(selectionPtr != nullptr);
	if (selectionPtr != nullptr) {
		const imtbase::ICollectionInfo* deviceListPtr = selectionPtr->GetSelectionConstraints();
		Q_ASSERT(deviceListPtr != nullptr);
		if (deviceListPtr != nullptr) {
			int currentIndex = DeviceListComboBox->currentIndex();
			QByteArray id = DeviceListComboBox->currentData().toByteArray();

			if (currentIndex >= 0 && currentIndex < deviceListPtr->GetElementIds().count()) {
				selectionPtr->SetSelectedIds({id});
			}
			else if (currentIndex < 0) {
				selectionPtr->SetSelectedIds({});
			}
			else {
				Q_ASSERT(false);
			}
		}
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CDeviceSelectionGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	imtbase::ISelection* objectPtr = GetObservedObject();
	Q_ASSERT(objectPtr != nullptr);
	if (objectPtr != nullptr){
		Q_ASSERT(objectPtr->GetSelectionMode() == imtbase::ISelection::SM_SINGLE);
	}

	QLayout* layoutPtr = GetQtWidget()->layout();
	if (layoutPtr != nullptr) {
		delete layoutPtr;

		if (*m_labelPositionAttrPtr == LP_LEFT) {
			layoutPtr = new QHBoxLayout();
			layoutPtr->setSpacing(6);
		}
		else {
			layoutPtr = new QVBoxLayout();
			layoutPtr->setSpacing(14);
		}

		GetQtWidget()->setLayout(layoutPtr);
		GetQtWidget()->layout()->setContentsMargins(0, 0, 0, 0);
		GetQtWidget()->layout()->addWidget(label);
		GetQtWidget()->layout()->addWidget(DeviceListComboBox);
		m_deviceListElider.RegisterElideObject(DeviceListComboBox);
	}

	label->setText(*m_labelTextAttrPtr);

	connect(
		DeviceListComboBox,
		qOverload<int>(&QComboBox::currentIndexChanged),
		this,
		&CDeviceSelectionGuiComp::OnDataChanged);

	if (objectPtr != nullptr) {
		const imtbase::ICollectionInfo* deviceListPtr = objectPtr->GetSelectionConstraints();
		m_deviceListObserver.RegisterObject(deviceListPtr, &CDeviceSelectionGuiComp::OnDeviceListUpdated);
	}

	if (m_measurementDeviceStateCompPtr.IsValid()) {
		m_deviceStateObserver.RegisterObject(
			m_measurementDeviceStateCompPtr.GetPtr(), &CDeviceSelectionGuiComp::OnDeviceStateUpdated);
	}
}


void CDeviceSelectionGuiComp::OnGuiDestroyed()
{
	m_deviceStateObserver.UnregisterAllObjects();
	m_deviceListObserver.UnregisterAllObjects();

	BaseClass::OnGuiDestroyed();
}


void CDeviceSelectionGuiComp::OnGuiDesignChanged()
{
	BaseClass::OnGuiDesignChanged();

	m_iconConnected = GetIcon(":/Icons/StateOk");
	m_iconNotConnected = GetIcon(":/Icons/StateUnknown");

	UpdateDeviceStateIcons();
}


// protected Q_SLOTS

void CDeviceSelectionGuiComp::OnAutoSelect()
{
	if (DeviceListComboBox->currentIndex() < 0 && DeviceListComboBox->count() > 0) {
		DeviceListComboBox->setCurrentIndex(0);
	}
}


void CDeviceSelectionGuiComp::OnDataChanged()
{
	BaseClass::DoUpdateModel();
}


} // namespace imtdevgui
