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
			if (m_pageIdToSelectionAlias.contains(selectedPageId)) {
				PageIdToSelectionAlias alias = m_pageIdToSelectionAlias[selectedPageId];
				iprm::ISelectionParam *selectionParam = const_cast<iprm::ISelectionParam*>(alias.selectionPtr);
				selectionParam->SetSelectedOptionIndex(alias.pageIndex);
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

	widgetPtr->SetItemPadding(6);
	widgetPtr->SetIconSize(32);

	widgetPtr->SetItemSelectedColor(QColor(240, 200, 80));
	widgetPtr->SetItemMouserOverColor(QColor(240, 220, 100));
	widgetPtr->SetItemMouserOverSelectedColor(QColor(240, 220, 100));
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CMenuPanelComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtwidgets::CMenuPanel* panelPtr = GetQtWidget();
	Q_ASSERT(panelPtr != nullptr);

	panelPtr->ResetPages();

	iprm::ISelectionParam* pageSelectionPtr = GetObservedObject();
	Q_ASSERT(pageSelectionPtr != nullptr);

	CreateMenuForSelection(*pageSelectionPtr, QByteArray());
}


// reimplemented (imod::CSingleModelObserverBase)

void CMenuPanelComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	m_pageIdToSelectionAlias.clear();

	iprm::ISelectionParam* pageSelectionPtr = GetObservedObject();
	Q_ASSERT(pageSelectionPtr != nullptr);

	CreatePageIdAliases(*pageSelectionPtr, QByteArray());

	Q_ASSERT(pageSelectionPtr != nullptr);

	int selectedIndex = pageSelectionPtr->GetSelectedOptionIndex();
	const iprm::IOptionsList* pageListPtr = pageSelectionPtr->GetSelectionConstraints();
	if (pageListPtr != nullptr){
		QByteArray selectedId;
		if (selectedIndex >= 0){
			selectedId = pageListPtr->GetOptionId(selectedIndex);
		}

		if (IsGuiCreated()){
			imtwidgets::CMenuPanel* widgetPtr = dynamic_cast<imtwidgets::CMenuPanel*>(GetWidget());
			widgetPtr->SetActivePage(selectedId);
		}
	}
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

				static int i = 0;
				panelPtr->SetPageEnabled(pageId, (i++) % 2/*pageListPtr->IsOptionEnabled(pageIndex)*/);
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

	if (!currentPageId.isEmpty()){
		panelPtr->SetActivePage(currentPageId);
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

			if (pageId.contains("Camera")){
				qDebug() << pageId << &selection << pageIndex;
			}

			PageIdToSelectionAlias alias;
			alias.parentPageId = parentId;
			alias.selectionPtr = &selection;
			alias.pageIndex = pageIndex;
			m_pageIdToSelectionAlias[pageId] = alias;

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


} // namespace imtgui


