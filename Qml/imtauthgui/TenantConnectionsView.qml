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
 * TenantConnectionsView
 *
 * Page for viewing established tenant relationships/connections.
 * 
 * TODO: Implement actual connections display when API is available.
 * Currently this is a placeholder that will be updated when the backend
 * provides a proper API for fetching established connections.
 */
ViewBase {
	id: connectionsView

	contentColor: Style.baseColor

	readonly property var tenantData: connectionsView.model
	property var stateManager: null
	property var apiClient: null
	
	function updateGui() {
		// TODO: Implement when API for fetching connections is available
	}
	
	function updateModel() {
		// No model updates needed for this view
	}
	
	CustomScrollbar {
		id: scrollbar
		z: connectionsView.z + 1
		anchors.right: parent.right
		anchors.top: mainFlickable.top
		anchors.bottom: mainFlickable.bottom
		secondSize: Style.marginM
		targetItem: mainFlickable
	}
	
	Flickable {
		id: mainFlickable
		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL
		anchors.horizontalCenter: parent.horizontalCenter
		width: Math.min(parent.width - Style.marginXL * 2, Style.sizeHintXXL)
		contentWidth: mainColumn.width
		contentHeight: mainColumn.height + 2 * Style.marginXL
		boundsBehavior: Flickable.StopAtBounds
		clip: true
		
		Column {
			id: mainColumn
			width: mainFlickable.width
			spacing: Style.marginXL
			
			// =============================================================
			// SECTION: Established Connections
			// =============================================================
			GroupHeaderView {
				width: mainColumn.width
				title: qsTr("Established Connections")
			}
			
			BaseText {
				width: mainColumn.width
				wrapMode: Text.WordWrap
				text: qsTr("This page will display your established tenant connections. Currently, this functionality requires backend API support that is not yet implemented.")
				font.pixelSize: Style.fontSizeM
				color: Style.textColor
			}
			
			// TODO: When API is ready, implement the following sections:
			// - List of established connections
			// - Connection details (roles, status, etc.)
			// - Connection management actions
			
			BaseText {
				width: mainColumn.width
				wrapMode: Text.WordWrap
				text: qsTr("When implemented, this page will show:nnu2022 Active tenant relationshipsnu2022 Connection roles and permissionsnu2022 Connection status and historynu2022 Management actions for connections")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}
		}
	}
}
