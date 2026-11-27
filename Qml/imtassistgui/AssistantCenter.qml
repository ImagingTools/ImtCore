// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
pragma Singleton
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
	AssistantCenter

	The one assistant of the application: one conversation, reachable from everywhere.
	The button in the top panel and the side panel of the application show it while the
	assistant is enabled for the organization of the session.

	The product injects the transport once:
		AssistantCenter.controller = gqlBasedAssistantController
	From then on the center checks the availability at login and at a tenant switch, and
	stops at logout.

	Pages open the assistant with their context, without importing this module:
		Events.sendEvent("OpenAssistant", [{"kind": "selection", "label": qsTr("User Anna Berg"), "data": {"userId": "u1"}}])
	The context is shown in the panel and sent only with the next message of the user.
*/
QtObject {
	id: root

	// An AssistantController implementation, e.g. GqlBasedAssistantController
	property var controller: null

	// The assistant is enabled for the organization of the session.
	property bool active: false
	property bool panelVisible: false

	onControllerChanged: {
		root.refresh()
	}

	// Views that do not import this module (the top panel, the page decorator) follow the state through Events
	onActiveChanged: {
		if (!root.active){
			root.panelVisible = false
		}

		root.__publishState()
	}

	onPanelVisibleChanged: {
		root.__publishState()
	}

	Component.onCompleted: {
		Events.subscribeEvent("AssistantStateRequested", root.__publishState)
		Events.subscribeEvent("OpenAssistant", root.open)
		Events.subscribeEvent("ToggleAssistant", root.toggle)
	}

	function __publishState(){
		Events.sendEvent("AssistantStateChanged", {"active": root.active, "panelVisible": root.panelVisible})
	}

	// Ask the server again whether the assistant is enabled, e.g. after the administration changed it.
	function refresh(){
		if (root.controller && AuthorizationController.isLoggedIn()){
			root.controller.checkAvailability()
		}
	}

	function stop(){
		root.active = false
		if (root.controller){
			root.controller.newConversation()
		}
	}

	/**
		Open the panel. \c contextItems, if given, is a list of {kind, label, data} released by the page.
	*/
	function open(contextItems){
		if (!root.active || !root.controller){
			return
		}

		let items = contextItems ? contextItems : []
		for (let i = 0; i < items.length; i++){
			if (items[i] && items[i].kind){
				root.controller.addContextItem(items[i].kind, items[i].label ? items[i].label : items[i].kind, items[i].data ? items[i].data : {})
			}
		}

		root.panelVisible = true
	}

	function close(){
		root.panelVisible = false
	}

	function toggle(){
		if (root.panelVisible){
			root.close()
		}
		else{
			root.open([])
		}
	}

	property Connections __controllerConnections: Connections {
		target: root.controller

		function onAvailabilityChecked(available){
			root.active = available
		}
	}

	property Connections __authorizationConnections: Connections {
		target: AuthorizationController

		function onLoggedIn(){
			root.refresh()
		}

		function onLoggedOut(){
			root.stop()
		}

		// The conversation and the configuration belong to the organization of the session
		function onTenantSelected(tenantId){
			root.stop()
			root.refresh()
		}
	}
}
