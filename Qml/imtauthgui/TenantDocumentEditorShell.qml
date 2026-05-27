// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtdocgui 1.0

/**
 * TenantDocumentEditorShell
 *
 * Shared wrapper around `SingleDocumentWorkspaceShellView` used by the
 * Role / Group / User create+edit screens inside the TenantEditor sub-pages
 * (TenantRolesPage / TenantGroupsPage / TenantMembersPage).
 *
 * Encapsulates the common configuration of those shells:
 *   - centered max-1000px layout matching the surrounding list pages
 *   - hidden internal header (`headerVisible: false`) and disabled
 *     document-name dialog (`documentNameInputEnabled: false`)
 *   - optional UUID pre-allocation via `generateNewId` for the create flow
 *     (passed as `proposedSourceDocumentId`)
 *   - bookkeeping of the parent page's `__activeShellView` reference when
 *     `activeShellTarget` is set
 *
 * The `closed` signal is forwarded from the inner shell so the parent can
 * pop its stack header.
 */
Item {
	id: root

	property var documentManager: null
	property string objectTypeId: ""
	property string objectId: ""
	property bool createNew: false
	// When true and createNew is true, pre-allocate a fresh UUID and pass it
	// as `proposedSourceDocumentId` so the server persists the new document
	// with the client-allocated id.
	property bool generateNewId: false

	// Optional target object whose `__activeShellView` property will be
	// kept in sync with the inner shell. Used by the parent page's header
	// close-button handler to defer the close to the document decorator.
	property var activeShellTarget: null

	readonly property alias shellView: shell

	signal closed()

	SingleDocumentWorkspaceShellView {
		id: shell
		anchors.top: parent.top
		anchors.bottom: parent.bottom
		anchors.left: parent.left
		anchors.leftMargin: Math.max((parent.width - Math.min(parent.width - Style.marginXL * 2, 1000)) / 2, Style.marginXL)
		width: Math.min(parent.width - Style.marginXL * 2, 1000)

		documentManager: root.documentManager
		objectTypeId: root.objectTypeId
		objectId: root.objectId
		createNew: root.createNew
		proposedSourceDocumentId: (root.createNew && root.generateNewId) ? UuidGenerator.generateUUID() : ""
		headerVisible: false
		documentNameInputEnabled: false

		onClosed: root.closed()
	}

	Component.onCompleted: {
		if (root.activeShellTarget)
			root.activeShellTarget.__activeShellView = shell
	}
	Component.onDestruction: {
		if (root.activeShellTarget && root.activeShellTarget.__activeShellView === shell)
			root.activeShellTarget.__activeShellView = null
	}
}
