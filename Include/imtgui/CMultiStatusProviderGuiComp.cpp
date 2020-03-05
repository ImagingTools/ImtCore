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
	static QIcon stateUnknownIcon(":/Icons/StateUnknown");
	static QIcon stateOkIcon(":/Icons/StateOk");
	static QIcon stateWarningIcon(":/Icons/StateWarning");
	static QIcon stateInvalidIcon(":/Icons/StateInvalid");

	switch (status){
		case istd::IInformationProvider::IC_INFO:
			return stateOkIcon;
		case istd::IInformationProvider::IC_WARNING:
			return stateWarningIcon;
		case istd::IInformationProvider::IC_ERROR:
		case istd::IInformationProvider::IC_CRITICAL:
			return stateInvalidIcon;
		default:
			return stateUnknownIcon;
	}
}


} // namespace imtgui


