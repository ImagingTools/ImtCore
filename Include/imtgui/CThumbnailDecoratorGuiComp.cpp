#include <imtgui/CThumbnailDecoratorGuiComp.h>


// Qt includes
#include <QtGui/QStandardItemModel>
#include <QtGui/QKeyEvent>
#include <QtGui/QGuiApplication>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QMessageBox>

// ACF includes
#include <iprm/IOptionsList.h>
#include <iwidgets/iwidgets.h>
#include <iqtgui/IMultiVisualStatusProvider.h>
#include <iqtgui/CCommandTools.h>


namespace imtgui
{


static const int s_minSpacing = 10;


// public methods

CThumbnailDecoratorGuiComp::CThumbnailDecoratorGuiComp()
	:m_commands("&View", 100),
	m_mainToolBar(NULL),
	m_commandsObserver(*this),
	m_pageModelObserver(*this),
	m_loginObserver(*this),
	m_pageVisualStatusObserver(*this),
	m_columnsCount(0),
	m_rowsCount(0),
	m_verticalSpacing(0),
	m_horizontalSpacing(0),
	m_horizontalFrameMargin(6),
	m_verticalFrameMargin(6),
	m_itemDelegate(nullptr),
	m_lastPageIndexForLoggedUser(-1),
	m_keyEnterTimerId(0)
{
	m_rootCommands.InsertChild(&m_commands);
	m_minItemSize = QSize(100, 50);
	m_maxWidth = 0;
	m_maxHeight = 0;

	qRegisterMetaType<QProcess::ExitStatus>("QProcess::ExitStatus");
	qRegisterMetaType<QProcess::ProcessState>();
}


// reimplemented (ibase::ICommandsProvider)

const imtbase::ICollectionInfo* CThumbnailDecoratorGuiComp::GetWidgetList() const
{
	return nullptr;
}


QWidget* CThumbnailDecoratorGuiComp::GetWidgetPtr(const QByteArray& /*widgetId*/) const
{
	if (IsGuiCreated()){
		return Main;
	}

	return nullptr;
}


// reimplemented (ibase::ICommandsProvider)

const ibase::IHierarchicalCommand* CThumbnailDecoratorGuiComp::GetCommands() const
{
	return &m_rootCommands;
}


// reimplemented (QOjbect)

bool CThumbnailDecoratorGuiComp::eventFilter(QObject *watched, QEvent *event)
{
	int eventType = event->type();

	if (eventType == QEvent::Resize){
		UpdateSpacing();
	}

	QSet<int> supportedEvents;
	supportedEvents.insert(QEvent::KeyRelease);
	supportedEvents.insert(QEvent::MouseButtonRelease);
	supportedEvents.insert(QEvent::MouseMove);
	supportedEvents.insert(QEvent::Wheel);
	supportedEvents.insert(QEvent::FocusIn);
	supportedEvents.insert(QEvent::FocusOut);
	supportedEvents.insert(QEvent::Enter);
	supportedEvents.insert(QEvent::Leave);

	if (m_loginCompPtr.IsValid()){
		bool isLogged = (m_loginCompPtr->GetLoggedUser() != NULL);

		if (isLogged && supportedEvents.contains(eventType)){
			// Auto log off functionality is activated, aslo restart the timer after any user activity:
			int autoLogoutSeconds = GetAutoLogoutTime();
			if (autoLogoutSeconds > 0){
				m_autoLogoutTimer.start(autoLogoutSeconds * 1000);

				qApp->installEventFilter(this);
			}
		}

		QTimerEvent* timerEventPtr = dynamic_cast<QTimerEvent*>(event);
		if (timerEventPtr != nullptr){
			if (timerEventPtr->timerId() == m_keyEnterTimerId){
				killTimer(m_keyEnterTimerId);
				m_keyEnterTimerId = 0;
			}
		}

		QKeyEvent* keyEventPtr = dynamic_cast<QKeyEvent*>(event);
		if (keyEventPtr != nullptr){
			int pressedKey = keyEventPtr->key();

			if ((pressedKey == Qt::Key_Return || pressedKey == Qt::Key_Enter) && m_keyEnterTimerId == 0){
				if ((PageStack->currentIndex() == LOGIN_PAGE_INDEX) && !isLogged && LoginButton->isEnabled()){
					on_LoginButton_clicked();
					m_keyEnterTimerId = startTimer(500);
				}
			}
		}
	}

	return BaseClass::eventFilter(watched, event);
}


// protected methods

// reimplemented (iqtgui::TRestorableGuiWrap)

void CThumbnailDecoratorGuiComp::OnRestoreSettings(const QSettings& settings)
{
	QString lastUser  = settings.value("LastUser").toString();

	UserEdit->setText(lastUser);

	if (!lastUser.isEmpty()){
		PasswordEdit->setFocus();
		PasswordEdit->setCursorPosition(0);
	}
}


void CThumbnailDecoratorGuiComp::OnSaveSettings(QSettings& settings) const
{
	QString lastUser  = UserEdit->text();
	
	settings.setValue("LastUser", lastUser);
}


// reimplemented (iqtgui::CGuiComponentBase)

void CThumbnailDecoratorGuiComp::OnGuiCreated()
{
	BaseClass::OnGuiCreated();

	if (m_mainToolBar == nullptr){
		m_mainToolBar = new QToolBar(CurrentPageToolBarFrame);
		m_mainToolBar->setProperty("ImtToolBar", true);
		m_mainToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

		CurrentPageToolBarFrame->layout()->addWidget(m_mainToolBar);
	}

	if (m_leftMenuPanelGuiCompPtr.IsValid()){
		m_leftMenuPanelGuiCompPtr->CreateGui(MenuPanelFrame);
	}

	MenuPanelFrame->setVisible(m_leftMenuPanelGuiCompPtr.IsValid());

	m_itemInfoMap.clear();
	m_subPageItemMap.clear();

	LeftFrame->setVisible(false);

	static const istd::IChangeable::ChangeSet commandsChangeSet(ibase::ICommandsProvider::CF_COMMANDS);

	if (m_commandsProviderCompPtr.IsValid() && m_commandsProviderModelCompPtr.IsValid()){
		m_commandsObserver.RegisterModel(m_commandsProviderModelCompPtr.GetPtr(), 0, commandsChangeSet);
	}

	if (m_pagesCompPtr.IsValid()){
		CreatePages(m_pagesCompPtr.GetPtr());

		if (m_pagesWidgetCompPtr.IsValid()){
			m_pagesWidgetCompPtr->CreateGui(ContentFrame);
		}

		m_pagesCompPtr->SetSelectedOptionIndex(-1);

		if (m_pagesModelCompPtr.IsValid()){
			m_pageModelObserver.RegisterModel(m_pagesModelCompPtr.GetPtr(), 0);
		}
	}

	if (m_loginModelCompPtr.IsValid()){
		m_loginObserver.RegisterModel(m_loginModelCompPtr.GetPtr());
	}

	if (m_rightsModelCompPtr.IsValid()){
		m_loginObserver.RegisterModel(m_rightsModelCompPtr.GetPtr(), 1);
	}

	if (m_defaultPageIndexAttrPtr.IsValid()){
		int startPageIndex = *m_defaultPageIndexAttrPtr;
		if (startPageIndex >= 0){
			SwitchToPage(startPageIndex);
		}
	}
	else{
		if (m_loginCompPtr.IsValid()){
			ShowLoginPage();
		}
		else{
			ShowHomePage();
		}
	}

	LoginControlButton->setVisible(m_loginCompPtr.IsValid());
	
	installEventFilter(this);

	SettingsButton->setVisible(m_settingsPageIndexAttrPtr.IsValid());
	LogButton->setVisible(m_logPageIndexAttrPtr.IsValid());

	UpdateLoginButtonsState();

	connect(&m_autoLogoutTimer, SIGNAL(timeout()), this, SLOT(Logout()));

	SetKeyboardCommandPath();

	if (m_defaultPageIndexAttrPtr.IsValid()){
		m_lastPageIndexForLoggedUser = *m_defaultPageIndexAttrPtr;
	}
}


void CThumbnailDecoratorGuiComp::OnGuiDestroyed()
{
	if (m_pagesWidgetCompPtr.IsValid() && m_pagesWidgetCompPtr->IsGuiCreated()){
		m_pagesWidgetCompPtr->DestroyGui();
	}

	if (m_leftMenuPanelGuiCompPtr.IsValid() && m_leftMenuPanelGuiCompPtr->IsGuiCreated()){
		m_leftMenuPanelGuiCompPtr->DestroyGui();
	}

	m_pageModelObserver.UnregisterAllModels();
	m_commandsObserver.UnregisterAllModels();
	m_loginObserver.UnregisterAllModels();
	m_pageVisualStatusObserver.UnregisterAllModels();

	BaseClass::OnGuiDestroyed();
}


void CThumbnailDecoratorGuiComp::OnGuiRetranslate()
{
	BaseClass::OnGuiRetranslate();

	int stackIndex = PageStack->currentIndex();
	if (stackIndex == LOGIN_PAGE_INDEX){
		CurrentPageLabel->setText(tr("Login"));
	}
	else if (stackIndex == HOME_PAGE_INDEX){
		CurrentPageLabel->setText(*m_welcomeTextAttrPtr);
	}
	else{
		Q_ASSERT(stackIndex == PAGE_CONTAINER_INDEX);
		if (m_pagesCompPtr.IsValid()){
			int selectedPageIndex = m_pagesCompPtr->GetSelectedOptionIndex();
			QString pageLabel;
			const iprm::IOptionsList* pageListPtr = m_pagesCompPtr->GetSelectionConstraints();
			if ((pageListPtr != NULL) && selectedPageIndex >= 0){
				pageLabel = pageListPtr->GetOptionName(selectedPageIndex);

				CurrentPageLabel->setText(pageLabel);
			}
		}
	}
}


// private slots

void CThumbnailDecoratorGuiComp::on_PageStack_currentChanged(int stackIndex)
{
	if (!UserEdit->text().isEmpty() && (stackIndex == LOGIN_PAGE_INDEX)){
		PasswordEdit->setFocus();
		PasswordEdit->setCursorPosition(0);
	}
}


void CThumbnailDecoratorGuiComp::on_PageList_clicked(const QModelIndex& index)
{
	QStandardItem* item = m_menuItemModel.itemFromIndex(index);
	bool pageExists = (item == nullptr) ? (false) : (item->data(CThumbPageItemGuiDelegate::DR_PAGE_ID).isValid());
	if (!pageExists){
		m_subPageItemMap.clear();
		SubPages->clear();

		CurrentPageLabel->clear();
		on_HomeButton_clicked();

		return;
	}

	if (m_itemInfoMap.contains(item)){
		ItemInfo& info = m_itemInfoMap[item];

		m_pagesCompPtr->SetSelectedOptionIndex(info.pageIndex);
	}
}


void CThumbnailDecoratorGuiComp::on_ExitButton_clicked()
{
	if (QMessageBox::question(GetWidget(), tr("Quit"), tr("Do you really want to quit?"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes){
		QWidget* topWidgetPtr = GetWidget();
		while (topWidgetPtr->parentWidget() != NULL){
			topWidgetPtr = topWidgetPtr->parentWidget();
		}

		if (topWidgetPtr != NULL){
			topWidgetPtr->close();

			qApp->quit();
		}
	}
}


void CThumbnailDecoratorGuiComp::on_SubPages_itemSelectionChanged()
{
	CurrentPageLabel->clear();

	QList<QTreeWidgetItem*> selectedItems = SubPages->selectedItems();
	if (!selectedItems.isEmpty()){
		QTreeWidgetItem* selectedItemPtr = selectedItems[0];

		QTreeWidgetItem* parentItemPtr = selectedItemPtr->parent();
		while (parentItemPtr != nullptr){
			if (m_subPageItemMap.contains(parentItemPtr)){
				ItemInfo& info = m_subPageItemMap[parentItemPtr];

				if (info.selectionPtr != nullptr){
					info.selectionPtr->SetSelectedOptionIndex(info.pageIndex);
				}
			}

			parentItemPtr = parentItemPtr->parent();
		}

		if (m_subPageItemMap.contains(selectedItemPtr)){
			ItemInfo& info = m_subPageItemMap[selectedItemPtr];

			if (info.selectionPtr != nullptr){
				info.selectionPtr->SetSelectedOptionIndex(info.pageIndex);
			}
		}

		CurrentPageLabel->setText(selectedItemPtr->text(0));
	}
}


void CThumbnailDecoratorGuiComp::on_HomeButton_clicked()
{
	ShowHomePage();
}


void CThumbnailDecoratorGuiComp::on_LoginButton_clicked()
{
	PasswordEdit->setFocus();
	if (m_loginCompPtr.IsValid()){
		QString userName = UserEdit->text();
		QString password = PasswordEdit->text();
		if (m_loginCompPtr->Login(userName, password)){
			int autoLogoutSeconds = GetAutoLogoutTime();
			if (autoLogoutSeconds > 0){
				m_autoLogoutTimer.start(autoLogoutSeconds * 1000);

				qApp->installEventFilter(this);
			}

			UpdateLoginButtonsState();

			int lastPageIndex = m_lastPageIndexForLoggedUser;

			// No page was selected:
			if (lastPageIndex < 0){
				ShowHomePage();
			}
			else{
				// Check if the page can be entered for the newly logged user:
				const iprm::IOptionsList* pageInfoListPtr = m_pagesCompPtr->GetSelectionConstraints();
				Q_ASSERT(pageInfoListPtr != nullptr);

				bool isPageEnabled = pageInfoListPtr->IsOptionEnabled(lastPageIndex);
	
				// If possible, go to the last active page:
				if (isPageEnabled){
					SwitchToPage(lastPageIndex);
				}
				// If not swtich to the default page or show the main menu:
				else{
					if (m_defaultPageIndexAttrPtr.IsValid()){
						SwitchToPage(*m_defaultPageIndexAttrPtr);
					}
					else{
						ShowHomePage();
					}
				}
			}
		}
		else{
			QMessageBox::warning(GetQtWidget(), tr("Error"), tr("Wrong password"));
			if (m_keyEnterTimerId != 0){
				killTimer(m_keyEnterTimerId);
			}
			m_keyEnterTimerId = startTimer(500);
		}
	}

	PasswordEdit->setText("");
}


void CThumbnailDecoratorGuiComp::on_LoginControlButton_clicked()
{
	Q_ASSERT(m_loginCompPtr.IsValid());

	if (m_loginCompPtr->GetLoggedUser() == nullptr){
		ShowLoginPage();
	}
	else{
		ProcessLogout();
	}
}


void CThumbnailDecoratorGuiComp::on_SettingsButton_clicked()
{
	Q_ASSERT(m_settingsPageIndexAttrPtr.IsValid());

	for (ItemInfoMap::Iterator itemIter = m_itemInfoMap.begin(); itemIter != m_itemInfoMap.end(); ++itemIter){
		int pageIndex = itemIter.value().pageIndex;
		if (pageIndex == *m_settingsPageIndexAttrPtr){
			QStandardItem* itemPtr = itemIter.key();

			on_PageList_clicked(itemPtr->index());
		}
	}
}


void CThumbnailDecoratorGuiComp::on_LogButton_clicked()
{
	Q_ASSERT(m_logPageIndexAttrPtr.IsValid());

	for (ItemInfoMap::Iterator itemIter = m_itemInfoMap.begin(); itemIter != m_itemInfoMap.end(); ++itemIter){
		int pageIndex = itemIter.value().pageIndex;
		if (pageIndex == *m_logPageIndexAttrPtr){
			QStandardItem* itemPtr = itemIter.key();

			on_PageList_clicked(itemPtr->index());
		}
	}
}


void CThumbnailDecoratorGuiComp::on_KeyboardButton_clicked()
{
	ShowKeyboard();
}


void CThumbnailDecoratorGuiComp::Logout()
{
	ProcessLogout();
}


void CThumbnailDecoratorGuiComp::OnVirtualKeyboardStateChanged(QProcess::ProcessState state)
{
	KeyboardButton->setEnabled(state == QProcess::NotRunning);
}


// private methods

void CThumbnailDecoratorGuiComp::ShowLoginPage()
{
	PageStack->setCurrentIndex(LOGIN_PAGE_INDEX);
	PageList->clearSelection();

	m_subPageItemMap.clear();
	SubPages->clear();

	if (m_pagesCompPtr.IsValid()){
		m_lastPageIndexForLoggedUser = m_pagesCompPtr->GetSelectedOptionIndex();

		m_pagesCompPtr->SetSelectedOptionIndex(-1);
	}

	LeftFrame->setVisible(false);

	UpdateLoginButtonsState();

	CurrentPageLabel->setText(tr("Login"));

	if (m_menuPanelVisibilityCompPtr.IsValid()){
		m_menuPanelVisibilityCompPtr->SetEnabled(false);
	}
}


void CThumbnailDecoratorGuiComp::ShowHomePage()
{
	if (m_loginCompPtr.IsValid()){
		iauth::CUser* userPtr = m_loginCompPtr->GetLoggedUser();
		if ((GetLoginMode() == LM_STRONG) && (userPtr == nullptr)){
			return;
		}
	}

	if (m_pagesCompPtr.IsValid()){
		m_pagesCompPtr->SetSelectedOptionIndex(-1);
	}

	PageStack->setCurrentIndex(HOME_PAGE_INDEX);
	PageList->clearSelection();
	
	m_subPageItemMap.clear();
	SubPages->clear();

	LeftFrame->setVisible(false);

	UpdateLoginButtonsState();

	CurrentPageLabel->setText(*m_welcomeTextAttrPtr);

	if (m_menuPanelVisibilityCompPtr.IsValid()){
		m_menuPanelVisibilityCompPtr->SetEnabled(true);
	}
}


void CThumbnailDecoratorGuiComp::SwitchToPage(int index)
{
	PageList->clearSelection();
	
	m_subPageItemMap.clear();
	SubPages->clear();

	LeftFrame->setVisible(false);

	if (m_pagesCompPtr.IsValid()){
		if (m_pagesCompPtr->SetSelectedOptionIndex(index)){
			QString pageLabel;
			const iprm::IOptionsList* pageListPtr = m_pagesCompPtr->GetSelectionConstraints();
			if ((pageListPtr != NULL) && index >= 0){
				pageLabel = pageListPtr->GetOptionName(index);
			}

			CurrentPageLabel->setText(pageLabel);

			const iprm::ISelectionParam* subSelectionPtr = m_pagesCompPtr->GetSubselection(index);
			if (subSelectionPtr != NULL){
				m_pageModelObserver.UnregisterModel(1);

				imod::IModel* subPageSelectionModelPtr = const_cast<imod::IModel*>(dynamic_cast<const imod::IModel*>(subSelectionPtr));
				if (subPageSelectionModelPtr != nullptr){
					m_pageModelObserver.RegisterModel(subPageSelectionModelPtr, 1);
				}

				CreateMenu(subSelectionPtr, NULL);
			}
			if (index < 0){
				PageStack->setCurrentIndex(HOME_PAGE_INDEX);
			}
			else{
				PageStack->setCurrentIndex(PAGE_CONTAINER_INDEX);
			}
		}
	}

	UpdateLoginButtonsState();

	HomeButton->setEnabled(true);

	if (m_menuPanelVisibilityCompPtr.IsValid()){
		m_menuPanelVisibilityCompPtr->SetEnabled(true);
	}
}


void CThumbnailDecoratorGuiComp::UpdateLoginButtonsState()
{
	bool hasCloseRight = true;
	if (m_rightsCompPtr.IsValid()){
		hasCloseRight = m_rightsCompPtr->HasRight(*m_closeRightIdAttrPtr, true);
		
		ExitButton->setEnabled(hasCloseRight);
	}

	bool isLogged = true;
	if (m_loginCompPtr.IsValid()){
		isLogged = (m_loginCompPtr->GetLoggedUser() != NULL);

		LoginButton->setEnabled(!isLogged);
		ExitButton->setEnabled(hasCloseRight && isLogged);

		LoginMode loginMode = GetLoginMode();
		if (loginMode == LM_STRONG){
			LoginControlButton->setEnabled(isLogged);

			HomeButton->setEnabled(isLogged);
		}
		else{
			LoginControlButton->setEnabled(PageStack->currentIndex() != LOGIN_PAGE_INDEX);

			LoginControlButton->setIcon(isLogged ? QIcon(":/Icons/Lock") : QIcon(":/Icons/Unlock"));
		}

		UserEdit->setEnabled(!isLogged);
		PasswordEdit->setEnabled(!isLogged);
		UserEdit->setEnabled(!isLogged);
		if (!isLogged){
			m_autoLogoutTimer.stop();

			qApp->removeEventFilter(this);
		}
	}

	SettingsButton->setEnabled(isLogged);
}


void CThumbnailDecoratorGuiComp::CreatePages(const iprm::ISelectionParam* selectionPtr)
{
	if (selectionPtr == nullptr){
		return;
	}

	m_itemInfoMap.clear();
	m_menuItemModel.clear();
	
	m_pageVisualStatusObserver.UnregisterAllModels();

	const iqtgui::IMultiVisualStatusProvider* visualStatusProviderPtr = dynamic_cast<const iqtgui::IMultiVisualStatusProvider*>(selectionPtr);

	const iprm::IOptionsList* itemsListPtr = selectionPtr->GetSelectionConstraints();
	int menuItemsCount = itemsListPtr->GetOptionsCount();

	SetLayoutProperties(menuItemsCount);

	m_menuItemModel.setRowCount(m_rowsCount);
	m_menuItemModel.setColumnCount(m_columnsCount);
	m_menuItemModel.setParent(this);

	for (int itemIndex = 0; itemIndex < menuItemsCount; ++itemIndex){
		QString itemName = itemsListPtr->GetOptionName(itemIndex);
		if (itemName.isEmpty()){
			itemName = tr("<unnamed>");
		}

		QByteArray menuItemId = itemsListPtr->GetOptionId(itemIndex);
		bool isItemEnabled = itemsListPtr->IsOptionEnabled(itemIndex);

		QStandardItem* menuItem = new QStandardItem(itemName);
		menuItem->setData(menuItemId, CThumbPageItemGuiDelegate::DR_PAGE_ID);
		menuItem->setData(isItemEnabled, CThumbPageItemGuiDelegate::DR_STATE);
		menuItem->setEnabled(isItemEnabled);

		if (visualStatusProviderPtr != nullptr){
			const iqtgui::IVisualStatus* statusPtr = visualStatusProviderPtr->GetVisualStatus(itemIndex);
			if (statusPtr != nullptr){
				menuItem->setIcon(statusPtr->GetStatusIcon());

				imod::IModel* statusModelPtr = dynamic_cast<imod::IModel*>(const_cast<iqtgui::IVisualStatus*>(statusPtr));
				if (statusModelPtr != nullptr){
					m_pageVisualStatusObserver.RegisterModel(statusModelPtr, itemIndex);
				}
			}
		}

		ItemInfo itemInfo;
		itemInfo.pageIndex = itemIndex;
		itemInfo.selectionPtr = const_cast<iprm::ISelectionParam*>(selectionPtr);
		m_itemInfoMap[menuItem] = itemInfo;

		int itemRow = itemIndex / m_columnsCount;
		int itemCol = itemIndex % m_columnsCount;
		m_menuItemModel.setItem(itemRow, itemCol, menuItem);
	}

	if (m_menuItemModel.columnCount() > 0){
		PageList->setModel(&m_menuItemModel);

		m_itemDelegate = new imtgui::CThumbPageItemGuiDelegate(m_menuItemModel, m_horizontalSpacing, m_verticalSpacing, this);

		PageList->setItemDelegate(m_itemDelegate);
	}

	PageList->resizeColumnsToContents();
	PageList->resizeRowsToContents();

	UpdateSpacing();
	UpdateMinSize();
}


void CThumbnailDecoratorGuiComp::CreateMenu(const iprm::ISelectionParam* selectionPtr, QTreeWidgetItem* parentItemPtr)
{
	m_subPageItemMap.clear();
	SubPages->clear();

	if (selectionPtr == nullptr){
		return;
	}

	const iprm::IOptionsList* pageListPtr = selectionPtr->GetSelectionConstraints();
	if (pageListPtr == nullptr){
		return;
	}

	int pagesCount = pageListPtr->GetOptionsCount();

	const iqtgui::IMultiVisualStatusProvider* visualStatusProviderPtr = dynamic_cast<const iqtgui::IMultiVisualStatusProvider*>(selectionPtr);

	int selectedIndex = selectionPtr->GetSelectedOptionIndex();

	for (int pageIndex = 0; pageIndex < pagesCount; ++pageIndex){
		QString pageName = pageListPtr->GetOptionName(pageIndex);
		if (pageName.isEmpty()){
			continue;
		}

		QByteArray pageId = pageListPtr->GetOptionId(pageIndex);

		QTreeWidgetItem* pageItem = new QTreeWidgetItem(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		pageItem->setText(0, pageName);
		pageItem->setData(0, CThumbPageItemGuiDelegate::DR_PAGE_ID, pageId);
		pageItem->setDisabled(!pageListPtr->IsOptionEnabled(pageIndex));

		if (visualStatusProviderPtr != nullptr){
			const iqtgui::IVisualStatus* statusPtr = visualStatusProviderPtr->GetVisualStatus(pageIndex);
			if (statusPtr != nullptr){
				pageItem->setIcon(0, statusPtr->GetStatusIcon());
			}
		}

		ItemInfo itemInfo;
		itemInfo.pageIndex = pageIndex;
		itemInfo.selectionPtr = const_cast<iprm::ISelectionParam*>(selectionPtr);
		m_subPageItemMap[pageItem] = itemInfo;

		if (parentItemPtr == nullptr){
			SubPages->addTopLevelItem(pageItem);
		}
		else{
			parentItemPtr->addChild(pageItem);
		}

		if ((parentItemPtr == nullptr) && (pageIndex == selectedIndex)){
			pageItem->setSelected(true);
		}

		iprm::ISelectionParam* subSelectionPtr = selectionPtr->GetSubselection(pageIndex);
		if (subSelectionPtr != nullptr){
			CreateMenu(subSelectionPtr, pageItem);
		}
	}

	bool hidePageListView = false;

	if (m_pagesCompPtr.IsValid() && m_disablePageListViewIndexesAttrPtr.IsValid()){
		int topLevelPageIndex = m_pagesCompPtr->GetSelectedOptionIndex();
		if ((topLevelPageIndex >= 0) && (m_disablePageListViewIndexesAttrPtr.FindValue(topLevelPageIndex) >= 0)){
			hidePageListView = true;
		}
	}

	LeftFrame->setVisible(!m_leftMenuPanelGuiCompPtr.IsValid() && !hidePageListView && (SubPages->topLevelItemCount() > 0));
}


void CThumbnailDecoratorGuiComp::UpdatePageState()
{
	if (m_pagesCompPtr.IsValid()){
		const iprm::IOptionsList* pageListPtr = m_pagesCompPtr->GetSelectionConstraints();
		int pagesCount = pageListPtr->GetOptionsCount();
		int selectedPageIndex = m_pagesCompPtr->GetSelectedOptionIndex();

		const iqtgui::IMultiVisualStatusProvider* pageVisualStatusProviderPtr = dynamic_cast<const iqtgui::IMultiVisualStatusProvider*>(m_pagesCompPtr.GetPtr());

		for (ItemInfoMap::Iterator itemIter = m_itemInfoMap.begin(); itemIter != m_itemInfoMap.end(); ++itemIter){
			int pageIndex = itemIter.value().pageIndex;
			if (pageIndex < pagesCount){
				QStandardItem* itemPtr = itemIter.key();

				QByteArray pageId = pageListPtr->GetOptionId(pageIndex);
				QByteArray itemId = itemPtr->data(CThumbPageItemGuiDelegate::DR_PAGE_ID).toByteArray();
				if (itemId == pageId){
					QString itemName = pageListPtr->GetOptionName(pageIndex);
					if (itemName.isEmpty()){
						itemName = tr("<unnamed>");
					}

					bool isItemEnabled = pageListPtr->IsOptionEnabled(pageIndex);
					itemPtr->setEnabled(isItemEnabled);
					itemPtr->setData(isItemEnabled, CThumbPageItemGuiDelegate::DR_STATE);
					itemPtr->setText(itemName);

					if (pageVisualStatusProviderPtr != nullptr){
						const iqtgui::IVisualStatus* pageStatusPtr = pageVisualStatusProviderPtr->GetVisualStatus(pageIndex);
						if (pageStatusPtr != nullptr){
							itemPtr->setIcon(pageStatusPtr->GetStatusIcon());
						}

						if (m_logPageIndexAttrPtr.IsValid() && (pageIndex == *m_logPageIndexAttrPtr)){
							LogButton->setIcon(pageStatusPtr->GetStatusIcon());
						}
					}
				}
			}

			if (selectedPageIndex >= 0){
				iprm::ISelectionParam* subMenuPtr = m_pagesCompPtr->GetSubselection(selectedPageIndex);
				if (subMenuPtr != NULL){
					const iqtgui::IMultiVisualStatusProvider* subPageVisualStatusProviderPtr = dynamic_cast<const iqtgui::IMultiVisualStatusProvider*>(subMenuPtr);

					const iprm::IOptionsList* subPageListPtr = subMenuPtr->GetSelectionConstraints();
					if (subPageListPtr != NULL){
						for (MenuItemInfoMap::Iterator subPageIter = m_subPageItemMap.begin(); subPageIter != m_subPageItemMap.end(); ++subPageIter){
							int subPageIndex = subPageIter.value().pageIndex;

							bool isPageEnabled = subPageListPtr->IsOptionEnabled(subPageIndex);

							subPageIter.key()->setDisabled(!isPageEnabled);

							if (subPageVisualStatusProviderPtr != nullptr){
								const iqtgui::IVisualStatus* subPageStatusPtr = subPageVisualStatusProviderPtr->GetVisualStatus(subPageIndex);
								if (subPageStatusPtr != nullptr){
									subPageIter.key()->setIcon(0, subPageStatusPtr->GetStatusIcon());
								}
							}
						}
					}
				}
			}
		}
	}
}


CThumbnailDecoratorGuiComp::LoginMode CThumbnailDecoratorGuiComp::GetLoginMode()
{
	if (m_defaultPageIndexAttrPtr.IsValid()){
		return LM_DEFAULT;
	}

	return LM_STRONG;
}


int CThumbnailDecoratorGuiComp::GetAutoLogoutTime() const
{
	int retVal = 0;
	if (m_autoLogoutMinutesAttrPtr.IsValid()){
		Q_ASSERT(*m_autoLogoutMinutesAttrPtr > 0);

		retVal = *m_autoLogoutMinutesAttrPtr * 60;
	}

	if (m_autoLogoutMinutesCompPtr.IsValid()){
		Q_ASSERT(m_autoLogoutMinutesCompPtr->GetValues().GetElementsCount() == 1);

		retVal = m_autoLogoutMinutesCompPtr->GetValues()[0] * 60;
	}

	return retVal;
}


void CThumbnailDecoratorGuiComp::ProcessLogout()
{
	if (m_loginCompPtr->Logout()){
		m_autoLogoutTimer.stop();

		qApp->removeEventFilter(this);

		UpdateLoginButtonsState();

		LoginMode loginMode = GetLoginMode();
		if (loginMode == LM_STRONG){
			ShowLoginPage();
		}
		else {
			if (m_defaultPageIndexAttrPtr.IsValid()){
				SwitchToPage(*m_defaultPageIndexAttrPtr);
			}
			else {
				ShowHomePage();
			}
		}
	}

}

void CThumbnailDecoratorGuiComp::GetMenuLayout(const int count)
{
	if (m_columnsCount <= 0 || m_rowsCount <= 0){
		m_columnsCount = m_horizontalItemsViewAttrPtr.IsValid() ? qMax(0, *m_horizontalItemsViewAttrPtr) : 0;
		m_rowsCount = m_verticalItemsViewAttrPtr.IsValid() ? qMax(0, *m_verticalItemsViewAttrPtr) : 0;
	}

	bool ifOneSet = (m_columnsCount > 0) || (m_rowsCount > 0);
	bool ifFits = ifOneSet && (m_columnsCount * m_rowsCount == count);

	if (m_columnsCount <= 0 && m_rowsCount <= 0 || (!ifOneSet && !ifFits)){
		int columns = (int)sqrt((double)count);
		m_columnsCount = qMax(1, columns);
		m_rowsCount = count / m_columnsCount;

		if ((count % m_columnsCount) > 0){
			m_rowsCount++;
		}
	}
	else if (m_rowsCount <= 0){
		m_rowsCount = count / m_columnsCount;
		if ((count % m_columnsCount) > 0){
			m_rowsCount++;
		}
	}
	else if (m_columnsCount <= 0){
		m_columnsCount = count / m_rowsCount;
		if ((count % m_rowsCount) > 0){
			m_columnsCount++;
		}
	}
}


void CThumbnailDecoratorGuiComp::SetLayoutProperties(const int count)
{
	QSize size = StartPage->size();
	UpdateSettings(count);

	static const int spacing = 10;
	const QSize gridSize = QSize(size.width() / m_columnsCount, size.height() / m_rowsCount);
	PageList->setIconSize(gridSize);
	PageList->setShowGrid(false);
	PageList->horizontalHeader()->hide();
	PageList->verticalHeader()->hide();
	PageList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	PageList->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}


void CThumbnailDecoratorGuiComp::UpdateSettings(const int count)
{
	GetMenuLayout(count);
	UpdateSpacing();
	UpdateMargins();
	UpdateMaxSize();
	UpdateMinSize();
}


void CThumbnailDecoratorGuiComp::UpdateSpacing()
{
	m_horizontalSpacing = m_horizontalSpacingAttrPtr.IsValid() ? qMax(0, *m_horizontalSpacingAttrPtr) : s_minSpacing;
	m_verticalSpacing = m_verticalSpacingAttrPtr.IsValid() ? qMax(0, *m_verticalSpacingAttrPtr) : s_minSpacing;

	//check constraints
	QSize currentSize = PageList->size();

	if (m_itemDelegate != nullptr){
		QStyleOptionViewItem option;
		option.font = PageList->font();
		m_minItemSize = m_itemDelegate->sizeHint(option, QModelIndex());
	}

	int emptySpaceH = currentSize.width() - (m_columnsCount * m_minItemSize.width());
	int emptySpaceV = currentSize.height() - (m_rowsCount * m_minItemSize.height());

	int maxPossibleSpacingH = (m_columnsCount > 1) ? (float(emptySpaceH) / (float(m_columnsCount - 1))) : -1;
	int maxPossibleSpacingV = (m_rowsCount > 1) ? (float(emptySpaceV) / (float(m_rowsCount - 1))) : -1;

	m_horizontalSpacing = (m_horizontalSpacing > maxPossibleSpacingH && maxPossibleSpacingH > 0) ?
		maxPossibleSpacingH : m_horizontalSpacing;
	m_verticalSpacing = (m_verticalSpacing > maxPossibleSpacingV && maxPossibleSpacingV > 0) ?
		maxPossibleSpacingV : m_verticalSpacing;

	if (m_itemDelegate != nullptr){
		m_itemDelegate->SetMargins(m_horizontalSpacing/2, m_verticalSpacing/2);
	}
}


void CThumbnailDecoratorGuiComp::UpdateMargins()
{
	if (m_horizontalFrameMargin <= 0){
		m_horizontalFrameMargin = m_horizontalFrameMarginAttrPtr.IsValid() ? qMax(0, *m_horizontalFrameMarginAttrPtr) : 15;
		m_verticalFrameMargin = m_verticalFrameMarginAttrPtr.IsValid() ? qMax(0, *m_verticalFrameMarginAttrPtr) : 15;
	}
	//TODO check constraints
	hSpacerLeft->changeSize(m_horizontalFrameMargin, 20);
	hSpacerRight->changeSize(m_horizontalFrameMargin, 20);
	vSpacerTop->changeSize(20, m_verticalFrameMargin);
	vSpacerBottom->changeSize(20, m_verticalFrameMargin);
}


void CThumbnailDecoratorGuiComp::UpdateMaxSize()
{
	if (m_maxWidth <= 0 || m_maxHeight <= 0){
		m_maxWidth = m_maximumFrameWidthAttrPtr.IsValid() ? qMax(0, *m_maximumFrameWidthAttrPtr) : 800;
		m_maxHeight = m_maximumFrameHeightAttrPtr.IsValid() ? qMax(0, *m_maximumFrameHeightAttrPtr) : 600;
	}

	PageList->setMaximumSize(QSize(m_maxWidth, m_maxHeight));
}


void CThumbnailDecoratorGuiComp::UpdateMinSize()
{
	if (m_itemDelegate != nullptr){
		QStyleOptionViewItem option;
		option.font = PageList->font();
		m_minItemSize = m_itemDelegate->sizeHint(option, QModelIndex());
	}

	int minWidth = m_columnsCount * m_minItemSize.width() + (m_columnsCount - 1) * s_minSpacing;
	int minHeight = m_rowsCount * m_minItemSize.height() + (m_rowsCount - 1) * s_minSpacing;

	PageList->setMinimumSize(minWidth, minHeight);
}


void CThumbnailDecoratorGuiComp::UpdateCommands()
{
	const iqtgui::CHierarchicalCommand* commandsPtr = nullptr;
	if (m_commandsProviderCompPtr.IsValid()){
		if (m_mainToolBar != nullptr){
			m_mainToolBar->clear();
	
			commandsPtr = dynamic_cast<const iqtgui::CHierarchicalCommand*>(m_commandsProviderCompPtr->GetCommands());
			if (commandsPtr != nullptr){
				iqtgui::CCommandTools::SetupToolbar(*commandsPtr, *m_mainToolBar);
			}
			else{
				QAction* placeholderAction = new QAction(QIcon("/"), "");
				placeholderAction->setEnabled(false);

				m_mainToolBar->addAction(placeholderAction);
				m_mainToolBar->addSeparator();
			}
		}
	}
}


// private methods

void CThumbnailDecoratorGuiComp::ShowKeyboard()
{
	HideKeyboard();

	m_keyboardProcessPtr.SetPtr(new QProcess(this));

	connect(	m_keyboardProcessPtr.GetPtr(),
				SIGNAL(stateChanged(QProcess::ProcessState)),
				this,
				SLOT(OnVirtualKeyboardStateChanged(QProcess::ProcessState)));

	m_keyboardProcessPtr->setProgram(m_winKeyboardPath);

	m_keyboardProcessPtr->startDetached();
}


void CThumbnailDecoratorGuiComp::HideKeyboard()
{
	if (m_keyboardProcessPtr != nullptr){
		m_keyboardProcessPtr->terminate();
		m_keyboardProcessPtr->waitForFinished(2000);
		if (!m_keyboardProcessPtr->isOpen()){
			m_keyboardProcessPtr->kill();
		}
		m_keyboardProcessPtr.Reset();
	}
}


void CThumbnailDecoratorGuiComp::SetKeyboardCommandPath()
{
	QString processPath;

	KeyboardButton->setVisible(false);

	if (m_winKeyboardPath.isEmpty()){
		processPath = getenv("SystemRoot");
		if (processPath.isEmpty()){
			processPath = "C:\\Windows";
		}

		processPath += "\\System32\\osk.exe";

		QFileInfo processFileInfo(processPath);
		if (processFileInfo.exists()){
			m_winKeyboardPath = processPath;

			KeyboardButton->setVisible(true);
		}
	}
}


// public methods of embedded class CommandsObserver

CThumbnailDecoratorGuiComp::CommandsObserver::CommandsObserver(CThumbnailDecoratorGuiComp& parent)
	:m_parent(parent)
{
}


// protected methods of embedded class CommandsObserver

// reimplemented (imod::CMultiModelDispatcherBase)

void CThumbnailDecoratorGuiComp::CommandsObserver::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_parent.UpdateCommands();
}


// public methods of embedded class PageModelObserver

CThumbnailDecoratorGuiComp::PageModelObserver::PageModelObserver(CThumbnailDecoratorGuiComp& parent)
	:m_parent(parent)
{
}


// protected methods of embedded class PageModelObserver

// reimplemented (imod::CMultiModelDispatcherBase)

void CThumbnailDecoratorGuiComp::PageModelObserver::OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet)
{
	if (changeSet.Contains(iprm::ISelectionParam::CF_SELECTION_CHANGED)){
		Q_ASSERT(m_parent.m_pagesCompPtr.IsValid());

		if (modelId == 0){
			m_parent.SwitchToPage(m_parent.m_pagesCompPtr->GetSelectedOptionIndex());
		}
	}

	m_parent.UpdatePageState();
}


// protected methods of embedded class LoginObserver

CThumbnailDecoratorGuiComp::LoginObserver::LoginObserver(CThumbnailDecoratorGuiComp& parent)
	:m_parent(parent)
{
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CThumbnailDecoratorGuiComp::LoginObserver::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	if (m_parent.IsGuiCreated()){
		QWidget* rootWidgetPtr = m_parent.GetWidget();
		Q_ASSERT(rootWidgetPtr != nullptr);

		QList<QDialog*> openDialogs = rootWidgetPtr->findChildren<QDialog*>();
		for (QDialog* dialogPtr : openDialogs){
			dialogPtr->reject();
		}
	}

	m_parent.UpdateLoginButtonsState();
}


// protected methods of embedded class PageStatusObserver

CThumbnailDecoratorGuiComp::PageVisualStatusObserver::PageVisualStatusObserver(CThumbnailDecoratorGuiComp& parent)
	:m_parent(parent)
{
}


// reimplemented (imod::CMultiModelDispatcherBase)

void CThumbnailDecoratorGuiComp::PageVisualStatusObserver::OnModelChanged(int /*modelId*/, const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	m_parent.UpdatePageState();
}


} // namespace imtgui


