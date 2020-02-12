#pragma once


// ACF includes
#include <ibase/TModelObserverCompWrap.h>
#include <iqtgui/TGuiObserverWrap.h>
#include <iqtgui/TGuiComponentBase.h>
#include <iprm/ISelectionParam.h>
#include <imod/CMultiModelDispatcherBase.h>

// ImtCore includes
#include <imtgui/IWidgetProvider.h>
#include <imtwidgets/CMenuPanel.h>


namespace imtgui
{


class CMenuPanelComp:
			public ibase::TModelObserverCompBaseWrap<
						iqtgui::TGuiObserverWrap<
									iqtgui::TGuiComponentBase<imtwidgets::CMenuPanel>,
									imod::TSingleModelObserverBase<iprm::ISelectionParam>>>,
			protected imod::CMultiModelDispatcherBase
{
public:
	typedef ibase::TModelObserverCompBaseWrap<
				iqtgui::TGuiObserverWrap<
							iqtgui::TGuiComponentBase<imtwidgets::CMenuPanel>,
							imod::TSingleModelObserverBase<iprm::ISelectionParam>>> BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CMenuPanelComp);
	I_ASSIGN(m_widgetProviderPtr, "WidgetProvider", "Widget provider for parent widget", false, "WidgetProvider");
	I_ASSIGN(m_isShowOverPtr, "ShowOver", "Show menu over", false, true);
	I_END_COMPONENT;

protected:
	void OnPageIdChanged(const QByteArray& selectedPageId, const QByteArray& deselectedPageId);

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;

	// reimplemented (iqtgui::TGuiObserverWrap)
	virtual void UpdateGui(const istd::IChangeable::ChangeSet& changeSet) override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiRetranslate() override;

	// reimplemented (imod::CMultiModelDispatcherBase)
	virtual void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

private:
	struct PageIdToSelectionAlias
	{
		QByteArray parentPageId;
		const iprm::ISelectionParam* selectionPtr;
		int pageIndex;
	};

	QMap<QByteArray, PageIdToSelectionAlias> m_pagesInfoMap;

	I_REF(imtgui::IWidgetProvider, m_widgetProviderPtr);
    I_ATTR(bool, m_isShowOverPtr);

private:
	void CreateMenuForSelection(const iprm::ISelectionParam& selection, const QByteArray& parentId);
	void CreatePageIdAliases(const iprm::ISelectionParam& selection, const QByteArray& parentId);
	QByteArray FindSelectedItem();
};


} // namespace imtgui


