#include <imtgui/CSingleStatusProviderGuiComp.h>


namespace imtgui
{


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CSingleStatusProviderGuiComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	const istd::IInformationProvider* informationProviderPtr = GetObjectPtr();
	if (informationProviderPtr != nullptr){
		QWidget* widgetPtr = GetWidget();
		Q_ASSERT(widgetPtr != nullptr);

		widgetPtr->setToolTip(informationProviderPtr->GetInformationDescription());
	}
}


void CSingleStatusProviderGuiComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	const istd::IInformationProvider* informationProviderPtr = GetObjectPtr();
	Q_ASSERT(informationProviderPtr != nullptr);

	QIcon icon = GetStatusIcon(informationProviderPtr->GetInformationCategory());
	IconLabel->setPixmap(icon.pixmap(QSize(16, 16)));

	if (m_showStatusAttrPtr.IsValid() && *m_showStatusAttrPtr){
		SourceLabel->setText(informationProviderPtr->GetInformationSource());
	}
	else{
		SourceLabel->hide();
	}

	if (m_showDescriptionAttrPtr.IsValid() && *m_showDescriptionAttrPtr){
		DescriptionLabel->setText(informationProviderPtr->GetInformationDescription());
	}
	else{
		DescriptionLabel->hide();  
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CSingleStatusProviderGuiComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	if (!IsUpdateBlocked()){
		UpdateBlocker blockUpdate(this);

		UpdateGui(istd::IChangeable::GetAnyChange());
	}
}


// private static methods

QIcon CSingleStatusProviderGuiComp::GetStatusIcon(istd::IInformationProvider::InformationCategory status)
{
	static QIcon stateUnknownIcon(":/Icons/StateUnknown2");
	static QIcon stateOkIcon(":/Icons/StateOk2");
	static QIcon stateWarningIcon(":/Icons/StateWarning2");
	static QIcon stateInvalidIcon(":/Icons/StateInvalid2");

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


