// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtWidgets/QToolBar>

// ACF includes
#include <ibase/ICommandsProvider.h>
#include <iqtgui/TDesignerGuiCompBase.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>
#include <imtgui/CCommandToolBar.h>
#include <GeneratedFiles/imtgui/ui_CPageDecoratorComp.h>


namespace imtgui
{


class CPageDecoratorComp: public iqtgui::TDesignerGuiCompBase<Ui::CPageDecoratorComp>
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiCompBase<Ui::CPageDecoratorComp> BaseClass;

	I_BEGIN_COMPONENT(CPageDecoratorComp);
		I_ASSIGN(m_pageGuiCompPtr, "PageGui", "Page content UI", true, "PageGui");
		I_ASSIGN(m_pageVisualStatusCompPtr, "PageVisualStatus", "Visual status of the page", false, "PageVisualStatus");
		I_ASSIGN(m_pageCommandsProviderCompPtr, "PageCommands", "Page commands", false, "PageCommands");
	I_END_COMPONENT;

	CPageDecoratorComp();

protected:
	void OnVisualStatusChanged(const istd::IChangeable::ChangeSet& changeSet, const iqtgui::IVisualStatus* visualStatusPtr);

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;
	virtual void OnGuiDesignChanged() override;

private:
	I_REF(iqtgui::IGuiObject, m_pageGuiCompPtr);
	I_REF(iqtgui::IVisualStatus, m_pageVisualStatusCompPtr);
	I_REF(ibase::ICommandsProvider, m_pageCommandsProviderCompPtr);

	imtbase::TModelUpdateBinder<iqtgui::IVisualStatus, CPageDecoratorComp> m_pageVisualStatusObserver;
	imtbase::TModelUpdateBinder<ibase::ICommandsProvider, CPageDecoratorComp> m_pageCommandsObserver;

	imtgui::CCommandToolBar m_pageCommandsToolBar;
};


} // namespace imtgui


