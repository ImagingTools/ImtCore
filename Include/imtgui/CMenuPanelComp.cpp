#include <imtgui/CMenuPanelComp.h>


// Qt includes
#include <QtCore>
#include <QtWidgets/QGraphicsEffect>

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

	widgetPtr->SetItemPadding(6);
	widgetPtr->SetIconSize(18);

	for (int i = 1; i < 50; i++){
		widgetPtr->InsertPage(QString("page%1").arg(i).toLatin1(), QByteArray());
		widgetPtr->SetPageName(QString("page%1").arg(i).toLatin1(), QString("Page %1").arg(i).toLatin1());
		widgetPtr->SetPageIcon(QString("page%1").arg(i).toLatin1(), QIcon(":/Icons/Error"));
		qDebug() << QString("page%1").arg(i).toLatin1();
	}
	widgetPtr->InsertPage("page`21", "page2");
	widgetPtr->InsertPage("page`22", "page2");
	widgetPtr->InsertPage("page`23", "page2");
	widgetPtr->InsertPage("page211", "page21");
	widgetPtr->InsertPage("page212", "page21");
	widgetPtr->InsertPage("page213", "page21");
	
	widgetPtr->SetPageName("page`21", "Page 2.1");
	widgetPtr->SetPageName("page`22", "Page 2.2");
	widgetPtr->SetPageName("page`23", "Page 2.3");
	widgetPtr->SetPageName("page211", "Page 2.1.1");
	widgetPtr->SetPageName("page212", "Page 2.1.2");
	widgetPtr->SetPageName("page213", "Page 2.1.3");

	widgetPtr->SetPageIcon("page`21", QIcon(":/Icons/Error"));
	widgetPtr->SetPageIcon("page`22", QIcon(":/Icons/Error"));
	widgetPtr->SetPageIcon("page`23", QIcon(":/Icons/Error"));
	widgetPtr->SetPageIcon("page211", QIcon(":/Icons/Error"));
	widgetPtr->SetPageIcon("page212", QIcon(":/Icons/Error"));
	widgetPtr->SetPageIcon("page213", QIcon(":/Icons/Error"));
}


// reimplemented (iqtgui::TGuiObserverWrap)

void CMenuPanelComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	//imtwidgets::CMenuPanel* panelPtr = GetQtWidget();
	//Q_ASSERT(panelPtr != nullptr);

	//panelPtr->ResetPages();

	//iprm::ISelectionParam* pageSelectionPtr = GetObservedObject();
	//Q_ASSERT(pageSelectionPtr != nullptr);

	//CreateMenuForSelection(*pageSelectionPtr, QByteArray());
}


// reimplemented (imod::CSingleModelObserverBase)

void CMenuPanelComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	//iprm::ISelectionParam* pageSelectionPtr = GetObservedObject();

	//Q_ASSERT(pageSelectionPtr != nullptr);

	//int selectedIndex = pageSelectionPtr->GetSelectedOptionIndex();
	//const iprm::IOptionsList* pageListPtr = pageSelectionPtr->GetSelectionConstraints();
	//if (pageListPtr != nullptr){
	//	QByteArray selectedId;
	//	if (selectedIndex >= 0){
	//		selectedId = pageListPtr->GetOptionId(selectedIndex);
	//	}

	//	if (IsGuiCreated()){
	//		imtwidgets::CMenuPanel* widgetPtr = dynamic_cast<imtwidgets::CMenuPanel*>(GetWidget());
	//		widgetPtr->SetActivePage(selectedId);
	//	}
	//}
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

			if (panelPtr->InsertPage(pageId)) {
				panelPtr->SetPageName(pageId, pageName);

				const iqtgui::IVisualStatus* visualStatusPtr = pageVisualStatus->GetVisualStatus(pageIndex);
				if (visualStatusPtr != nullptr){
					QIcon icon = pageVisualStatus->GetVisualStatus(pageIndex)->GetStatusIcon();
					qDebug() << "*** " << pageId << icon.isNull();
					if (icon.isNull()){
						icon = QIcon(QIcon(":/Icons/StateInvalid"));
					}
					panelPtr->SetPageIcon(pageId, icon);
				}

				panelPtr->SetPageEnabled(pageId, pageListPtr->IsOptionEnabled(pageIndex));
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


} // namespace imtgui


