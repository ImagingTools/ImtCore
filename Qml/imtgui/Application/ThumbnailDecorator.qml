import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtauthgui 1.0
import imtguigql 1.0
import imtdocgui 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtbaseImtBaseTypesSdl 1.0

Rectangle {
	id: thumbnailDecoratorContainer;
	
	anchors.fill: parent;
	
	color: Style.backgroundColor;
	
	property alias drawingContainer: drawingContainer_;
	property alias stackView: stackView_;
	property alias topPanel: topPanelLoader_.item;
	
	property int mainMargin: Style.mainWindowMargin !== undefined ? Style.mainWindowMargin : 0;
	property int pageMargin: Style.pageMargin !== undefined ? Style.pageMargin : 0;
	property int mainRadius: 0;
	
	property Item dialogManager: ModalDialogManager;
	
	property alias menuPanelRadius: menuPanel.radius;
	property alias loadPageByClick: pagesManager.loadByClick;
	property bool canRecoveryPassword: true;

	// Applications that decorate TopPanel themselves keep it; the ones that opt out
	// hand its duties to the navigation rail and get the whole height for content.
	readonly property bool topPanelVisible: Style.enableTopPanel !== undefined ? Style.enableTopPanel : true;

	property SettingsController settingsController: SettingsController {}
	
	Component.onCompleted: {
		Events.subscribeEvent("StartLoading", thumbnailDecoratorContainer.startLoading);
		Events.subscribeEvent("StopLoading", thumbnailDecoratorContainer.stopLoading);
		Events.subscribeEvent("ShowPreferencePage", thumbnailDecoratorContainer.showPreferencePage);
		
		PopupManager.popupContainer = popupContainer;
	}
	
	Component.onDestruction: {
		Events.unSubscribeEvent("StartLoading", thumbnailDecoratorContainer.startLoading);
		Events.unSubscribeEvent("StopLoading", thumbnailDecoratorContainer.stopLoading);
		
		Events.unSubscribeEvent("ShowPreferencePage", thumbnailDecoratorContainer.showPreferencePage);
	}
	
	function updateModels(){
		pagesManager.updateModel();
	}
	
	function onLogout(){
		clearModels();
		Events.sendEvent("DrawerSetVisible", false);
		drawingContainer.content = null;
	}
	
	function clearModels(){
		menuPanel.clearModels();
		pagesManager.clearModels();
		
		MainDocumentService.clear();
	}
	
	function fillPreferenceParamsSet(){
	}
	
	Connections {
		target: AuthorizationController;
		
		function onLoggedOut(){
			thumbnailDecoratorContainer.onLogout();
		}
	}
	
	Connections {
		target: MainDocumentService;
		
		function onTryRegisterDocumentService(typeId, callback){
			let pageIndex = -1;
			for (let i = 0; i < menuPanel.model.getItemsCount(); i++){
				let pageId = menuPanel.model.getData("id", i);
				if (pageId === typeId){
					pageIndex = i;
					pagesManager.loadPageContent(i);
					break;
				}
			}
			
			callback(pageIndex >= 0);
		}
	}
	
	MenuPanel {
		id: menuPanel;
		z: 9
		anchors.left: parent.left;
		anchors.leftMargin: thumbnailDecoratorContainer.mainMargin;
		anchors.top: topPanelLoader_.bottom;
		anchors.topMargin: thumbnailDecoratorContainer.mainMargin;
		anchors.bottom: bottomPanel.top;
		anchors.bottomMargin: thumbnailDecoratorContainer.mainMargin;
		
		model: pagesManager.pageModel;
		
		// The rail keeps its own colour even before the page list arrives. Left
		// transparent while empty it showed the window's darker backdrop through
		// itself, so the panel came up grey and turned white once data landed.
		color: Style.colorMenuPanel !== undefined ? Style.colorMenuPanel : Style.baseColor;
		
		Component.onCompleted: {
			MainDocumentService.documentOpened.connect(onDocumentOpened);
			Events.subscribeEvent("GlobalSearchActivated", seacrhActivated)
		}
		
		Component.onDestruction: {
			MainDocumentService.documentOpened.disconnect(onDocumentOpened);
			Events.unSubscribeEvent("GlobalSearchActivated", seacrhActivated)
		}
		
		function onDocumentOpened(typeId, documentId, documentTypeId){
			for (let i = 0; i < menuPanel.model.getItemsCount(); i++){
				let pageId = menuPanel.model.getData("id", i);
				if (pageId === typeId){
					menuPanel.activePageIndex = i;
					break;
				}
			}
		}
		
		function seacrhActivated(text){
			if (text == ""){
				return;
			}
			
			for (let i = 0; i < menuPanel.model.getItemsCount(); i++){
				let pageId = menuPanel.model.getData("id", i);
				if (pageId === "Search"){
					menuPanel.activePageIndex = i;
					break;
				}
			}
		}
	}
	
	BottomPanel {
		id: bottomPanel;
		
		anchors.left: parent.left;
		anchors.leftMargin: thumbnailDecoratorContainer.mainMargin;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: thumbnailDecoratorContainer.mainMargin;
	}
	
	PagesManager {
		id: pagesManager;
		
		anchors.left: menuPanel.visible ? menuPanel.right : parent.left;
		anchors.right: thumbnailDecoratorContainer.right;
		anchors.top: topPanelLoader_.bottom;
		anchors.bottom: thumbnailDecoratorContainer.bottom;
		
		anchors.topMargin: thumbnailDecoratorContainer.pageMargin;
		anchors.bottomMargin: thumbnailDecoratorContainer.pageMargin;
		anchors.leftMargin: thumbnailDecoratorContainer.pageMargin;
		anchors.rightMargin: thumbnailDecoratorContainer.pageMargin;
		
		activePageIndex: menuPanel.activePageIndex;
	}
	
	DrawingContainer{
		id: drawingContainer_;
		z: topPanelLoader_.z + 1;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: 50;
		edge: Qt.RightEdge;
	}
	
	// Loaded rather than merely hidden: the panel brings a UserPanel of its own,
	// which would otherwise sit here invisible and repeat every profile request
	// the rail's account row already makes.
	Loader {
		id: topPanelLoader_;
		
		z: 10;
		
		anchors.top: parent.top;
		anchors.topMargin: thumbnailDecoratorContainer.mainMargin;
		
		width: parent.width;
		height: !active ? 0 : Style.sizePanelsHeight !== undefined ? Style.sizePanelsHeight : 60;
		
		active: thumbnailDecoratorContainer.topPanelVisible;
		sourceComponent: thumbnailDecoratorContainer.topPanelComp;
	}
	
	property Component topPanelComp: Component {
		TopPanel {}
	}
	
	function showPreferencePage(){
		fillPreferenceParamsSet();
		ModalDialogManager.openDialog(preferenceDialogComp, {"paramsSet": settingsController.createRepresentation()});
	}
	
	StackView {
		id: stackView_;
		
		z: topPanelLoader_.z + 1;
		
		anchors.fill: parent;
	}
	
	// The server address is edited in the preferences, and on desktop it has to be
	// reachable before there is a session - sign-in and "no connection" both cover
	// the rail that otherwise carries this button. Same z as the stack view and
	// declared after it, so the dialogs below still come out on top.
	ToolButton {
		id: shellPreferenceButton;
		objectName: "ShellPreferenceButton";
		
		z: stackView_.z;
		
		anchors.top: parent.top;
		anchors.right: parent.right;
		anchors.margins: Style.marginM;
		
		width: Style.controlHeightL;
		height: width;
		
		visible: !thumbnailDecoratorContainer.topPanelVisible
			&& Qt.platform.os !== "web"
			&& stackView_.count > 0;
		
		tooltipText: qsTr("Settings");
		iconSource: "qrc:/" + Style.getIconPath("Icons/Settings", Icon.State.On, Icon.Mode.Normal);
		
		decorator: Component {
			ToolButtonDecorator {
				icon.width: Style.iconSizeM;
				radius: height / 2;
			}
		}
		
		onClicked: {
			Events.sendEvent("ShowPreferencePage");
		}
	}
	
	DialogManagerView {
		anchors.fill: parent;
		
		z: topPanelLoader_.z + 1;
	}
	
	function showPage(pageComp){
		stackView.clear()
		
		if (pageComp){
			stackView.addPage(pageComp)
		}
	}
	
	property Component authorizationPageComp: Component {
		AuthorizationPage {
			id: authorizationPage;
			
			anchors.fill: parent;
			anchors.topMargin: topPanelLoader_.height;
			
			canRecoveryPassword: thumbnailDecoratorContainer.canRecoveryPassword;
			
			Component.onCompleted: {
				Events.sendEvent("SetUserPanelEnabled", false);
			}
			
			onRegisterUser: {
				AuthorizationController.registerUser(userData);
			}
			
			onLogin: {
				AuthorizationController.login(login, password);
			}
		}
	}
	
	// SuperuserPasswordPage manages its own submitting/error state inline (in-button
	// loading indicator, inline error banner) rather than the startLoading()/
	// stopLoading() full-screen overlay - that used to get stuck on screen forever
	// after a failed CreateSuperuser call, since nothing ever turned it back off.
	property Component superuserPasswordPageComp: Component {
		SuperuserPasswordPage {
			id: superuserPasswordPage;

			anchors.fill: parent;
			anchors.topMargin: topPanelLoader_.height;

			Component.onCompleted: {
				Events.sendEvent("SetUserPanelEnabled", false);
			}

			onPasswordSetted: {
				thumbnailDecoratorContainer.showPage(thumbnailDecoratorContainer.authorizationPageComp);
			}
		}
	}
	
	PopupContainer {
		id: popupContainer;
		z: 10000
		anchors.right: parent.right;
		anchors.rightMargin: Style.marginM;
		anchors.bottom: parent.bottom;
		anchors.bottomMargin: Style.marginM;
	}
	
	Loader {
		id: loadingPageLoader;
		z: 9999;
		anchors.fill: parent;
	}
	
	property Component loadingPageComp: Component {
		Loading {
			id: loading;
			z: 9999;
			anchors.fill: parent;
		}
	}
	
	function startLoading(){
		loadingPageLoader.sourceComponent = loadingPageComp;
	}
	
	function stopLoading(){
		loadingPageLoader.sourceComponent = undefined;
	}
	
	Component {
		id: preferenceDialogComp;
		PreferenceDialog {
			settingsController: thumbnailDecoratorContainer.settingsController
		}
	}
	
	property Component errorDialog: Component {
		ErrorDialog {
			onFinished: {}
		}
	}
}
