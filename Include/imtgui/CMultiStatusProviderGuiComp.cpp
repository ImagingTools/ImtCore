#include <imtgui/CMultiStatusProviderGuiComp.h>


namespace imtgui
{


// protected methods

// reimplemented (iqtgui::CGuiComponentBase)

void CMultiStatusProviderGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	SubStatusesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}


void CMultiStatusProviderGuiComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	if (!IsUpdateBlocked()){
		UpdateBlocker blockUpdate(this);

		UpdateGui(istd::IChangeable::GetAnyChange());
	}
}

void CMultiStatusProviderGuiComp::OnGuiDesignChanged()
{
	BaseClass::OnGuiDesignChanged();

	m_stateUnknownIcon = GetIcon(":/Icons/StateUnknown");
	m_stateOkIcon = GetIcon(":/Icons/StateOk");
	m_stateWarningIcon = GetIcon(":/Icons/StateWarning");
	m_stateInvalidIcon = GetIcon(":/Icons/StateInvalid");
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CMultiStatusProviderGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	const imtbase::IMultiStatusProvider* multiStatusProviderPtr = GetObjectPtr();
	Q_ASSERT(multiStatusProviderPtr != nullptr);

	UpdateStatusesGui(*multiStatusProviderPtr);

	const istd::IInformationProvider* informationProviderPtr = dynamic_cast<const istd::IInformationProvider*>(multiStatusProviderPtr);
	if (informationProviderPtr){
		UpdateCommonStatusGui(*informationProviderPtr);
	}
}


void CMultiStatusProviderGuiComp::UpdateCommonStatusGui(const istd::IInformationProvider& status)
{
	QString statusText = status.GetInformationDescription();
	QIcon statusIcon = GetStatusIcon(status.GetInformationCategory());

	CommonStatusIcon->setPixmap(statusIcon.pixmap(QSize(32, 32)));
	CommonStatusText->setText(statusText);
}


void CMultiStatusProviderGuiComp::UpdateStatusesGui(const imtbase::IMultiStatusProvider& statuses)
{
	const imtbase::ICollectionInfo& statusProviderInfo = statuses.GetStatusList();

	imtbase::ICollectionInfo::Ids statusIds = statusProviderInfo.GetElementIds();

	SubStatusesTable->setRowCount(statusIds.count());

	for (int i = 0; i < statusIds.count(); ++i){
		QByteArray statusInfoId = statusIds[i];

		const istd::IInformationProvider* statusPtr = statuses.GetStatus(statusInfoId);
		Q_ASSERT(statusPtr != nullptr);

		QString statusText = statusPtr->GetInformationDescription();
		QString statusSource = statusPtr->GetInformationSource();
		QIcon statusIcon = GetStatusIcon(statusPtr->GetInformationCategory());

		SubStatusesTable->setItem(i, 0, new QTableWidgetItem(statusIcon, statusSource));
		SubStatusesTable->setItem(i, 1, new QTableWidgetItem(statusText));
	}
}


QIcon CMultiStatusProviderGuiComp::GetStatusIcon(istd::IInformationProvider::InformationCategory status)
{

	switch (status){
		case istd::IInformationProvider::IC_INFO:
			return m_stateOkIcon;
		case istd::IInformationProvider::IC_WARNING:
			return m_stateWarningIcon;
		case istd::IInformationProvider::IC_ERROR:
		case istd::IInformationProvider::IC_CRITICAL:
			return m_stateInvalidIcon;
		default:
			return m_stateUnknownIcon;
	}
}


} // namespace imtgui


