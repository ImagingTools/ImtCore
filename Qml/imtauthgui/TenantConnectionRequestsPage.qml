// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtcolgui 1.0
import imtguigql 1.0
import imtauthgui 1.0

/**
 * TenantConnectionRequestsPage
 *
 * Container page for connection requests functionality using MultiPageView.
 * 
 * This page hosts multiple sub-pages for different connection scenarios:
 *   - Overview: Dashboard with statistics, recent activity, and attention-required items
 *   - Requests: Create and manage connection requests
 *   - Connect Codes: Generate and manage one-time connect codes
 *   - Redeem Code: Activate received connect codes
 *   - Active Connections: View established tenant relationships
 */
ViewBase {
	id: connectionsPage
	
	anchors.fill: parent
	commandsPanelVisible: false
	contentColor: Style.baseColor
	
	readonly property var tenantData: connectionsPage.model
	property var stateManager: null
	property var apiClient: null
	
	function updateGui() {
		// Refresh the current page
		var currentPage = multiPageView.getPageByIndex(multiPageView.currentIndex)
		if (currentPage && typeof currentPage.updateGui === "function") {
			currentPage.updateGui()
		}
	}
	
	function updateModel() {
		// Update model on the current page if needed
		var currentPage = multiPageView.getPageByIndex(multiPageView.currentIndex)
		if (currentPage && typeof currentPage.updateModel === "function") {
			currentPage.updateModel()
		}
	}
	
	Component.onCompleted: {
		multiPageView.updatePages()
	}
	
	// --- Multi-page navigation ---
	MultiPageView {
		id: multiPageView
		anchors.fill: parent
		panelWidth: Style.sizeHintXXS  // Increased panel width for better readability
		
		function updatePages() {
			multiPageView.clear()
			multiPageView.addPage("Overview", qsTr("Overview"), overviewPageComp, "Icons/Dashboard")
			multiPageView.addPage("Requests", qsTr("Requests"), requestsPageComp, "Icons/Link")
			multiPageView.addPage("ConnectCodes", qsTr("Connect Codes"), connectCodesPageComp, "Icons/Code")
			multiPageView.addPage("RedeemCode", qsTr("Redeem Code"), redeemCodePageComp, "Icons/Unlock")
			multiPageView.addPage("ActiveConnections", qsTr("Active Connections"), connectionsPageComp, "Icons/Network")
			multiPageView.currentIndex = 0
		}
	}
	
	// --- Page Components ---
	Component {
		id: overviewPageComp
		
		TenantConnectionOverviewView {
			model: connectionsPage.tenantData
			stateManager: connectionsPage.stateManager
			apiClient: connectionsPage.apiClient
		}
	}
	
	Component {
		id: requestsPageComp
		
		TenantConnectionRequestsView {
			model: connectionsPage.tenantData
			stateManager: connectionsPage.stateManager
			apiClient: connectionsPage.apiClient
		}
	}
	
	Component {
		id: connectCodesPageComp
		
		TenantConnectCodesView {
			model: connectionsPage.tenantData
			stateManager: connectionsPage.stateManager
			apiClient: connectionsPage.apiClient
		}
	}
	
	Component {
		id: redeemCodePageComp
		
		TenantRedeemCodeView {
			model: connectionsPage.tenantData
			stateManager: connectionsPage.stateManager
			apiClient: connectionsPage.apiClient
		}
	}
	
	Component {
		id: connectionsPageComp
		
		TenantConnectionsView {
			model: connectionsPage.tenantData
			stateManager: connectionsPage.stateManager
			apiClient: connectionsPage.apiClient
		}
	}
}
