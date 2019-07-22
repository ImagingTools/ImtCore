#include <imtgui/CMultiStatusProviderGuiComp.h>


namespace imtgui
{


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CMultiStatusProviderGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtbase::IMultiStatusProvider* objectPtr = GetObjectPtr();
	Q_ASSERT(objectPtr != nullptr);

	UpdateCommonStatusGui(*objectPtr);
	UpdateSubStatusesGui(*objectPtr);
}


// reimplemented (iqtgui::CGuiComponentBase)

void CMultiStatusProviderGuiComp::UpdateCommonStatusGui(imtbase::IMultiStatusProvider& object)
{
	QString statusName = object.GetInformationDescription();
	QIcon statusIcon = GetStatusIcon(object.GetInformationCategory());

	CommonStatusIcon->setPixmap(statusIcon.pixmap(QSize(32, 32)));
	CommonStatusText->setText(statusName);
}


void CMultiStatusProviderGuiComp::UpdateSubStatusesGui(imtbase::IMultiStatusProvider& object)
{
	const iprm::IOptionsList& statusProviderInfo = object.GetStatusInfoList();

	int statusesCount = statusProviderInfo.GetOptionsCount();
	if (statusesCount <= 0){
		return;
	}

	SubStatusesTable->setRowCount(statusesCount);

	for (int i = 0; i < statusesCount; ++i){
		QByteArray statusInfoId = statusProviderInfo.GetOptionId(i);

		const istd::IInformationProvider* statusPtr = object.GetStatusInfo(statusInfoId);
		Q_ASSERT(statusPtr != nullptr);

		QString statusText = statusPtr->GetInformationDescription();
		QString statusDesc = statusPtr->GetInformationSource();
		QIcon statusIcon = GetStatusIcon(statusPtr->GetInformationCategory());

		SubStatusesTable->setItem(i, 0, new QTableWidgetItem(statusIcon, statusText));
		SubStatusesTable->setItem(i, 1, new QTableWidgetItem(statusDesc));
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
