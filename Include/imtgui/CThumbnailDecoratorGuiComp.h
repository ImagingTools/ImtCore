#pragma once


// Qt includes
#include <QtCore/QMap>
#include <QtCore/QTimer>
#include <QtCore/QProcess>
#include <QtWidgets/QListView>
#include <QtWidgets/QMenu>
#include <QtGui/QStandardItemModel>
#include <QtGui/QStandardItem>

// ACF includes
#include <imod/CMultiModelDispatcherBase.h>
#include <ilog/IMessageConsumer.h>
#include <iqtgui/TDesignerGuiCompBase.h>
#include <ibase/ICommandsProvider.h>
#include <iprm/ISelectionParam.h>
#include <iqtgui/CHierarchicalCommand.h>
#include <iqtgui/TRestorableGuiWrap.h>
#include <imeas/INumericValue.h>
#include <iauth/ILogin.h>
#include <iauth/IRightsProvider.h>

// ImtCore includes
#include <imtgui/IWidgetProvider.h>
#include <imtgui/IPageNavigationController.h>
#include <imtgui/CThumbPageItemGuiDelegate.h>
#include <GeneratedFiles/imtgui/ui_CThumbnailDecoratorGuiComp.h>


class QToolBar;

namespace imtbase
{
	class CCollectionInfo;
}


namespace imtgui
{


class CThumbnailDecoratorGuiCompAttr:
			public iqtgui::TRestorableGuiWrap<iqtgui::TDesignerGuiCompBase<Ui::CThumbnailDecoratorGuiComp>>
{
public:
	typedef iqtgui::TRestorableGuiWrap<
				iqtgui::TDesignerGuiCompBase<Ui::CThumbnailDecoratorGuiComp>> BaseClass;

	I_BEGIN_BASE_COMPONENT(CThumbnailDecoratorGuiCompAttr);
		I_ASSIGN(m_horizontalItemsViewAttrPtr, "HorizontalItemsView", "Count of visual items in horizontal row", false, 1);
		I_ASSIGN(m_verticalItemsViewAttrPtr, "VerticalItemsView", "Count of visual items in vertical column", false, 1);
		I_ASSIGN(m_horizontalSpacingAttrPtr, "HorizontalSpacing", "Horizontal spacing of items on view", false, 15);
		I_ASSIGN(m_verticalSpacingAttrPtr, "VerticalSpacing", "Vertical spacing of items on view", false, 15);
		I_ASSIGN(m_horizontalFrameMarginAttrPtr, "HorizontalFrameMargin", "Horizontal (left&right) side margin of thumbnail frame", false, 15);
		I_ASSIGN(m_verticalFrameMarginAttrPtr, "VerticalFrameMargin", "Vertical (top&bottom) side margin of thumbnail frame", false, 15);
		I_ASSIGN(m_maximumFrameWidthAttrPtr, "MaximumFrameWidth", "Maximum width of thumbnail frame", false, 320);
		I_ASSIGN(m_maximumFrameHeightAttrPtr, "MaximumFrameHeight", "Maximum height of thumbnail frame", false, 240);
		I_ASSIGN(m_autoLogoutMinutesAttrPtr, "AutoLogoutMinutes", "Time intervall for the automatic logout", false, 60);
		I_ASSIGN(m_defaultPageIndexAttrPtr, "DefaultPageIndex", "Index of the default (start) page", false, -1);
		I_ASSIGN(m_welcomeTextAttrPtr, "WelcomeText", "Welcome text on the start page", true, "Welcome");
		I_ASSIGN(m_closeRightIdAttrPtr, "CloseRightId", "ID of the user right to close the application", true, "Close");
		I_ASSIGN_MULTI_0(m_disablePageListViewIndexesAttrPtr, "DisablePageListViewIndexes", "List of page indexes where the page list view is not shown", false);
		I_ASSIGN(m_showPageTitlesAttrPtr, "ShowPageTitles", "Show title of the current page in the main tool bar", true, true);
		I_ASSIGN(m_loginGuiCompPtr, "LoginGui", "Login gui", false, "StandardLoginGui");
	I_END_COMPONENT;

protected:
	I_ATTR(int, m_horizontalItemsViewAttrPtr);
	I_ATTR(int, m_verticalItemsViewAttrPtr);
	I_ATTR(int, m_horizontalSpacingAttrPtr);
	I_ATTR(int, m_verticalSpacingAttrPtr);
	I_ATTR(int, m_horizontalFrameMarginAttrPtr);
	I_ATTR(int, m_verticalFrameMarginAttrPtr);
	I_ATTR(int, m_maximumFrameWidthAttrPtr);
	I_ATTR(int, m_maximumFrameHeightAttrPtr);
	I_ATTR(int, m_autoLogoutMinutesAttrPtr);
	I_ATTR(int, m_defaultPageIndexAttrPtr);
	I_TEXTATTR(m_welcomeTextAttrPtr);
	I_ATTR(QByteArray, m_closeRightIdAttrPtr);
	I_MULTIATTR(int, m_disablePageListViewIndexesAttrPtr);
	I_ATTR(bool, m_showPageTitlesAttrPtr);
	I_REF(iqtgui::IGuiObject, m_loginGuiCompPtr);
};


class CThumbnailDecoratorGuiCompAttr2: public CThumbnailDecoratorGuiCompAttr
{
public:
	typedef CThumbnailDecoratorGuiCompAttr BaseClass;

	I_BEGIN_BASE_COMPONENT(CThumbnailDecoratorGuiCompAttr2);
		I_ASSIGN(m_hideMenuPanelOnHomePageAttrPtr, "HideMenuPanelOnHomePage", "Hide menu panel when home page is active", true, true);
		I_ASSIGN(m_hideHomeButtonAttrPtr, "HideHomeButton", "Hide home button", true, false);
		I_ASSIGN(m_accountMenuCompPtr, "AccountMenu", "Menu for logged account", false, "AccountMenu");
		I_ASSIGN(m_viewPageNavigationAttrPtr, "ViewPageNavigation", "View page navigation", true, true);
		I_ASSIGN(m_quitDialogIgnoredAttrPtr, "QuitDialogIgnored", "If true, exit application without dialog", true, false);
		I_ASSIGN(m_additionalCommandsProviderCompPtr, "AdditionalCommands", "Additional tool commands showed on the right side of the main tool bar", false, "AdditionalCommands");
		I_ASSIGN_TO(m_additionalCommandsProviderModelCompPtr, m_additionalCommandsProviderCompPtr, false);
		I_ASSIGN(m_rightsCommandsCompPtr, "RightsCommands", "Additional tool commands showed on the right side of the login button", false, "RightsCommands");
	I_END_COMPONENT;

protected:
	I_ATTR(bool, m_hideMenuPanelOnHomePageAttrPtr);
	I_ATTR(bool, m_hideHomeButtonAttrPtr);
	I_REF(iqtgui::IGuiObject, m_accountMenuCompPtr);
	I_ATTR(bool, m_viewPageNavigationAttrPtr);
	I_ATTR(bool, m_quitDialogIgnoredAttrPtr);
	I_REF(ibase::ICommandsProvider, m_additionalCommandsProviderCompPtr);
	I_REF(imod::IModel, m_additionalCommandsProviderModelCompPtr);
	I_REF(ibase::ICommandsProvider, m_rightsCommandsCompPtr);
};


class CThumbnailDecoratorGuiComp:
			public CThumbnailDecoratorGuiCompAttr2,
			public virtual ibase::ICommandsProvider,
			virtual public IWidgetProvider
{
	Q_OBJECT

public:
	typedef CThumbnailDecoratorGuiCompAttr2 BaseClass;

	I_BEGIN_COMPONENT(CThumbnailDecoratorGuiComp);
		I_REGISTER_INTERFACE(ibase::ICommandsProvider);
		I_REGISTER_INTERFACE(IWidgetProvider);
		I_ASSIGN(m_menuPanelVisibilityCompPtr, "MenuPanelVisibility", "Menu panel visibility status", false, "MenuPanelVisibility");
		I_ASSIGN(m_pagesWidgetCompPtr, "PageUiContainer", "UI component containing all application pages", true, "PageUiContainer");
		I_ASSIGN(m_pagesCompPtr, "PageModel", "Data model describing the used pages", true, "PageModel");
		I_ASSIGN_TO(m_pagesModelCompPtr, m_pagesCompPtr, true);
		I_ASSIGN(m_commandsProviderCompPtr, "Commands", "Provider of the commands showed in the main tool bar", false, "Commands");
		I_ASSIGN_TO(m_commandsProviderModelCompPtr, m_commandsProviderCompPtr, false);
		I_ASSIGN(m_loginCompPtr, "Login", "Login logic component", false, "Login");
		I_ASSIGN_TO(m_loginModelCompPtr, m_loginCompPtr, false);
		I_ASSIGN(m_rightsCompPtr, "Rights", "User rights for the application", false, "Rights");
		I_ASSIGN_TO(m_rightsModelCompPtr, m_rightsCompPtr, false);
		I_ASSIGN(m_autoLogoutMinutesCompPtr, "AutoLogoutTime", "Parameter to control automatical logout interval. Setting of this parameter overrides 'AutoLogoutMinutes' value", false, "AutoLogoutTime");
		I_ASSIGN(m_leftMenuPanelGuiCompPtr, "LeftMenuPanel", "Left menu panel", false, "LeftMenuPanel");
		I_ASSIGN(m_pageNavigationControllerCompPtr, "NavigationController", "Page navigation controller", false, "NavigationController");
		I_ASSIGN(m_dashboardGuiCompPtr, "Dashboard", "Dashboard UI", false, "Dashboard");
	I_END_COMPONENT;

	enum
	{
		LOGIN_PAGE_INDEX,
		HOME_PAGE_INDEX,
		PAGE_CONTAINER_INDEX
	};

	enum LoginMode
	{
		/**
			The user must be logged in to see any page of the application
		*/
		LM_STRONG,

		/**
			The unlogged user can see the application's menu and navigate over all accessible pages.
		*/
		LM_DEFAULT
	};

	enum UserAction
	{
		UA_APPLICATION_EXIT = 0,
		UA_LOGIN_CONTROL_ENABLED,
		UA_LOGIN_ENABLED,
		UA_HOME_ENABLED,
	};

	CThumbnailDecoratorGuiComp();

	// reimplemented (imtgui::IWidgetProvider)
	virtual const imtbase::ICollectionInfo* GetWidgetList() const override;
	virtual QWidget* GetWidgetPtr(const QByteArray& widgetId) const override;

	// reimplemented (ibase::ICommandsProvider)
	virtual const ibase::IHierarchicalCommand* GetCommands() const override;

	// reimplemented (QObject)
	virtual bool eventFilter(QObject* watched, QEvent* event) override;

protected:
	// reimplemented (iqtgui::TRestorableGuiWrap)
	virtual void OnRestoreSettings(const QSettings& settings) override;
	virtual void OnSaveSettings(QSettings& settings) const override;

	// reimplemented (iqtgui::CGuiComponentBase)
	virtual void OnGuiCreated() override;
	virtual void OnGuiDestroyed() override;
	virtual void OnGuiRetranslate() override;
	virtual void OnTryClose(bool* ignoredPtr = nullptr) override;
	virtual void OnGuiDesignChanged();

private Q_SLOTS:
	void on_PageList_clicked(const QModelIndex& index);
	void on_SubPages_itemSelectionChanged();
	void on_HomeButton_clicked();
	void on_LoginControlButton_clicked();
	void on_CommandsMenuButton_clicked();
	void OnAutoLogoutTimer();
	void OnCheckIsFullScreenTimer();
	void on_BackPageButton_clicked();
	void on_NextPageButton_clicked();
	void on_DashboardButton_clicked();

private:
	void ShowLoginPage();
	void ShowHomePage();
	void SwitchToPage(int index);
	void UpdateLoginButtonsState();
	void CreatePages(const iprm::ISelectionParam* selectionPtr);
	void CreateMenu(const iprm::ISelectionParam* selectionPtr, QTreeWidgetItem* parentItemPtr);
	void UpdatePageState();
	LoginMode GetLoginMode();
	int GetAutoLogoutTime() const;
	void ProcessLogout();
	bool IsUserActionAllowed(UserAction action);
	void ExitApplication();
	bool IsHomePageEnabled() const;

	/**
		Calculate layout of page thumbnails based on m_horizontalItemsViewAttrPtr and m_verticalItemsViewAttrPtr
		if the attributes are not set, or set incorrectly, the dimentions close to sqrt(totalcounr) are taken with
		the preferred vertical.
	*/
	void GetMenuLayout(const int count);
	void SetLayoutProperties(const int count);
	void UpdateSettings(const int count);
	void UpdateSpacing();
	void UpdateMargins();
	void UpdateMaxSize();
	void UpdateMinSize();
	void UpdateCommands();
	void UpdateAdditionalCommands();
	int SetupCommandsMenu(const iqtgui::CHierarchicalCommand& command, QMenu& result, int& prevGroupId);
	void UpdateMenuVisibility();
private:
	struct ItemInfo
	{
		ItemInfo()
			:selectionPtr(nullptr),
			pageIndex(-1)
		{
		}

		iprm::ISelectionParam* selectionPtr;
		int pageIndex;
	};

	typedef QMap<QStandardItem*, ItemInfo> ItemInfoMap;
	ItemInfoMap m_itemInfoMap;

	typedef QMap<QTreeWidgetItem*, ItemInfo> MenuItemInfoMap;
	MenuItemInfoMap m_subPageItemMap;


	class CommandsObserver: public imod::CMultiModelDispatcherBase
	{
	public:
		typedef imod::CMultiModelDispatcherBase BaseClass;

		explicit CommandsObserver(CThumbnailDecoratorGuiComp& parent);

		// reimplemented (imod::CMultiModelDispatcherBase)
		void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CThumbnailDecoratorGuiComp& m_parent;
	};

	class AdditionalCommandsObserver: public imod::CMultiModelDispatcherBase
	{
	public:
		typedef imod::CMultiModelDispatcherBase BaseClass;

		explicit AdditionalCommandsObserver(CThumbnailDecoratorGuiComp& parent);

		// reimplemented (imod::CMultiModelDispatcherBase)
		void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CThumbnailDecoratorGuiComp& m_parent;
	};


	class PageModelObserver: public imod::CMultiModelDispatcherBase
	{
	public:
		explicit PageModelObserver(CThumbnailDecoratorGuiComp& parent);

		// reimplemented (imod::CMultiModelDispatcherBase)
		void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CThumbnailDecoratorGuiComp& m_parent;
	};


	class LoginObserver: public imod::CMultiModelDispatcherBase
	{
	public:
		explicit LoginObserver(CThumbnailDecoratorGuiComp& parent);

		// reimplemented (imod::CMultiModelDispatcherBase)
		void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CThumbnailDecoratorGuiComp& m_parent;
	};


	class PageVisualStatusObserver: public imod::CMultiModelDispatcherBase
	{
	public:
		explicit PageVisualStatusObserver(CThumbnailDecoratorGuiComp& parent);

		// reimplemented (imod::CMultiModelDispatcherBase)
		void OnModelChanged(int modelId, const istd::IChangeable::ChangeSet& changeSet) override;

	private:
		CThumbnailDecoratorGuiComp& m_parent;
	};


	CommandsObserver m_commandsObserver;
	AdditionalCommandsObserver m_additionalCommandsObserver;
	PageModelObserver m_pageModelObserver;
	LoginObserver m_loginObserver;
	PageVisualStatusObserver m_pageVisualStatusObserver;

	QStandardItemModel m_menuItemModel;
	imtbase::CCollectionInfo* m_widgetList;

	// commands
	iqtgui::CHierarchicalCommand m_rootCommands;
	iqtgui::CHierarchicalCommand m_commands;
	int m_columnsCount;
	int m_rowsCount;
	int m_horizontalSpacing;
	int m_verticalSpacing;
	int m_horizontalFrameMargin;
	int m_verticalFrameMargin;
	int m_maxWidth;
	int m_maxHeight;
	QSize m_minItemSize;
	imtgui::CThumbPageItemGuiDelegate* m_itemDelegate;

	I_REF(iprm::IEnableableParam, m_menuPanelVisibilityCompPtr);
	I_REF(iqtgui::IGuiObject, m_pagesWidgetCompPtr);
	I_REF(iprm::ISelectionParam, m_pagesCompPtr);
	I_REF(imod::IModel, m_pagesModelCompPtr);
	I_REF(ibase::ICommandsProvider, m_commandsProviderCompPtr);
	I_REF(imod::IModel, m_commandsProviderModelCompPtr);
	I_REF(iauth::ILogin, m_loginCompPtr);
	I_REF(imod::IModel, m_loginModelCompPtr);
	I_REF(iauth::IRightsProvider, m_rightsCompPtr);
	I_REF(imod::IModel, m_rightsModelCompPtr);
	I_REF(imeas::INumericValue, m_autoLogoutMinutesCompPtr);
	I_REF(iqtgui::IGuiObject, m_leftMenuPanelGuiCompPtr);
	I_REF(imtgui::IPageNavigationController, m_pageNavigationControllerCompPtr);
	I_REF(iqtgui::IGuiObject, m_dashboardGuiCompPtr);

	QToolBar* m_mainToolBar;
	QToolBar* m_additionalCommandsToolBar;
	QToolBar* m_rightsCommandsToolBar;
	QTimer m_autoLogoutTimer;
	QTimer m_checkIsFullScreenTimer;

	QMenu m_commandsMenu;

	istd::TDelPtr<QProcess> m_keyboardProcessPtr;
	QString m_winKeyboardPath;

	int m_lastPageIndexForLoggedUser;
	bool m_isExitProcess;

	QSet<int> m_supportedEvents;
};


} // namespace imtgui



