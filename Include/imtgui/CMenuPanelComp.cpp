#include <imtgui/CMenuPanelComp.h>


// ACF includes
#include <iprm/IOptionsList.h>
#include <iqtgui/IMultiVisualStatusProvider.h>


namespace imtgui
{


// public methods

CMenuPanelComp::CMenuPanelComp()
			:m_pageVisualStatusObserver(*this)
{

}


// protected methods

void CMenuPanelComp::OnPageIdChanged(const QByteArray& selectedPageId, const QByteArray& /*deselectedPageId*/)
{
	if (!IsUpdateBlocked()){
		UpdateBlocker block(this);

		if (!selectedPageId.isEmpty()){
			if (m_pagesInfoMap.contains(selectedPageId)){
				PageInfo currentPageInfo = m_pagesInfoMap[selectedPageId];

				iprm::ISelectionParam *currentSelectionParamPtr = const_cast<iprm::ISelectionParam*>(currentPageInfo.selectionPtr);
				Q_ASSERT(currentSelectionParamPtr != nullptr);

				currentSelectionParamPtr->SetSelectedOptionIndex(currentPageInfo.pageIndex);

				QByteArray pageId = m_pagesInfoMap[selectedPageId].parentPageId;
				while (!pageId.isEmpty()){
					currentPageInfo = m_pagesInfoMap[pageId];
					currentSelectionParamPtr = const_cast<iprm::ISelectionParam*>(currentPageInfo.selectionPtr);
					Q_ASSERT(currentSelectionParamPtr != nullptr);

					currentSelectionParamPtr->SetSelectedOptionIndex(currentPageInfo.pageIndex);
					
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

	if (m_widgetProviderCompPtr.IsValid() && m_isShowOverAttrPtr.IsValid() && *m_isShowOverAttrPtr){
		widgetPtr->SetMainWidget(m_widgetProviderCompPtr->GetWidgetPtr(QByteArray()));
	}

	widgetPtr->SetItemIndent(20);
	widgetPtr->SetItemPadding(4);

	widgetPtr->SetIconSize(24);

	widgetPtr->SetItemTextColor(QColor("#335777"));
	widgetPtr->SetItemSelectedColor(QColor(240, 200, 120));
	widgetPtr->SetItemSelectedContourColor(QColor("#335777"));
	widgetPtr->SetItemMouserOverColor(QColor(240, 220, 100));
	widgetPtr->SetItemMouserOverSelectedColor(QColor(255, 235, 100));

	widgetPtr->SetAnimationDelay(800);
	widgetPtr->SetAnimationDuration(300);
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CMenuPanelComp::UpdateGui(const istd::IChangeable::ChangeSet& changeSet)
{
	imtwidgets::CMenuPanel* panelPtr = GetQtWidget();
	Q_ASSERT(panelPtr != nullptr);

	panelPtr->ResetPages();

	iprm::ISelectionParam* pageSelectionPtr = GetObservedObject();
	Q_ASSERT(pageSelectionPtr != nullptr);

	{
		UpdateBlocker updateBlocker(this);

		UnregisterAllModels();
		m_pageVisualStatusObserver.UnregisterAllModels();
		m_pagesInfoMap.clear();
		m_modelIndex = 0;

		CreatePagesInfoMap(*pageSelectionPtr, QByteArray());
		CreateMenuForSelection(*pageSelectionPtr, QByteArray());
	}

	

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
	if (!IsUpdateBlocked()){
		UpdateGui(changeSet);
	}
}


// private methods

bool CMenuPanelComp::IsPageEnabled(const QByteArray& pageId) const
{
	QByteArray currentId = pageId;

	while (!currentId.isEmpty()){
		PageInfo pageInfo = m_pagesInfoMap[currentId];
		const iprm::IOptionsList* list = pageInfo.selectionPtr->GetSelectionConstraints();
		if (!list->IsOptionEnabled(pageInfo.pageIndex)){
			return false;
		}

		currentId = m_pagesInfoMap[currentId].parentPageId;
	}

	return true;
}


void CMenuPanelComp::CreateMenuForSelection(const iprm::ISelectionParam& selection, const QByteArray& parentId)
{
	imtwidgets::CMenuPanel* panelPtr = GetQtWidget();
	Q_ASSERT(panelPtr != nullptr);

	//for (int i = 0; i < 40; i++){
	//	QByteArray id = QString("page%1").arg(i).toLatin1();
	//	panelPtr->InsertPage(id, QByteArray());
	//	panelPtr->SetPageName(id, id);
	//}
	//return;

	const iqtgui::IMultiVisualStatusProvider* visualStatusProviderPtr = dynamic_cast<const iqtgui::IMultiVisualStatusProvider*>(&selection);
	Q_ASSERT(visualStatusProviderPtr != nullptr);

	int currentIndex = selection.GetSelectedOptionIndex();
	const iprm::IOptionsList* pageListPtr = selection.GetSelectionConstraints();
	if (pageListPtr != nullptr){
		int pageCount = pageListPtr->GetOptionsCount();
		for (int pageIndex = 0; pageIndex < pageCount; ++pageIndex){
			QString pageName = pageListPtr->GetOptionName(pageIndex);
			QByteArray pageId = pageListPtr->GetOptionId(pageIndex);

			if (panelPtr->InsertPage(pageId, parentId)){
				panelPtr->SetPageName(pageId, pageName);

				bool isPageEnabled = IsPageEnabled(pageId);
				
				panelPtr->SetPageEnabled(pageId, isPageEnabled);

				const iqtgui::IVisualStatus* visualStatusPtr = visualStatusProviderPtr->GetVisualStatus(pageIndex);
				if (visualStatusPtr != nullptr){
					QIcon icon = visualStatusPtr->GetStatusIcon();

					panelPtr->SetPageIcon(pageId, icon);
				}

				const iprm::ISelectionParam* subSelectionPtr = selection.GetSubselection(pageIndex);
				if (subSelectionPtr != nullptr){
					CreateMenuForSelection(*subSelectionPtr, pageId);
				}
			}
		}
	}
}


void CMenuPanelComp::CreatePagesInfoMap(const iprm::ISelectionParam& selection, const QByteArray& parentId)
{
	int currentIndex = selection.GetSelectedOptionIndex();
	QByteArray currentPageId;

	const iqtgui::IMultiVisualStatusProvider* visualStatusProviderPtr = dynamic_cast<const iqtgui::IMultiVisualStatusProvider*>(&selection);
	if (visualStatusProviderPtr != nullptr){
		for (int i = 0; i < visualStatusProviderPtr->GetStatusesCount(); i++){
			const iqtgui::IVisualStatus* visualStatusPtr = visualStatusProviderPtr->GetVisualStatus(i);
			
			if (visualStatusPtr != nullptr){
				imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(const_cast<iqtgui::IVisualStatus*>(visualStatusPtr));
				if (modelPtr != nullptr){
					m_pageVisualStatusObserver.RegisterModel(modelPtr, m_modelIndex++);
				}
			}
		}
	}

	const iprm::IOptionsList* pageListPtr = selection.GetSelectionConstraints();
	if (pageListPtr != nullptr){
		int pageCount = pageListPtr->GetOptionsCount();
		for (int pageIndex = 0; pageIndex < pageCount; ++pageIndex){
			QString pageName = pageListPtr->GetOptionName(pageIndex);
			QByteArray pageId = pageListPtr->GetOptionId(pageIndex);

			PageInfo pageInfo;
			pageInfo.parentPageId = parentId;
			pageInfo.selectionPtr = &selection;
			pageInfo.pageIndex = pageIndex;
			m_pagesInfoMap[pageId] = pageInfo;

			if (pageIndex == currentIndex){
				currentPageId = pageId;
			}

			const iprm::ISelectionParam* subSelectionPtr = selection.GetSubselection(pageIndex);
			if (subSelectionPtr != nullptr){
				const imod::IModel* constModelPtr = dynamic_cast<const imod::IModel*>(subSelectionPtr);
				imod::IModel* modelPtr = const_cast<imod::IModel*>(constModelPtr);
				if (modelPtr != nullptr){
					RegisterModel(modelPtr, m_modelIndex++);
				}
				CreatePagesInfoMap(*subSelectionPtr, pageId);
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


void CMenuPanelComp::UpdatePageState()
{

}


// protected methods of embedded class PageStatusObserver

CMenuPanelComp::PageVisualStatusObserver::PageVisualStatusObserver(CMenuPanelComp& parent)
	:m_parent(parent)
{
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CMenuPanelComp::PageVisualStatusObserver::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_parent.UpdatePageState();
}

} // namespace imtgui


