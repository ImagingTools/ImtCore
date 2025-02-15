#include <imtgui/CPageDecoratorComp.h>


// ACF includes
#include <iwidgets/iwidgets.h>
#include <iqtgui/CCommandTools.h>


namespace imtgui
{


CPageDecoratorComp::CPageDecoratorComp()
	:m_pageVisualStatusObserver(*this),
	m_pageCommandsObserver(*this)
{
}


// protected methods

void CPageDecoratorComp::OnVisualStatusChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const iqtgui::IVisualStatus* visualStatusPtr)
{
	PageIcon->setPixmap(visualStatusPtr->GetStatusIcon().pixmap(32, 32));

	PageTitle->setText(visualStatusPtr->GetStatusText());
}


void CPageDecoratorComp::OnPageCommandsChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const ibase::ICommandsProvider* /*commandsPtr*/)
{
	UpdateCommands();
}


void CPageDecoratorComp::UpdateCommands()
{
	if (m_pageCommandsProviderCompPtr.IsValid()) {
		const iqtgui::CHierarchicalCommand* commandPtr = dynamic_cast<const iqtgui::CHierarchicalCommand*>(m_pageCommandsProviderCompPtr->GetCommands());
		if (commandPtr != nullptr) {
			if (m_mainCommandsToolBar == nullptr){
				m_mainCommandsToolBar = new QToolBar(MiddleCommandsFrame);
				m_mainCommandsToolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
				m_mainCommandsToolBar->setIconSize(QSize(24, 24));

				QLayout* layoutPtr = MiddleCommandsFrame->layout();
				if (layoutPtr != nullptr){
					layoutPtr->addWidget(m_mainCommandsToolBar);
				}
			}

			m_mainCommandsToolBar->clear();

			iqtgui::CCommandTools::SetupToolbar(*commandPtr, *m_mainCommandsToolBar);
		}
	}
}


// reimplemented (iqtgui::CGuiComponentBase)

void CPageDecoratorComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	if (m_pageGuiCompPtr.IsValid()) {
		m_pageGuiCompPtr->CreateGui(Content);
	}

	if (m_pageVisualStatusCompPtr.IsValid()) {
		m_pageVisualStatusObserver.RegisterObject(m_pageVisualStatusCompPtr.GetPtr(), &CPageDecoratorComp::OnVisualStatusChanged);
	}

	if (m_pageCommandsProviderCompPtr.IsValid()) {
		m_pageCommandsObserver.RegisterObject(m_pageCommandsProviderCompPtr.GetPtr(), &CPageDecoratorComp::OnPageCommandsChanged);
	}
}


void CPageDecoratorComp::OnGuiDestroyed()
{
	if (m_pageGuiCompPtr.IsValid() && m_pageGuiCompPtr->IsGuiCreated()){
		m_pageGuiCompPtr->DestroyGui();
	}

	BaseClass::OnGuiDestroyed();
}


void CPageDecoratorComp::OnGuiDesignChanged()
{
	if (m_pageVisualStatusCompPtr.IsValid()) {
		PageIcon->setPixmap(m_pageVisualStatusCompPtr->GetStatusIcon().pixmap(32, 32));

		PageTitle->setText(m_pageVisualStatusCompPtr->GetStatusText());
	}

	BaseClass::OnGuiDesignChanged();
}


// private slots


} // namespace imtgui


