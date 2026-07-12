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
 *
 * Uses the same card-based layout as TenantConnectOrganizationPage and
 * TenantConnectionCodePage.
 */
ViewBase {
	id: generalPage

	anchors.fill: parent
	commandsPanelVisible: false
	contentColor: Style.baseColor

	// Backward-compat accessor: pages reference tenantData; ViewBase exposes it as `model`.
	readonly property var tenantData: generalPage.model
	property var stateManager: null

	readonly property bool __canEditName: generalPage.stateManager
		? (generalPage.stateManager.isNewTenant || generalPage.stateManager.canChangeOrganizationName)
		: false
	readonly property bool __canEditDescription: generalPage.stateManager
		? (generalPage.stateManager.isNewTenant || generalPage.stateManager.canChangeOrganizationDescription)
		: false
	readonly property bool __canEditIsActive: generalPage.stateManager
		? (generalPage.stateManager.isNewTenant || generalPage.stateManager.canEditOrganization)
		: false

	Flickable {
		anchors.fill: parent
		anchors.topMargin: Style.marginXL
		contentHeight: mainColumn.height + Style.marginXL
		clip: true

		Column {
			id: mainColumn
			anchors.horizontalCenter: parent.horizontalCenter
			width: Math.min(parent.width - Style.marginXL * 2, 600)
			spacing: Style.marginXL

			Text {
				text: qsTr("General")
				font.pixelSize: Style.fontSizeXL
				font.bold: true
				color: Style.textColor
			}

			Text {
				text: qsTr("Basic workspace settings and configuration.")
				font.pixelSize: Style.fontSizeM
				color: Style.inactiveTextColor
				wrapMode: Text.WordWrap
				width: parent.width
			}

			Rectangle {
				width: parent.width
				height: settingsColumn.height + Style.marginL * 2
				color: Style.backgroundColor2
				radius: Style.radiusL
				border.width: 1
				border.color: Style.borderColor

				Column {
					id: settingsColumn
					anchors.left: parent.left
					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					anchors.margins: Style.marginL
					spacing: Style.marginM

					Column {
						width: parent.width
						spacing: Style.marginXS

						Text {
							text: qsTr("Tenant Name")
							font.pixelSize: Style.fontSizeS
							color: Style.inactiveTextColor
						}

						CustomTextField {
							id: nameInput
							width: parent.width
							height: Style.controlHeightM
							textSize: Style.fontSizeM
							readOnly: !generalPage.__canEditName
							placeHolderText: qsTr("Enter the tenant name")

							text: generalPage.tenantData ? (generalPage.tenantData.m_name || "") : ""
							onEditingFinished: {
								if (generalPage.tenantData && generalPage.tenantData.m_name !== nameInput.text)
									generalPage.tenantData.m_name = nameInput.text
							}
						}
					}

					Column {
						width: parent.width
						spacing: Style.marginXS

						Text {
							text: qsTr("Description")
							font.pixelSize: Style.fontSizeS
							color: Style.inactiveTextColor
						}

						CustomTextField {
							id: descriptionInput
							width: parent.width
							height: Style.controlHeightM
							textSize: Style.fontSizeM
							readOnly: !generalPage.__canEditDescription
							placeHolderText: qsTr("Enter the description")

							text: generalPage.tenantData ? (generalPage.tenantData.m_description || "") : ""
							onEditingFinished: {
								if (generalPage.tenantData && generalPage.tenantData.m_description !== descriptionInput.text)
									generalPage.tenantData.m_description = descriptionInput.text
							}
						}
					}
				}
			}

			Rectangle {
				width: parent.width
				height: 1
				color: Style.borderColor
			}

			Row {
				spacing: Style.marginM

				SwitchCustom {
					id: isActiveSwitch
					readOnly: !generalPage.__canEditIsActive
					checked: generalPage.tenantData && generalPage.tenantData.m_isActive !== undefined
						? generalPage.tenantData.m_isActive
						: true
					onCheckedChanged: {
						if (generalPage.tenantData && generalPage.tenantData.m_isActive !== isActiveSwitch.checked)
							generalPage.tenantData.m_isActive = isActiveSwitch.checked
					}
				}

				Text {
					text: qsTr("Active")
					font.pixelSize: Style.fontSizeM
					color: Style.textColor
					anchors.verticalCenter: parent.verticalCenter
				}
			}
		}
	}
}
