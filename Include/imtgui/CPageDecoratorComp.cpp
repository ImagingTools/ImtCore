// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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


// reimplemented (iqtgui::CGuiComponentBase)

void CPageDecoratorComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	if (m_pageGuiCompPtr.IsValid()){
		m_pageGuiCompPtr->CreateGui(Content);
	}

	if (m_pageVisualStatusCompPtr.IsValid()){
		m_pageVisualStatusObserver.RegisterObject(m_pageVisualStatusCompPtr.GetPtr(), &CPageDecoratorComp::OnVisualStatusChanged);
	}

	if (m_pageCommandsProviderCompPtr.IsValid()){
		m_pageCommandsToolBar.RegisterCommands(MiddleCommandsFrame, m_pageCommandsProviderCompPtr.GetPtr());
	}
}


void CPageDecoratorComp::OnGuiDestroyed()
{
	if (m_pageGuiCompPtr.IsValid() && m_pageGuiCompPtr->IsGuiCreated()){
		m_pageGuiCompPtr->DestroyGui();
	}

	m_pageCommandsToolBar.UnregisterCommands();

	BaseClass::OnGuiDestroyed();
}


void CPageDecoratorComp::OnGuiDesignChanged()
{
	if (m_pageVisualStatusCompPtr.IsValid()){
		PageIcon->setPixmap(m_pageVisualStatusCompPtr->GetStatusIcon().pixmap(32, 32));

		PageTitle->setText(m_pageVisualStatusCompPtr->GetStatusText());
	}

	BaseClass::OnGuiDesignChanged();
}


} // namespace imtgui


