// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtWidgets/QToolButton>

// ACF includes
#include <iprm/IOptionsList.h>
#include <iprm/ISelectionParam.h>
#include <iqtgui/IIconProvider.h>
#include <iqtgui/TDesignerGuiObserverCompBase.h>

// ImtCore includes
#include <imtbase/TModelUpdateBinder.h>
#include <GeneratedFiles/imtgui/ui_CButtonBasedSelectionGuiComp.h>


namespace imtgui
{


class CButtonBasedSelectionGuiComp
	: public iqtgui::TDesignerGuiObserverCompBase<Ui::CButtonBasedSelectionGuiComp, iprm::ISelectionParam>
{
	Q_OBJECT
public:
	typedef iqtgui::TDesignerGuiObserverCompBase<Ui::CButtonBasedSelectionGuiComp, iprm::ISelectionParam> BaseClass;

	enum Orientation
	{
		O_HORIZONTAL = 0,
		O_VERTICAL
	};

	I_BEGIN_COMPONENT(CButtonBasedSelectionGuiComp);
		I_ASSIGN(m_iconProviderCompPtr, "IconProvider", "Icons for buttons", false, "IconProvider");
		I_ASSIGN(m_orientationAttrPtr, "Orientation", "Orientation of the toolbar", true, 0);
		I_ASSIGN(m_hideTheOnlyButtonAttrPtr, "HideTheOnlyButton", "Hide button if there is only one option", true, false);
		I_ASSIGN(m_spacingAttrPtr, "Spacing", "Spacing between consecutive buttons", true, 0);
		I_ASSIGN(m_iconSizeAttrPtr, "IconSize", "Icon size", true, 16);
		I_ASSIGN(m_buttonStyleAttrPtr, "ButtonStyle", "See Qt::ToolButtonStyle", true, 0);
	I_END_COMPONENT;

	CButtonBasedSelectionGuiComp();

protected:
	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;
	virtual void OnGuiModelAttached() override;
	virtual void OnGuiModelDetached() override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiDesignChanged() override;

private Q_SLOTS:
	void OnButtonClicked(bool checked);

private:
	void OnOptionListUpdate(const istd::IChangeable::ChangeSet& changeSet, const iprm::IOptionsList* objectPtr);
	void OnIconsUpdate(const istd::IChangeable::ChangeSet& changeSet, const iqtgui::IIconProvider* objectPtr);

private:
	I_REF(iqtgui::IIconProvider, m_iconProviderCompPtr);
	I_ATTR(int, m_orientationAttrPtr);
	I_ATTR(bool, m_hideTheOnlyButtonAttrPtr);
	I_ATTR(int, m_spacingAttrPtr);
	I_ATTR(int, m_iconSizeAttrPtr);
	I_ATTR(int, m_buttonStyleAttrPtr);

	imtbase::TModelUpdateBinder<iprm::IOptionsList, CButtonBasedSelectionGuiComp> m_optionListObserver;
	imtbase::TModelUpdateBinder<iqtgui::IIconProvider, CButtonBasedSelectionGuiComp> m_iconObserver;
};


} // namespace imtgui
