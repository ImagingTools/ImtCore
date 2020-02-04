#include <imtgui/CMenuPanelComp.h>

// ACF includes
#include <iprm/IOptionsList.h>
#include <iqtgui/IMultiVisualStatusProvider.h>

// ImtCore includes
#include <imtwidgets/CMenuPanel.h>

#include <QtCore/QDebug>


namespace imtgui
{


// protected methods

void CMenuPanelComp::OnPageIdChanged(const QByteArray& selectedPageId, const QByteArray& /*deselectedPageId*/)
{
	iprm::ISelectionParam* pageSelectionPtr = GetObservedObject();

	Q_ASSERT(pageSelectionPtr != nullptr);

	const iprm::IOptionsList* pageListPtr = pageSelectionPtr->GetSelectionConstraints();
	if (pageListPtr != nullptr){
		int pageCount = pageListPtr->GetOptionsCount();
		for (int pageIndex = 0; pageIndex < pageCount; ++pageIndex){
			QString pageName = pageListPtr->GetOptionName(pageIndex);
			QByteArray pageId = pageListPtr->GetOptionId(pageIndex);

			if (pageId == selectedPageId){
				pageSelectionPtr->SetSelectedOptionIndex(pageIndex);
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
			}
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


