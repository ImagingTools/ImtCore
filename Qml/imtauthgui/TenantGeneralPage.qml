// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

/**
 * TenantGeneralPage
 *
 * General tab of the TenantEditor — basic workspace settings.
 *
 * Inherits ViewBase so that updates flow through the protected
 * doUpdateGui / doUpdateModel wrappers (re-entrance guard).
 */
ViewBase {
	id: generalPage

	commandsPanelVisible: false

	// Backward-compat accessor: pages reference tenantData; ViewBase exposes it as `model`.
	readonly property var tenantData: generalPage.model
	property var stateManager: null

	function updateGui() {
		if (!generalPage.tenantData) return
		nameInput.text = generalPage.tenantData.m_name || ""
		descriptionInput.text = generalPage.tenantData.m_description || ""
		isActiveInput.checked = generalPage.tenantData.m_isActive !== undefined
			? generalPage.tenantData.m_isActive
			: true
	}

	function updateModel() {
		if (!generalPage.tenantData) return
		generalPage.tenantData.m_name = nameInput.text
		generalPage.tenantData.m_description = descriptionInput.text
		generalPage.tenantData.m_isActive = isActiveInput.checked
	}

	readonly property bool __readOnly: !(generalPage.stateManager
		&& (generalPage.stateManager.isOwner || generalPage.stateManager.isNewTenant))

	CustomScrollbar {
		id: generalScrollbar
		z: parent.z + 1
		anchors.right: parent.right
		anchors.top: generalFlickable.top
		anchors.bottom: generalFlickable.bottom
		secondSize: Style.marginM
		targetItem: generalFlickable
	}

	Flickable {
		id: generalFlickable
		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: generalScrollbar.left
		anchors.rightMargin: Style.marginXL
		contentWidth: generalColumn.width
		contentHeight: generalColumn.height + 2 * Style.marginXL

		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Column {
			id: generalColumn
			width: Style.sizeHintXXL
			spacing: Style.marginXL

			Column {
				width: parent.width
				spacing: Style.marginXS

				BaseText {
					text: qsTr("General")
					font.pixelSize: Style.fontSizeXL
					font.bold: true
					color: Style.textColor
				}

				BaseText {
					text: qsTr("Basic workspace settings and configuration.")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}
			}

			Rectangle {
				width: parent.width
				height: 1
				color: Style.borderColor
			}

			GroupElementView {
				id: generalGroup
				width: parent.width

				TextInputElementView {
					id: nameInput
					name: qsTr("Tenant Name")
					placeHolderText: qsTr("Enter the tenant name")
					readOnly: generalPage.__readOnly

					onEditingFinished: {
						let oldText = generalPage.tenantData ? generalPage.tenantData.m_name : ""
						if (oldText !== nameInput.text)
							generalPage.doUpdateModel()
					}

					KeyNavigation.tab: descriptionInput
					KeyNavigation.backtab: isActiveInput
				}

				TextInputElementView {
					id: descriptionInput
					name: qsTr("Description")
					placeHolderText: qsTr("Enter the description")
					readOnly: generalPage.__readOnly

					onEditingFinished: {
						let oldText = generalPage.tenantData ? generalPage.tenantData.m_description : ""
						if (oldText !== descriptionInput.text)
							generalPage.doUpdateModel()
					}

					KeyNavigation.tab: isActiveInput
					KeyNavigation.backtab: nameInput
				}

				SwitchElementView {
					id: isActiveInput
					name: qsTr("Active")
					readOnly: generalPage.__readOnly

					onCheckedChanged: generalPage.doUpdateModel()
				}
			}
		}
	}
}
