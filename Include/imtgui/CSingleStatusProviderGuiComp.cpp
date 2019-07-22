#include <imtgui/CSingleStatusProviderGuiComp.h>


namespace imtgui
{


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CSingleStatusProviderGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	const istd::IInformationProvider* informationProviderPtr = GetObjectPtr();
	Q_ASSERT(informationProviderPtr != nullptr);

	QIcon icon = GetStatusIcon(informationProviderPtr->GetInformationCategory());
	IconLabel->setPixmap(icon.pixmap(QSize(32, 32)));

	if (m_showStatusAttrPtr.IsValid() && *m_showStatusAttrPtr){
		SourceLabel->setText(informationProviderPtr->GetInformationSource());
	}

	if (m_showDescriptionAttrPtr.IsValid() && *m_showDescriptionAttrPtr){
		DescriptionLabel->setText(informationProviderPtr->GetInformationDescription());
	}
}


QIcon CSingleStatusProviderGuiComp::GetStatusIcon(istd::IInformationProvider::InformationCategory status)
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


