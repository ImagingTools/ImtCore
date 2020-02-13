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
	CMenuPanelComp();

	typedef ibase::TModelObserverCompBaseWrap<
				iqtgui::TGuiObserverWrap<
							iqtgui::TGuiComponentBase<imtwidgets::CMenuPanel>,
							imod::TSingleModelObserverBase<iprm::ISelectionParam>>> BaseClass;
	typedef imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CMenuPanelComp);
		I_ASSIGN(m_widgetProviderCompPtr, "WidgetProvider", "Widget provider for parent widget", false, "WidgetProvider");
		I_ASSIGN(m_isShowOverAttrPtr, "ShowOver", "Show expanded menu over the underlaying widget", false, true);
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
	class PageVisualStatusObserver: public imod::CMultiModelDispatcherBase
	{
	public:
		explicit PageVisualStatusObserver(CMenuPanelComp& parent);

		// reimplemented (imod::CMultiModelDispatcherBase)
		void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CMenuPanelComp& m_parent;
	};

	struct PageInfo
	{
		QByteArray parentPageId;
		const iprm::ISelectionParam* selectionPtr;
		int pageIndex;
	};

	int m_modelIndex;
	QMap<QByteArray, PageInfo> m_pagesInfoMap;
	PageVisualStatusObserver m_pageVisualStatusObserver;

	I_REF(imtgui::IWidgetProvider, m_widgetProviderCompPtr);
	I_ATTR(bool, m_isShowOverAttrPtr);

private:
	bool IsPageEnabled(const QByteArray& pageId) const;
	void CreateMenuForSelection(const iprm::ISelectionParam& selection, const QByteArray& parentId);
	void CreatePagesInfoMap(const iprm::ISelectionParam& selection, const QByteArray& parentId);
	QByteArray FindSelectedItem();
	void UpdatePageState();
};


} // namespace imtgui


