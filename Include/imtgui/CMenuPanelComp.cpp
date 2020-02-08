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
			if (m_pagesInfoMap.contains(selectedPageId)) {
				PageIdToSelectionAlias currentAlias = m_pagesInfoMap[selectedPageId];
				iprm::ISelectionParam *currentSelectionParam = const_cast<iprm::ISelectionParam*>(currentAlias.selectionPtr);

				//qDebug() << currentAlias.parentPageId << currentAlias.selectionPtr << currentAlias.pageIndex;

				currentSelectionParam->SetSelectedOptionIndex(currentAlias.pageIndex);

				QByteArray pageId = m_pagesInfoMap[selectedPageId].parentPageId;
				while (!pageId.isEmpty()){
					currentAlias = m_pagesInfoMap[pageId];
					currentSelectionParam = const_cast<iprm::ISelectionParam*>(currentAlias.selectionPtr);
					currentSelectionParam->SetSelectedOptionIndex(currentAlias.pageIndex);

					//qDebug() << currentAlias.parentPageId << currentAlias.selectionPtr << currentAlias.pageIndex;

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
	connect(widgetPtr, &imtwidgets::CMenuPanel::PageIdChanged, this, &CMenuPanelComp::OnPageIdChanged);

	widgetPtr->SetItemPadding(4);
	widgetPtr->SetIconSize(24);

	widgetPtr->SetItemSelectedColor(QColor(240, 200, 120));
	widgetPtr->SetItemMouserOverColor(QColor(240, 220, 100));
	widgetPtr->SetItemMouserOverSelectedColor(QColor(255, 235, 100));
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CMenuPanelComp::UpdateGui(const istd::IChangeable::ChangeSet& changeSet)
{
	imtwidgets::CMenuPanel* panelPtr = GetQtWidget();
	Q_ASSERT(panelPtr != nullptr);

	panelPtr->ResetPages();

	m_pagesInfoMap.clear();

	iprm::ISelectionParam* pageSelectionPtr = GetObservedObject();
	Q_ASSERT(pageSelectionPtr != nullptr);

	CreatePageIdAliases(*pageSelectionPtr, QByteArray());
	CreateMenuForSelection(*pageSelectionPtr, QByteArray());

	QByteArray selectedPageId = FindSelectedItem();
	panelPtr->SetActivePage(selectedPageId);

	BaseClass::UpdateGui(changeSet);
}


// reimplemented (iqtgui::CGuiComponentBase)

void CMenuPanelComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	UpdateGui(istd::IChangeable::GetAnyChange());
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
	if (pageListPtr != nullptr) {
		int pageCount = pageListPtr->GetOptionsCount();
		for (int pageIndex = 0; pageIndex < pageCount; ++pageIndex) {
			QString pageName = pageListPtr->GetOptionName(pageIndex);
			QByteArray pageId = pageListPtr->GetOptionId(pageIndex);

			if (panelPtr->InsertPage(pageId, parentId)) {
				panelPtr->SetPageName(pageId, pageName);
				panelPtr->SetPageEnabled(pageId, pageListPtr->IsOptionEnabled(pageIndex));

				const iqtgui::IVisualStatus* visualStatusPtr = pageVisualStatus->GetVisualStatus(pageIndex);
				if (visualStatusPtr != nullptr){
					QIcon icon = pageVisualStatus->GetVisualStatus(pageIndex)->GetStatusIcon();
					if (icon.isNull()){
						icon = QIcon(":/Icons/StateInvalid");
					}
					panelPtr->SetPageIcon(pageId, icon);
				}
				else {
					panelPtr->SetPageIcon(pageId, QIcon(":/Icons/Error"));
				}
			}

			if (pageIndex == currentIndex){
				currentPageId = pageId;
			}

			const iprm::ISelectionParam* subSelectionPtr = selection.GetSubselection(pageIndex);
			if (subSelectionPtr != nullptr) {
				CreateMenuForSelection(*subSelectionPtr, pageId);
			}
		}
	}
}


void CMenuPanelComp::CreatePageIdAliases(const iprm::ISelectionParam& selection, const QByteArray& parentId)
{
	int currentIndex = selection.GetSelectedOptionIndex();
	QByteArray currentPageId;
	const iprm::IOptionsList* pageListPtr = selection.GetSelectionConstraints();
	if (pageListPtr != nullptr) {
		int pageCount = pageListPtr->GetOptionsCount();
		for (int pageIndex = 0; pageIndex < pageCount; ++pageIndex) {
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
			if (subSelectionPtr != nullptr) {
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
		if (selectedIndex >= 0) {
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


