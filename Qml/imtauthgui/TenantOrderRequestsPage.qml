// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantOrderRequestsPage
 *
 * Order requests tab of the TenantEditor (RFC phase 3, §15).
 *
 * Lists the order requests materialized inside the current (target) tenant by
 * the cross-tenant message broker and exposes the status-feedback operations
 * (confirm / reject / update status) that report the local processing result
 * back to the originating source tenant. All operations go through the abstract
 * TenantManagementApiClient contract.
 */
ViewBase {
	id: orderRequestsPage

	commandsPanelVisible: false
	contentColor: Style.baseColor

	readonly property var tenantData: orderRequestsPage.model
	property var stateManager: null
	property var apiClient: null

	readonly property bool __canManage: orderRequestsPage.stateManager
		&& (orderRequestsPage.stateManager.isCreator || orderRequestsPage.stateManager.isOwner)

	// Status options. Index maps to the SDL OrderRequestStatus tokens.
	readonly property var __statusTokens: ["Received", "Confirmed", "Rejected", "InProgress", "Completed", "Cancelled"]

	function updateGui() {
		orderRequestsPage.__refresh()
	}

	function __refresh() {
		if (orderRequestsPage.apiClient && orderRequestsPage.tenantData && orderRequestsPage.tenantData.m_id)
			orderRequestsPage.apiClient.fetchOrderRequests(orderRequestsPage.tenantData.m_id)
	}

	onVisibleChanged: {
		if (orderRequestsPage.visible)
			orderRequestsPage.__refresh()
	}

	Component.onCompleted: orderRequestsPage.__refresh()

	Connections {
		target: orderRequestsPage.apiClient

		function onOrderRequestConfirmed(orderRequestId) {
			PopupManager.addSuccessMessage(qsTr("Order request confirmed"), true)
			orderRequestsPage.__refresh()
		}

		function onOrderRequestRejected(orderRequestId) {
			PopupManager.addSuccessMessage(qsTr("Order request rejected"), true)
			orderRequestsPage.__refresh()
		}

		function onOrderRequestStatusUpdated(orderRequestId) {
			PopupManager.addSuccessMessage(qsTr("Order request status updated"), true)
			orderRequestsPage.__refresh()
		}
	}

	CustomScrollbar {
		id: orderRequestsScrollbar
		z: parent.z + 1
		anchors.right: parent.right
		anchors.top: orderRequestsFlickable.top
		anchors.bottom: orderRequestsFlickable.bottom
		secondSize: Style.marginM
		targetItem: orderRequestsFlickable
	}

	Flickable {
		id: orderRequestsFlickable
		anchors.top: parent.top
		anchors.topMargin: Style.marginXL
		anchors.bottom: parent.bottom
		anchors.bottomMargin: Style.marginXL
		anchors.left: parent.left
		anchors.leftMargin: Style.marginXL
		anchors.right: parent.right
		anchors.rightMargin: Style.marginXL
		contentHeight: orderRequestsColumn.height + 2 * Style.marginXL
		clip: true

		Column {
			id: orderRequestsColumn
			width: Style.sizeHintXXL
			spacing: Style.marginXL

			Column {
				width: parent.width
				spacing: Style.marginXS

				BaseText {
					text: qsTr("Order Requests")
					font.pixelSize: Style.fontSizeXL
					font.bold: true
					color: Style.textColor
				}

				BaseText {
					width: parent.width
					wrapMode: Text.WordWrap
					text: qsTr("Order requests received from partner tenants. Confirm, reject or update the status of a request to report the result back to the source tenant.")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}
			}

			Rectangle {
				width: parent.width
				height: 1
				color: Style.borderColor
			}

			Button {
				text: qsTr("Refresh")
				onClicked: orderRequestsPage.__refresh()
			}

			BaseText {
				width: parent.width
				visible: !orderRequestsList.count
				text: qsTr("No order requests for this tenant.")
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			Column {
				id: orderRequestsList
				width: parent.width
				spacing: Style.marginM

				property int count: orderRequestsPage.apiClient && orderRequestsPage.apiClient.orderRequestsModel
					? orderRequestsPage.apiClient.orderRequestsModel.count
					: 0

				Repeater {
					model: orderRequestsPage.apiClient ? orderRequestsPage.apiClient.orderRequestsModel : null

					delegate: Rectangle {
						width: orderRequestsList.width
						height: orderRequestColumn.height + 2 * Style.marginM
						color: Style.alternateBaseColor
						radius: Style.radiusS
						border.color: Style.borderColor
						border.width: 1

						Column {
							id: orderRequestColumn
							anchors.left: parent.left
							anchors.right: parent.right
							anchors.verticalCenter: parent.verticalCenter
							anchors.leftMargin: Style.marginM
							anchors.rightMargin: Style.marginM
							spacing: Style.marginXS

							BaseText {
								width: parent.width
								elide: Text.ElideRight
								text: qsTr("Article: %1   Quantity: %2")
									.arg(model.articleNumber || "")
									.arg(model.quantity || 0)
								font.pixelSize: Style.fontSizeM
								color: Style.textColor
							}

							BaseText {
								width: parent.width
								elide: Text.ElideRight
								text: qsTr("Status: %1").arg(model.status || qsTr("Received"))
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}

							BaseText {
								width: parent.width
								elide: Text.ElideRight
								text: qsTr("Source tenant: %1").arg(model.sourceTenantId || "")
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}

							BaseText {
								width: parent.width
								visible: model.note && model.note !== ""
								wrapMode: Text.WordWrap
								text: model.note || ""
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}

							Row {
								spacing: Style.marginM
								visible: orderRequestsPage.__canManage

								Button {
									text: qsTr("Confirm")
									onClicked: {
										if (orderRequestsPage.apiClient)
											orderRequestsPage.apiClient.confirmOrderRequest(model.orderRequestId || "", "")
									}
								}

								Button {
									text: qsTr("Reject")
									onClicked: {
										if (orderRequestsPage.apiClient)
											orderRequestsPage.apiClient.rejectOrderRequest(model.orderRequestId || "", "")
									}
								}

								Button {
									text: qsTr("In Progress")
									onClicked: {
										if (orderRequestsPage.apiClient)
											orderRequestsPage.apiClient.updateOrderRequestStatus(model.orderRequestId || "", orderRequestsPage.__statusTokens[3], "")
									}
								}

								Button {
									text: qsTr("Completed")
									onClicked: {
										if (orderRequestsPage.apiClient)
											orderRequestsPage.apiClient.updateOrderRequestStatus(model.orderRequestId || "", orderRequestsPage.__statusTokens[4], "")
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
