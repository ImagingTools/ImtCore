#include <imtgui/CMenuPanelComp.h>


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

		iprm::ISelectionParam* pageSelectionPtr = GetObservedObject();
		Q_ASSERT(pageSelectionPtr != nullptr);

		const iprm::IOptionsList* pageListPtr = pageSelectionPtr->GetSelectionConstraints();
		if (pageListPtr != nullptr) {
			int pageCount = pageListPtr->GetOptionsCount();
			for (int pageIndex = 0; pageIndex < pageCount; ++pageIndex) {
				QByteArray pageId = pageListPtr->GetOptionId(pageIndex);
				if (pageId == selectedPageId) {
					pageSelectionPtr->SetSelectedOptionIndex(pageIndex);
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
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CMenuPanelComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtwidgets::CMenuPanel* panelPtr = GetQtWidget();
	Q_ASSERT(panelPtr != nullptr);

	panelPtr->ResetPages();

	iprm::ISelectionParam* pageSelectionPtr = GetObservedObject();
	iqtgui::IMultiVisualStatusProvider* pageVisualStatus = dynamic_cast<iqtgui::IMultiVisualStatusProvider*>(GetObservedObject());

	Q_ASSERT(pageSelectionPtr != nullptr);
	Q_ASSERT(pageVisualStatus != nullptr);

	int currentIndex = pageSelectionPtr->GetSelectedOptionIndex();
	QByteArray currentPageId;
	const iprm::IOptionsList* pageListPtr = pageSelectionPtr->GetSelectionConstraints();
	if (pageListPtr != nullptr){
		int pageCount = pageListPtr->GetOptionsCount();
		for (int pageIndex = 0; pageIndex < pageCount; ++pageIndex){
			QString pageName = pageListPtr->GetOptionName(pageIndex);
			QByteArray pageId = pageListPtr->GetOptionId(pageIndex);

			if (panelPtr->InsertPage(pageId)){
				panelPtr->SetPageName(pageId, pageName);
				
				QIcon icon = pageVisualStatus->GetVisualStatus(pageIndex)->GetStatusIcon();
				panelPtr->SetPageIcon(pageId, icon);

				panelPtr->SetPageEnabled(pageId, pageListPtr->IsOptionEnabled(pageIndex));
			}

			if (pageIndex == currentIndex){
				currentPageId = pageId;
			}
		}
	}

	if (!currentPageId.isEmpty()) {
		panelPtr->SetActivePage(currentPageId);
	}
}


// reimplemented (imod::CSingleModelObserverBase)

void CMenuPanelComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	iprm::ISelectionParam* pageSelectionPtr = GetObservedObject();

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


} // namespace imtgui


