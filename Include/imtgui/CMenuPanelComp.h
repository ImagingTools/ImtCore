#pragma once


// ACF includes
#include <ibase/TModelObserverCompWrap.h>
#include <iqtgui/TGuiObserverWrap.h>
#include <iqtgui/TGuiComponentBase.h>
#include <iprm/ISelectionParam.h>
#include <imod/CMultiModelDispatcherBase.h>

// ImtCore includes
#include <imtwidgets/CMenuPanel.h>


namespace imtgui
{


class CMenuPanelComp:
			public ibase::TModelObserverCompBaseWrap<
						iqtgui::TGuiObserverWrap<
									iqtgui::TGuiComponentBase<imtwidgets::CMenuPanel>,
									imod::TSingleModelObserverBase<iprm::ISelectionParam>>>,
			public imod::CMultiModelDispatcherBase
{
	//Q_OBJECT

public:
	typedef ibase::TModelObserverCompBaseWrap<
				iqtgui::TGuiObserverWrap<
							iqtgui::TGuiComponentBase<imtwidgets::CMenuPanel>,
							imod::TSingleModelObserverBase<iprm::ISelectionParam>>> BaseClass;
	typedef public imod::CMultiModelDispatcherBase BaseClass2;

	I_BEGIN_COMPONENT(CMenuPanelComp);
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

private:
	void CreateMenuForSelection(const iprm::ISelectionParam& selection, const QByteArray& parentId);
	void CreatePageIdAliases(const iprm::ISelectionParam& selection, const QByteArray& parentId);
	QByteArray FindSelectedItem();
};


} // namespace imtgui


