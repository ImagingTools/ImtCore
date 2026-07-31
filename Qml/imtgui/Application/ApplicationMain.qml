import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtauthgui 1.0
import imtauthUsersSdl 1.0


Item {
	id: application;
	
	anchors.fill: parent;
	
	property Decorators decorators: decorators_
	
	property alias localSettings: clientSettingsController.json
	
	property alias thumbDecMenuPanelRadius: thumbnailDecorator.menuPanelRadius;

	property bool serverReady: true; // [deprecated]
	property bool authorizationServerConnected: false;
	property bool useWebSocketSubscription: false;
	
	property bool firstModelsIsInit: false;
	property bool serverConnected: subscriptionManager_.status == 1;
	
	property alias subscriptionManager: subscriptionManager_;
	
	property alias loadPageByClick: thumbnailDecorator.loadPageByClick;
	property alias canRecoveryPassword: thumbnailDecorator.canRecoveryPassword;

	property alias authConnectionState: connectionState.authConnectionState
	property alias pageAboutProvider: pageAboutProvider
	
	// TODO: ConnectionStateMachine.qml
	QtObject {
		id: connectionState

		property int totalStatus: -1
		
		property var status : ({
			TRY_CONNECTING_TO_AUTH_SERVER: 0,
			AUTH_SERVER_CONNECTION_ERROR: 1,
			TRY_CONNECTING_TO_APP_SERVER: 2,
			APP_SERVER_CONNECTION_ERROR: 3,
			CONNECTION_SUCCESFUL: 4
		})
		
		// 0 - WebSocket.Connecting, 1 - WebSocket.Open, 2 - WebSocket.Closing, 3 - WebSocket.Closed, 4 - WebSocket.Error
		//
		// Until the WebSocket has been given a URL it still reports its initial
		// Closed state - and the port it needs only arrives with the first
		// GetApplicationInfo response. Reporting that as APP_SERVER_CONNECTION_ERROR
		// meant every cold start emitted ServerConnectionError at t=0 and, whenever
		// the first response took longer than statusTimer's second, showed
		// "Server connection error" instead of "Try connecting...". Report
		// "connecting" for the whole window in which the port is still unknown.
		property int appConnectionState: {
			if (!application.useWebSocketSubscription){
				return 1;
			}

			if (application.webSocketPort < 0){
				return 0;
			}

			return subscriptionManager_.status;
		}
		property int authConnectionState: 1 // -1 - Unknown, 0 - Connecting, 1 - Connected, 2 - Disconnected

		onAppConnectionStateChanged: {
			checkState()
		}
		
		onAuthConnectionStateChanged: {
			checkState()
		}
		
		function checkState(){
			if (authConnectionState == 1 && appConnectionState == 1){
				totalStatus = status.CONNECTION_SUCCESFUL
			}
			else if (appConnectionState == 0){
				totalStatus = status.TRY_CONNECTING_TO_APP_SERVER
			}
			else if (appConnectionState == 4 || appConnectionState == 3){
				totalStatus = status.APP_SERVER_CONNECTION_ERROR
			}
			else if (authConnectionState == 0){
				totalStatus = status.TRY_CONNECTING_TO_AUTH_SERVER
			}
			else if (authConnectionState == 2){
				totalStatus = status.AUTH_SERVER_CONNECTION_ERROR
			}
		}
		
		onTotalStatusChanged: {
			if (totalStatus == status.TRY_CONNECTING_TO_AUTH_SERVER){
				internal.errorMessage = qsTr("Try connecting to authorization server ...")
				internal.loadingVisible = true
			}
			else if (totalStatus == status.AUTH_SERVER_CONNECTION_ERROR){
				internal.errorMessage = qsTr("Authorization server connection error")
				internal.loadingVisible = false
			}
			else if (totalStatus == status.TRY_CONNECTING_TO_APP_SERVER){
				internal.errorMessage = qsTr("Try connecting to ") + application.getServerUrl() + " ..."
				internal.loadingVisible = true
			}
			else if (totalStatus == status.APP_SERVER_CONNECTION_ERROR){
				internal.errorMessage = qsTr("Server connection error")
				internal.loadingVisible = false
				Events.sendEvent("ServerConnectionError");
			}
			else if (totalStatus == status.CONNECTION_SUCCESFUL){
				thumbnailDecorator.stackView.clear();
				statusTimer.stop()
				Events.sendEvent("SearchVisible", true);
				Events.sendEvent("SetUserPanelEnabled", true);
				
				application.firstModelsInit()
				
				return
			}

			statusTimer.restart()
		}
	}
	
	Timer {
		id: statusTimer
		interval: 1000
		repeat: false
		onTriggered: {
			thumbnailDecorator.stackView.clear();
			
			Events.sendEvent("SearchVisible", false);
			Events.sendEvent("SetUserPanelEnabled", false);
			
			application.showMessagePage(internal.errorMessage, internal.loadingVisible)
		}
	}
	
	QtObject {
		id: internal
		property string errorMessage: "";
		property bool loadingVisible: false;
	}

	function showMessagePage(message, loadingVisible){
		if (!loadingVisible){
			loadingVisible = false
		}

		internal.errorMessage = message;
		internal.loadingVisible = loadingVisible;
		
		thumbnailDecorator.stackView.addPage(messagePageComp);
	}
	
	signal saveSettings(string json);
	signal settingsSaved();
	signal settingsSaveFailed();
	
	Component.onCompleted: {
		setDecorators()
		timer.onTriggeredFunc();
	}
	
	// Dragging a window edge changes width and height on nearly every frame, and each
	// change used to dispatch two global Events immediately - so a resize walked every
	// subscriber list of three event keys, allocating a fresh parameters object each
	// time. Coalesce into one dispatch per event-loop turn instead, and only for the
	// dimension that actually changed.
	onWidthChanged: sizeEventTimer.restart()
	onHeightChanged: sizeEventTimer.restart()

	QtObject {
		id: lastSentSize

		property int width: -1
		property int height: -1
	}

	Timer {
		id: sizeEventTimer

		interval: 0
		repeat: false

		onTriggered: {
			let newWidth = application.width;
			let newHeight = application.height;

			if (newWidth === lastSentSize.width && newHeight === lastSentSize.height){
				return;
			}

			let widthChanged = newWidth !== lastSentSize.width;
			let heightChanged = newHeight !== lastSentSize.height;

			lastSentSize.width = newWidth;
			lastSentSize.height = newHeight;

			Events.sendEvent("AppSizeChanged", {"Width": newWidth, "Height": newHeight})

			if (widthChanged){
				Events.sendEvent("AppWidthChanged", newWidth)
			}

			if (heightChanged){
				Events.sendEvent("AppHeightChanged", newHeight)
			}
		}
	}
	
	// WebSocket port, resolved from the merged GetApplicationInfo response (see
	// ApplicationInfoProvider / CApplicationInfoControllerComp). -1 means "not known
	// yet"; the retry Timer below re-fetches until it is.
	property int webSocketPort: -1;

	onWebSocketPortChanged: {
		application.connectToWebSocketServer();
	}

	Decorators {
		id: decorators_
	}

	// Exposed like `decorators` so applications that offer a Qt-style theme switch
	// (e.g. ControlsGallery) reuse this instance instead of creating a second full
	// DecoratorsQt tree of their own.
	property DecoratorsQt decoratorsQt: decoratorsQt_

	DecoratorsQt {
		id: decoratorsQt_;
	}

	function setDecorators(){
		Style.setDecorators(decorators)
	}

	function getServerUrl(){
		return clientSettingsController.getServerUrl()
	}

	function reconnect(){
		application.webSocketPort = -1;
	}

	function getHeaders(){
		return {"productId": AuthorizationController.productId};
	}

	property alias thumbnailDecoratorGui: thumbnailDecorator;

	// Startup bootstrap data (app metadata, logo, version, WebSocket port) arrives via
	// the shared ApplicationInfoProvider singleton - a single GetApplicationInfo
	// request that also folds in what used to be separate GetWebSocketUrl / GetUserMode /
	// CheckSuperuserExists queries (see CApplicationInfoControllerComp on the server).
	Connections {
		target: ApplicationInfoProvider;

		function onUpdated(){
			let serverApplicationInfo = ApplicationInfoProvider.serverApplicationInfo;
			if (serverApplicationInfo){
				context.appId = serverApplicationInfo.m_applicationId;
				context.appName = serverApplicationInfo.m_applicationName;
				Style.logoIconName = serverApplicationInfo.m_logoIconName;

				pageAboutProvider.serverVersion = serverApplicationInfo.m_version

				AuthorizationController.productId = serverApplicationInfo.m_applicationId
			}

			if (ApplicationInfoProvider.webSocketPort >= 0){
				application.webSocketPort = ApplicationInfoProvider.webSocketPort;
			}
		}
	}

	property DesignSchemaProvider designProvider : DesignSchemaProvider
	{
	}
	
	property LanguageProvider languageProvider : LanguageProvider
	{
	}
	
	ClientSettingsController {
		id: clientSettingsController
		
		onSaveSettings: {
			application.saveSettings(paramsSet.toJson())
		}
		
		onUrlChanged: {
			// AuthorizationController.logout();
			application.reconnect();
		}
	}
	
	UserSettingsController {
		id: userSettingsController
		
		onParamModelCreated: {
			let languageSelectionParam = application.languageProvider.selectionParam;
			let languageParamter = getParameterById(application.languageProvider.typeId);
			if (languageParamter.m_data !== languageSelectionParam.toJson()){
				application.languageProvider.selectionParam.createFromJson(languageParamter.m_data)
			}
			
			let designSelectionParam = application.designProvider.selectionParam;
			let designParamter = getParameterById(application.designProvider.typeId)
			if (designParamter.m_data !== designSelectionParam.toJson()){
				application.designProvider.selectionParam.createFromJson(designParamter.m_data)
			}
		}
	}
	
	PageAboutProvider {
		id: pageAboutProvider
	}
	
	SubscriptionManager {
		id: subscriptionManager_;
		active: application.useWebSocketSubscription;
	}

	// Global ticket notification subscriptions — always active regardless of
	// which page is currently selected in MenuPanel.  Previously these lived
	// inside TicketCollectionView.qml which is lazy-loaded by PagesManager,
	// so notifications were lost until the user navigated to the Tickets page.
	//
	// autoSubscribe is false so that registration only happens after login
	// (see onLoggedIn / onLoggedOut below).

	// Custom popup delegate for ticket notifications — clickable card
	// that navigates to the ticket.
	Component {
		id: ticketNotificationDelegate

		Rectangle {
			id: ticketPopupRoot

			property string messageId: ""
			property string messageType: ""
			property string messageText: ""
			property bool closable: false
			property var popupContainer: null

			// Custom properties set via addCustomMessage properties bag
			property string ticketId: ""
			property string ticketNumber: ""
			property string ticketTitle: ""
			property string senderName: ""
			property string preview: ""
			property string notificationType: "message" // "message" or "assignee"

			width: parent ? parent.width : Style.sizeHintXS
			height: contentColumn.height + 2 * Style.marginL
			radius: Style.radiusM
			border.color: "#bbbbbb"
			color: ticketPopupRoot.notificationType === "assignee" ? "#d4edda" : "#d6eaf8"

			MouseArea {
				anchors.fill: parent
				cursorShape: Qt.PointingHandCursor
				hoverEnabled: true
				onClicked: {
					if (ticketPopupRoot.ticketId)
						NavigationController.navigate("Tickets/Ticket/" + ticketPopupRoot.ticketId)
					if (ticketPopupRoot.popupContainer)
						ticketPopupRoot.popupContainer.removeMessageById(ticketPopupRoot.messageId)
				}
				onEntered: ticketPopupRoot.border.color = Style.highlightColor
				onExited: ticketPopupRoot.border.color = "#bbbbbb"
			}

			Column {
				id: contentColumn
				anchors.left: parent.left
				anchors.right: closeArea.left
				anchors.top: parent.top
				anchors.margins: Style.marginL
				spacing: Style.marginM

				// Title row: icon + "#Number Title — SenderName" (all key info)
				Row {
					width: parent.width
					spacing: Style.marginS

					Image {
						anchors.verticalCenter: parent.verticalCenter
						width: Style.iconSizeS
						height: width
						source: ticketPopupRoot.notificationType === "assignee"
								? Style.getIconPath("Icons/Assignment", Icon.State.On, Icon.Mode.Normal)
								: Style.getIconPath("Icons/Message", Icon.State.On, Icon.Mode.Normal)
						sourceSize.width: width
						sourceSize.height: height
					}

					BaseText {
						width: parent.width - Style.iconSizeS - Style.marginS
						text: ticketPopupRoot.notificationType === "assignee"
							  ? (ticketPopupRoot.ticketNumber ? "#" + ticketPopupRoot.ticketNumber + " " : "")
								+ ticketPopupRoot.ticketTitle + " — " + qsTr("Assigned to you")
							  : (ticketPopupRoot.ticketNumber ? "#" + ticketPopupRoot.ticketNumber + " " : "")
								+ ticketPopupRoot.ticketTitle
								+ (ticketPopupRoot.senderName ? " — " + ticketPopupRoot.senderName : "")
						font.bold: true
						elide: Text.ElideRight
						maximumLineCount: 1
					}
				}

				// Message preview — single line, limited height, no raw \n
				BaseText {
					width: parent.width
					text: ticketPopupRoot.preview.split("\n").join(" ")
					color: "#666666"
					elide: Text.ElideRight
					maximumLineCount: 1
					visible: ticketPopupRoot.preview !== ""
				}

				// Hint
				BaseText {
					text: qsTr("Click to open →")
					color: "#999999"
				}
			}

			ToolButton {
				id: closeArea
				width: Style.iconSizeS + Style.marginM
				height: width
				anchors.right: parent.right
				anchors.top: parent.top
				anchors.margins: Style.marginS
				iconSource: "qrc:/" +Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal)
				decorator: Component {
					ToolButtonDecorator {
						color: "transparent"
						icon.width: Style.iconSizeXS
					}
				}
				onClicked: {
					if (ticketPopupRoot.popupContainer)
						ticketPopupRoot.popupContainer.removeMessageById(ticketPopupRoot.messageId)
				}
			}
		}
	}

	// Subscribes to server-side ticket message notifications and surfaces
	// them via PopupManager. The server-side filter (CTicketMessageNotifierComp)
	// already restricts delivery to users related to the ticket
	// (reporter / assignees / admin), excluding the sender.
	SubscriptionClient {
		id: ticketMessageSubscription
		gqlCommandId: "OnTicketMessageReceived"
		autoSubscribe: false
		onMessageReceived: {
			if (!data){
				return
			}
			var ticketId = data.containsKey("ticketId") ? data.getData("ticketId") : ""
			var ticketNumber = data.containsKey("ticketNumber") ? data.getData("ticketNumber") : ""
			var ticketTitle = data.containsKey("ticketTitle") ? data.getData("ticketTitle") : ""
			var senderName = data.containsKey("senderUserName") ? data.getData("senderUserName") : ""
			var content = data.containsKey("content") ? data.getData("content") : ""
			var messageId = data.containsKey("messageId") ? data.getData("messageId") : ""

			var preview = content ? String(content) : ""
			if (preview.length > 80){
				preview = preview.substring(0, 80) + "…"
			}

			PopupManager.addCustomMessage(messageId, ticketNotificationDelegate, {
				autoClose: true,
				ticketId: ticketId,
				ticketNumber: ticketNumber,
				ticketTitle: ticketTitle,
				senderName: senderName,
				preview: preview,
				notificationType: "message"
			})
		}
	}

	// Subscribes to server-side assignee change notifications and surfaces
	// them via PopupManager. The server-side filter (CTicketAssigneeNotifierComp)
	// delivers only to newly added assignees.
	SubscriptionClient {
		id: ticketAssigneeSubscription
		gqlCommandId: "OnTicketAssigneeChanged"
		autoSubscribe: false
		onMessageReceived: {
			if (!data){
				return
			}

			var ticketId = data.containsKey("ticketId") ? data.getData("ticketId") : ""
			var ticketNumber = data.containsKey("ticketNumber") ? data.getData("ticketNumber") : ""
			var ticketTitle = data.containsKey("ticketTitle") ? data.getData("ticketTitle") : ""

			var popupId = "TicketAssignee_" + ticketNumber
			PopupManager.addCustomMessage(popupId, ticketNotificationDelegate, {
				autoClose: true,
				ticketId: ticketId,
				ticketNumber: ticketNumber,
				ticketTitle: ticketTitle,
				notificationType: "assignee"
			})
		}
	}
	
	property Component messagePageComp: Component {
		ServerNoConnectionView {
			z: 5;
			anchors.fill: parent;
			anchors.topMargin: thumbnailDecorator.topPanel.height;
			loadingVisible: internal.loadingVisible;
			text: internal.errorMessage;
		}
	}
	
	function getWebSocketUrl(serverUrl){
		try {
			let url = new URL(serverUrl);

			let protocol = "ws";
			if (url.protocol === "https:"){
				protocol = "wss";
			}

			url.protocol = protocol

			if (application.webSocketPort >= 0){
				url.port = application.webSocketPort;
			}
			else{
				console.error("WebSocket port provider has invalid port!");
			}

			if (context.appId && context.appId !== ""){
				url.pathname = "/" + context.appId + "/wssub";
			}

			return String(url)
		}
		catch(error){
			console.error(error);
			return "";
		}
	}
	
	Component {
		id: serverLogProviderComp
		ServerLogProvider {}
	}
	
	ThumbnailDecorator {
		id: thumbnailDecorator;
		
		anchors.fill: parent;
		Component.onCompleted: {
			settingsController.registerParamEditor("ServerLog", serverLogProviderComp)
		}
		
		function fillPreferenceParamsSet(){
			if (Qt.platform.os !== "web"){
				settingsController.registerParamsSetController("Network", qsTr("Network"), clientSettingsController)
			}

			if (application.serverConnected){
				settingsController.registerParamsSetController("General", qsTr("General"), userSettingsController)
				settingsController.registerParamsSetController("About", qsTr("About"), pageAboutProvider)
			}
		}
	}
	
	function updateAllModels(){
		userSettingsController.getSettings()
		thumbnailDecorator.updateModels();
	}
	
	function onSimpleUserManagement(){
		application.updateAllModels();
	}
	
	// After forced logout (session/refresh expired) open the login UI immediately
	// instead of leaving the user on a connection-status overlay.
	function enterLoggedOutUi(){
		statusTimer.stop();
		Events.sendEvent("SearchVisible", false);
		Events.sendEvent("SetUserPanelEnabled", false);

		if (AuthorizationController.isStrongUserManagement()){
			// Show login right away; superuser probe may still switch to setup page.
			thumbnailDecorator.showPage(thumbnailDecorator.authorizationPageComp);
			AuthorizationController.updateSuperuserModel();
			return;
		}

		if (AuthorizationController.isSimpleUserManagement()){
			application.onSimpleUserManagement();
			return;
		}

		// User mode unknown — re-detect (async). Still show login so the UI is not blank.
		thumbnailDecorator.showPage(thumbnailDecorator.authorizationPageComp);
		AuthorizationController.updateUserManagementModel();
	}
	
	function firstModelsInit(force){
		if (!force){
			force = false
		}

		if (!force && firstModelsIsInit){
			return
		}

		// No explicit updateUserManagementModel() call here: by the time this runs
		// (WS reaches CONNECTION_SUCCESSFUL, which itself requires a WebSocket port
		// obtained from the very first ApplicationInfoProvider fetch at startup),
		// userMode/superuser status have already arrived from that same merged
		// GetApplicationInfo response - just apply the routing decision now that
		// the connection overlay machinery has settled (see routeUserManagementUi).
		let loggedUserId = AuthorizationController.getLoggedUserId();
		if (loggedUserId === ""){
			application.routeUserManagementUi();
		}

		firstModelsIsInit = true;
	}
	
	function connectToWebSocketServer(){
		if (!application.useWebSocketSubscription){
			return;
		}
		
		subscriptionManager_.active = false;
		let serverUrl = getServerUrl();
		let webSocketServerUrl = getWebSocketUrl(serverUrl);
		subscriptionManager_.url = webSocketServerUrl;
		subscriptionManager_.active = true;
	}
	
	// Applies the login / superuser-setup / simple-dashboard routing decision from
	// the latest known userMode + superuser status (both arrive together in the
	// merged ApplicationInfoProvider response - see CApplicationInfoControllerComp).
	//
	// Guarded on CONNECTION_SUCCESFUL: the connection-status overlay machinery
	// (statusTimer / onTotalStatusChanged) clears thumbnailDecorator.stackView
	// whenever it is not yet in the "connected" state, and again exactly once when
	// it *reaches* that state. Routing before that point means the login page gets
	// shown and then immediately wiped by that clear() - login page flashes, then
	// a blank/gray screen. So this only acts once the overlay machinery has
	// settled; firstModelsInit() (called from the CONNECTION_SUCCESFUL branch)
	// re-applies it at that point using the already-cached data (no extra fetch).
	function routeUserManagementUi(){
		if (connectionState.totalStatus !== connectionState.status.CONNECTION_SUCCESFUL){
			return;
		}

		if (AuthorizationController.isSimpleUserManagement()){
			application.onSimpleUserManagement();
			return;
		}

		if (!AuthorizationController.isStrongUserManagement()){
			return;
		}

		let status = ApplicationInfoProvider.superuserStatus;
		let message = ApplicationInfoProvider.superuserMessage;

		if (AuthorizationController.getLoggedUserId() !== ""){
			// Already logged in - a background refresh (e.g. on WebSocket reconnect)
			// must not disturb the current session/page.
			if (status === "UNKNOWN" && message){
				console.warn("Superuser check failed:", message)
				PopupManager.addWarningMessage(message, true)
			}
			return;
		}

		if (status === "EXISTS"){
			thumbnailDecorator.showPage(thumbnailDecorator.authorizationPageComp)
		}
		else if (status === "NOT_EXISTS"){
			thumbnailDecorator.showPage(thumbnailDecorator.superuserPasswordPageComp)
		}
		else{
			// UNKNOWN / network error (e.g. database unreachable - see
			// CApplicationInfoControllerComp::FillSuperuserStatus): prefer the login
			// page over a sticky "Try connecting..." message after session expiry,
			// but surface *why* to the user instead of only logging it - a login
			// attempt here is likely to fail too, and a silent redirect would look
			// like the click just did nothing.
			thumbnailDecorator.showPage(thumbnailDecorator.authorizationPageComp)
			if (message){
				console.warn("Superuser check failed:", message)
				PopupManager.addErrorMessage(message, true)
			}
		}
	}

	Connections {
		target: AuthorizationController;

		function onUserModeChanged(userMode){
			application.routeUserManagementUi();
		}

		function onSuperuserExistResult(status, message){
			application.routeUserManagementUi();
		}

		function onLoggedIn(){
			thumbnailDecorator.drawingContainer.content = Style.drawingContainerDecorator;
			thumbnailDecorator.showPage(undefined)
			
			application.updateAllModels();

			// Register ticket notification subscriptions now that we have
			// a valid auth token for the logged-in user.
			ticketMessageSubscription.registerSubscription();
			ticketAssigneeSubscription.registerSubscription();
		}

		function onTenantSelected(tenantId){
			application.updateAllModels();
		}
		
		function onLoggedOut(){
			// Unregister subscriptions for the previous user before
			// SubscriptionManager.clear() wipes the model.
			ticketMessageSubscription.unRegisterSubscription();
			ticketAssigneeSubscription.unRegisterSubscription();

			thumbnailDecorator.stopLoading();
			NavigationController.clear();
			subscriptionManager_.clear();
			// Force login UI after session/refresh death (do not wait on connection state).
			application.enterLoggedOutUi();
			application.firstModelsIsInit = false;
		}
	}
	
	// Retries the bootstrap fetch until a WebSocket port is known. The first retry is
	// deliberately quick (a cold start that just lost a race with the server should
	// not cost three seconds), then backs off so an unreachable server is not polled
	// at a fixed 3s forever. ApplicationInfoProvider is single-flight, so a slow
	// response never stacks duplicate requests.
	property Timer timer: Timer{
		readonly property int minInterval: 1000;
		readonly property int maxInterval: 15000;

		interval: minInterval;
		repeat: true;
		running: application.useWebSocketSubscription && application.webSocketPort == -1;
		onTriggered: {
			interval = Math.min(interval * 2, maxInterval);

			onTriggeredFunc();
		}

		onRunningChanged: {
			if (running){
				interval = minInterval;
			}
		}

		function onTriggeredFunc(){
			if (!application.useWebSocketSubscription){
				return;
			}

			if (application.webSocketPort == -1){
				ApplicationInfoProvider.updateModel();
			}
		}
	}
}


