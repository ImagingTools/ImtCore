#include <imtgui/CMenuPanelComp.h>


// Qt includes

// ACF includes
#include <iprm/IOptionsList.h>
#include <iqtgui/IMultiVisualStatusProvider.h>


namespace imtgui
{


// protected methods

void CMenuPanelComp::OnPageIdChanged(const QByteArray& selectedPageId, const QByteArray& /*deselectedPageId*/)
{
	if (!IsUpdateBlocked()){
		UpdateBlocker block(this);

		if (!selectedPageId.isEmpty()){
			if (m_pagesInfoMap.contains(selectedPageId)){
				PageIdToSelectionAlias currentAlias = m_pagesInfoMap[selectedPageId];

				iprm::ISelectionParam *currentSelectionParamPtr = const_cast<iprm::ISelectionParam*>(currentAlias.selectionPtr);
				Q_ASSERT(currentSelectionParamPtr != nullptr);

				currentSelectionParamPtr->SetSelectedOptionIndex(currentAlias.pageIndex);

				QByteArray pageId = m_pagesInfoMap[selectedPageId].parentPageId;
				while (!pageId.isEmpty()){
					currentAlias = m_pagesInfoMap[pageId];
					currentSelectionParamPtr = const_cast<iprm::ISelectionParam*>(currentAlias.selectionPtr);
					Q_ASSERT(currentSelectionParamPtr != nullptr);

					currentSelectionParamPtr->SetSelectedOptionIndex(currentAlias.pageIndex);
					
					pageId = m_pagesInfoMap[pageId].parentPageId;
				}
			}
		}
	}
}


// reimplemented (icomp::CComponentBase)

void CMenuPanelComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	imtwidgets::CMenuPanel* widgetPtr = dynamic_cast<imtwidgets::CMenuPanel*>(GetWidget());
	Q_ASSERT(widgetPtr != nullptr);
	connect(widgetPtr, &imtwidgets::CMenuPanel::PageIdChanged, this, &CMenuPanelComp::OnPageIdChanged);

	if (m_widgetProviderPtr.IsValid() && m_isShowOverPtr.IsValid() && *m_isShowOverPtr) {
		widgetPtr->SetMainWidget(m_widgetProviderPtr->GetWidgetPtr(QByteArray()));
	}

	widgetPtr->SetItemIndent(20);
	widgetPtr->SetItemPadding(4);

	widgetPtr->SetIconSize(24);

	widgetPtr->SetItemTextColor(QColor("#335777"));
	widgetPtr->SetItemSelectedColor(QColor(240, 200, 120));
	widgetPtr->SetItemSelectedContourColor(QColor("#335777"));
	widgetPtr->SetItemMouserOverColor(QColor(240, 220, 100));
	widgetPtr->SetItemMouserOverSelectedColor(QColor(255, 235, 100));

	widgetPtr->SetAnimationDelay(500);
	widgetPtr->SetAnimationDuration(300);
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CMenuPanelComp::UpdateGui(const istd::IChangeable::ChangeSet& changeSet)
{
	BaseClass::UpdateGui(changeSet);

	imtwidgets::CMenuPanel* panelPtr = GetQtWidget();
	Q_ASSERT(panelPtr != nullptr);

	panelPtr->ResetPages();

	iprm::ISelectionParam* pageSelectionPtr = GetObservedObject();
	Q_ASSERT(pageSelectionPtr != nullptr);

	m_pagesInfoMap.clear();
	CreatePageIdAliases(*pageSelectionPtr, QByteArray());
	CreateMenuForSelection(*pageSelectionPtr, QByteArray());

	QByteArray selectedPageId = FindSelectedItem();
	panelPtr->SetActivePage(selectedPageId);
}


// reimplemented (iqtgui::CGuiComponentBase)

void CMenuPanelComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	UpdateGui(istd::IChangeable::GetAnyChange());
}


void CMenuPanelComp::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet)
{
	//UpdateGui(changeSet);
}


// private methods

void CMenuPanelComp::CreateMenuForSelection(const iprm::ISelectionParam& selection, const QByteArray& parentId)
{
	imtwidgets::CMenuPanel* panelPtr = GetQtWidget();
	Q_ASSERT(panelPtr != nullptr);

	const iqtgui::IMultiVisualStatusProvider* pageVisualStatus = dynamic_cast<const iqtgui::IMultiVisualStatusProvider*>(&selection);
	Q_ASSERT(pageVisualStatus != nullptr);

	int currentIndex = selection.GetSelectedOptionIndex();
	QByteArray currentPageId;
	const iprm::IOptionsList* pageListPtr = selection.GetSelectionConstraints();
	if (pageListPtr != nullptr){
		int pageCount = pageListPtr->GetOptionsCount();
		for (int pageIndex = 0; pageIndex < pageCount; ++pageIndex){
			QString pageName = pageListPtr->GetOptionName(pageIndex);
			QByteArray pageId = pageListPtr->GetOptionId(pageIndex);

			if (panelPtr->InsertPage(pageId, parentId)){
				panelPtr->SetPageName(pageId, pageName);
				panelPtr->SetPageEnabled(pageId, pageListPtr->IsOptionEnabled(pageIndex));

				const iqtgui::IVisualStatus* visualStatusPtr = pageVisualStatus->GetVisualStatus(pageIndex);
				if (visualStatusPtr != nullptr){
					QIcon icon = visualStatusPtr->GetStatusIcon();

					panelPtr->SetPageIcon(pageId, icon);
				}
			}

			if (pageIndex == currentIndex){
				currentPageId = pageId;
			}

			const iprm::ISelectionParam* subSelectionPtr = selection.GetSubselection(pageIndex);
			if (subSelectionPtr != nullptr){
				CreateMenuForSelection(*subSelectionPtr, pageId);
			}
		}
	}
}


void CMenuPanelComp::CreatePageIdAliases(const iprm::ISelectionParam& selection, const QByteArray& parentId)
{
	UnregisterAllModels();

	int currentIndex = selection.GetSelectedOptionIndex();
	QByteArray currentPageId;
	int subSelectionIterator = 0;

	const iprm::IOptionsList* pageListPtr = selection.GetSelectionConstraints();
	if (pageListPtr != nullptr){
		int pageCount = pageListPtr->GetOptionsCount();
		for (int pageIndex = 0; pageIndex < pageCount; ++pageIndex){
			QString pageName = pageListPtr->GetOptionName(pageIndex);
			QByteArray pageId = pageListPtr->GetOptionId(pageIndex);

			PageIdToSelectionAlias alias;
			alias.parentPageId = parentId;
			alias.selectionPtr = &selection;
			alias.pageIndex = pageIndex;
			m_pagesInfoMap[pageId] = alias;

			if (pageIndex == currentIndex){
				currentPageId = pageId;
			}

			const iprm::ISelectionParam* subSelectionPtr = selection.GetSubselection(pageIndex);
			if (subSelectionPtr != nullptr){
				const imod::IModel* constModelPtr = dynamic_cast<const imod::IModel*>(subSelectionPtr);
				imod::IModel* modelPtr = const_cast<imod::IModel*>(constModelPtr);
				if (modelPtr != nullptr){
					RegisterModel(modelPtr, subSelectionIterator++);
				}
				CreatePageIdAliases(*subSelectionPtr, pageId);
			}
		}
	}
}


QByteArray CMenuPanelComp::FindSelectedItem()
{
	iprm::ISelectionParam* selectionPtr = GetObservedObject();
	Q_ASSERT(selectionPtr != nullptr);

	QString selectedPageId;

	while (selectionPtr != nullptr){
		int selectedIndex = selectionPtr->GetSelectedOptionIndex();
		const iprm::IOptionsList *pageListPtr = selectionPtr->GetSelectionConstraints();
		if (selectedIndex >= 0){
			selectedPageId = pageListPtr->GetOptionName(selectedIndex);
			selectionPtr = selectionPtr->GetSubselection(selectedIndex);
		}
		else {
			break;
		}
	}

	return QByteArray(selectedPageId.toLatin1());
}


} // namespace imtgui


