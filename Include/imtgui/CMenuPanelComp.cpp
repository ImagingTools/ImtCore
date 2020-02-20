#include <imtgui/CMenuPanelComp.h>


// ACF includes
#include <iprm/IOptionsList.h>
#include <iqtgui/IMultiVisualStatusProvider.h>

// ImtCore includes
#include <imtgui/CMonitorInfoProvider.h>


namespace imtgui
{


// public methods

CMenuPanelComp::CMenuPanelComp()
	:m_pageSubselectionObserver(*this),
	m_pageVisualStatusObserver(*this),
	m_monitorInfoObserver(*this)
{
	m_subselectionModelIndex = 0;
	m_visualStatusModelIndex = 0;
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
	widgetPtr->SetItemPadding(5);
	widgetPtr->SetIconSize(28);

	widgetPtr->SetItemTextColor(QColor("#656565"));
	widgetPtr->SetItemSelectedColor(QColor(255, 255, 255));
	widgetPtr->SetItemSelectedContourColor(QColor(77, 113, 163));
	widgetPtr->SetItemMouserOverColor(QColor(0, 205, 255));
	widgetPtr->SetItemMouserOverSelectedColor(QColor(0, 172, 220));

	widgetPtr->SetAnimationDelay(800);
	widgetPtr->SetAnimationDuration(150);

	imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(m_monitorInfoProviderPtr.GetPtr());
	if (modelPtr != nullptr){
		modelPtr->AttachObserver(&m_monitorInfoObserver);
	}
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CMenuPanelComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	iprm::ISelectionParam* pageSelectionPtr = GetObservedObject();
	Q_ASSERT(pageSelectionPtr != nullptr);

	UpdateSelection(*pageSelectionPtr, QByteArray());
}


void CMenuPanelComp::OnGuiModelAttached()
{
	BaseClass::OnGuiModelAttached();

	iprm::ISelectionParam* pageSelectionPtr = GetObservedObject();
	Q_ASSERT(pageSelectionPtr != nullptr);

	CreatePageTree(*pageSelectionPtr, QByteArray());
}


void CMenuPanelComp::OnGuiModelDetached()
{
	m_pageSubselectionObserver.UnregisterAllModels();
	m_pageVisualStatusObserver.UnregisterAllModels();

	m_pagesInfoMap.clear();

	m_subselectionModelIndex = 0;
	m_visualStatusModelIndex = 0;

	imtwidgets::CMenuPanel* panelPtr = GetQtWidget();
	Q_ASSERT(panelPtr != nullptr);

	panelPtr->ResetPages();

	BaseClass::OnGuiModelDetached();
}


// reimplemented (iqtgui::CGuiComponentBase)

void CMenuPanelComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	if (!IsUpdateBlocked()){
		UpdateBlocker blockUpdate(this);
	
		UpdateGui(istd::IChangeable::GetAnyChange());
	}
}


// private methods

bool CMenuPanelComp::IsPageEnabled(const QByteArray& pageId) const
{
	QByteArray currentId = pageId;

	while (!currentId.isEmpty()){
		if (m_pagesInfoMap.contains(currentId)){
			PageInfo pageInfo = m_pagesInfoMap[currentId];
			const iprm::IOptionsList* list = pageInfo.selectionPtr->GetSelectionConstraints();
			if (!list->IsOptionEnabled(pageInfo.pageIndex)){
				return false;
			}

			currentId = m_pagesInfoMap[currentId].parentPageId;
		}
		else{
			return false;
		}
	}

	return true;
}


void CMenuPanelComp::UpdateSelection(const iprm::ISelectionParam& selection, const QByteArray& /*parentId*/)
{
	imtwidgets::CMenuPanel* panelPtr = GetQtWidget();
	Q_ASSERT(panelPtr != nullptr);

	const iqtgui::IMultiVisualStatusProvider* visualStatusProviderPtr = dynamic_cast<const iqtgui::IMultiVisualStatusProvider*>(&selection);
	Q_ASSERT(visualStatusProviderPtr != nullptr);

	const iprm::IOptionsList* pageListPtr = selection.GetSelectionConstraints();
	if (pageListPtr != nullptr){
		int pageCount = pageListPtr->GetOptionsCount();
		for (int pageIndex = 0; pageIndex < pageCount; ++pageIndex){
			QString pageName = pageListPtr->GetOptionName(pageIndex);
			QByteArray pageId = pageListPtr->GetOptionId(pageIndex);

			panelPtr->SetPageName(pageId, pageName);

			bool isPageEnabled = IsPageEnabled(pageId);
			panelPtr->SetPageEnabled(pageId, isPageEnabled);

			const iqtgui::IVisualStatus* visualStatusPtr = visualStatusProviderPtr->GetVisualStatus(pageIndex);
			if (visualStatusPtr != nullptr) {
				QIcon icon = visualStatusPtr->GetStatusIcon();

				panelPtr->SetPageIcon(pageId, icon);
			}

			const iprm::ISelectionParam* subSelectionPtr = selection.GetSubselection(pageIndex);
			if (subSelectionPtr != nullptr) {
				UpdateSelection(*subSelectionPtr, pageId);
			}
		}
	}

	panelPtr->SetActivePage(FindSelectedItem());
}


void CMenuPanelComp::CreatePageTree(const iprm::ISelectionParam& selection, const QByteArray& parentId)
{
	imtwidgets::CMenuPanel* panelPtr = GetQtWidget();
	Q_ASSERT(panelPtr != nullptr);

	int currentIndex = selection.GetSelectedOptionIndex();
	QByteArray currentPageId;

	const iqtgui::IMultiVisualStatusProvider* visualStatusProviderPtr = dynamic_cast<const iqtgui::IMultiVisualStatusProvider*>(&selection);
	if (visualStatusProviderPtr != nullptr){
		for (int i = 0; i < visualStatusProviderPtr->GetStatusesCount(); i++){
			const iqtgui::IVisualStatus* visualStatusPtr = visualStatusProviderPtr->GetVisualStatus(i);
			
			if (visualStatusPtr != nullptr){
				imod::IModel* modelPtr = dynamic_cast<imod::IModel*>(const_cast<iqtgui::IVisualStatus*>(visualStatusPtr));
				if (modelPtr != nullptr){
					m_pageVisualStatusObserver.RegisterModel(modelPtr, m_visualStatusModelIndex++);
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

			panelPtr->InsertPage(pageId, parentId);

			const iprm::ISelectionParam* subSelectionPtr = selection.GetSubselection(pageIndex);
			if (subSelectionPtr != nullptr){
				const imod::IModel* constModelPtr = dynamic_cast<const imod::IModel*>(subSelectionPtr);
				imod::IModel* modelPtr = const_cast<imod::IModel*>(constModelPtr);
				if (modelPtr != nullptr){
					m_pageSubselectionObserver.RegisterModel(modelPtr, m_subselectionModelIndex++);
				}

				CreatePageTree(*subSelectionPtr, pageId);
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
		else{
			break;
		}
	}

	return QByteArray(selectedPageId.toLatin1());
}


void CMenuPanelComp::UpdatePageSubselection()
{
	if (!IsUpdateBlocked()){
		UpdateBlocker updateBlocker(this);

		UpdateGui(istd::IChangeable::GetAnyChange());
	}
}


void CMenuPanelComp::UpdatePageState()
{
	if (!IsUpdateBlocked()){
		UpdateBlocker updateBlocker(this);

		UpdateGui(istd::IChangeable::GetAnyChange());
	}
}


void CMenuPanelComp::UpdateMonitorsInfo()
{

}


// protected methods of embedded class PageSubselectionObserver

CMenuPanelComp::PageSubselectionObserver::PageSubselectionObserver(CMenuPanelComp& parent)
	:m_parent(parent)
{
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CMenuPanelComp::PageSubselectionObserver::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_parent.UpdatePageSubselection();
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


// protected methods of embedded class PageStatusObserver

CMenuPanelComp::MonitorsInfoObserver::MonitorsInfoObserver(CMenuPanelComp& parent)
	:m_parent(parent)
{
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CMenuPanelComp::MonitorsInfoObserver::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	int event = changeSet.GetIds().values()[0];

	imtgui::IMonitorInfoProvider* monitorInfoProviderPtr = GetObservedObject();
	
	imtwidgets::CMenuPanel* widgetPtr = m_parent.GetQtWidget();
	Q_ASSERT(widgetPtr != nullptr);

	widgetPtr->SetIconSize(8 * monitorInfoProviderPtr->GetPhysicalResolutionX(0));
	widgetPtr->SetItemIndent(6 * monitorInfoProviderPtr->GetPhysicalResolutionX(0));
	widgetPtr->SetItemPadding(1 * monitorInfoProviderPtr->GetPhysicalResolutionX(0));

	switch (event){
	case CMonitorInfoProvider::MCE_PRIMARY_SCREEN_CHANGED:
		qDebug() << "MCE_PRIMARY_SCREEN_CHANGED";
		break;
	case CMonitorInfoProvider::MCE_SCREEN_ADDED:
		qDebug() << "MCE_SCREEN_ADDED";
		break;
	case CMonitorInfoProvider::MCE_SCREEN_REMOVED:
		qDebug() << "MCE_SCREEN_REMOVED";
		break;

	case CMonitorInfoProvider::MCE_PHYSICAL_SIZE:
		qDebug() << "MCE_PHYSICAL_SIZE";
		break;
	case CMonitorInfoProvider::MCE_PHYSICAL_RESOLUTION:
		qDebug() << "MCE_PHYSICAL_RESOLUTION";
		break;
	case CMonitorInfoProvider::MCE_ORIENTATION:
		qDebug() << "MCE_ORIENTATION";
		break;
	}
}


} // namespace imtgui


