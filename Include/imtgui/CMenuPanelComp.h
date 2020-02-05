#pragma once


// ACF includes
#include <ibase/TModelObserverCompWrap.h>
#include <iqtgui/TGuiObserverWrap.h>
#include <iqtgui/TGuiComponentBase.h>
#include <iprm/ISelectionParam.h>
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtwidgets/CMenuPanel.h>


namespace imtgui
{


class CMenuPanelComp:
			public ibase::TModelObserverCompBaseWrap<
						iqtgui::TGuiObserverWrap<
									iqtgui::TGuiComponentBase<imtwidgets::CMenuPanel>,
									imod::TSingleModelObserverBase<iprm::ISelectionParam>>>
{
	//Q_OBJECT

public:
	typedef ibase::TModelObserverCompBaseWrap<
				iqtgui::TGuiObserverWrap<
							iqtgui::TGuiComponentBase<imtwidgets::CMenuPanel>,
							imod::TSingleModelObserverBase<iprm::ISelectionParam>>> BaseClass;

	I_BEGIN_COMPONENT(CMenuPanelComp);
	I_END_COMPONENT;

protected:
	void OnPageIdChanged(const QByteArray& selectedPageId, const QByteArray& deselectedPageId);

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiRetranslate() override;

private:
	void CreateMenuForSelection(const iprm::ISelectionParam& selection, const QByteArray& parentId);
};


} // namespace imtgui


