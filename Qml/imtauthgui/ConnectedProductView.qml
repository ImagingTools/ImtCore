// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * The card of a connected product: its catalog entry, the tokens of its servers,
 * the permission tree it published and its administration.
 */
ViewBase {
	id: container

	anchors.fill: parent
	contentColor: Style.baseColor

	property var tokenApiClient: null
	property var permissionsProvider: null

	// The ID of the saved product. Tokens, roles and the tree refer to it, so it stays once stored.
	property string storedProductId: ""

	onStoredProductIdChanged: {
		multiPageView.updatePages()
	}

	onVisibleChanged: {
		container.updateAdministeredProduct()
	}

	Component.onDestruction: {
		container.releaseAdministeredProduct()
	}

	// The administration works in the context of its product, the rest of the console in its own.
	function updateAdministeredProduct(){
		let isAdministering = container.visible
					&& multiPageView.currentIndex === multiPageView.getIndexById("Administration")
		if (isAdministering){
			AuthorizationController.administeredProductId = container.storedProductId
		}
		else{
			container.releaseAdministeredProduct()
		}
	}

	// Another opened product may be administered by now, its context stays
	function releaseAdministeredProduct(){
		if (AuthorizationController.administeredProductId === container.storedProductId){
			AuthorizationController.administeredProductId = ""
		}
	}

	function updateGui(){
		var generalPage = multiPageView.getPageById("General")
		if (generalPage){
			generalPage.updateGui()
		}
	}

	function updateModel(){
		var generalPage = multiPageView.getPageById("General")
		if (generalPage){
			generalPage.updateModel()
		}
	}

	MultiPageView {
		id: multiPageView
		objectName: "ConnectedProductPages"
		anchors.fill: parent
		panelWidth: Style.sizeHintXXS

		onCurrentIndexChanged: {
			container.updateAdministeredProduct()
		}

		function addPageOnce(pageId, name, pageComp, icon){
			if (multiPageView.getIndexById(pageId) < 0){
				multiPageView.addPage(pageId, name, pageComp, icon)
			}
		}

		function updatePages(){
			multiPageView.addPageOnce("General", qsTr("General"), generalPageComp, "Icons/Settings")
			if (container.storedProductId !== ""){
				multiPageView.addPageOnce("Tokens", qsTr("Tokens"), tokensPageComp, "Icons/Key")
				multiPageView.addPageOnce("Permissions", qsTr("Permissions"), permissionsPageComp, "Icons/Key")
				multiPageView.addPageOnce("Administration", qsTr("Administration"), administrationPageComp, "Icons/Account")
			}
		}

		Component.onCompleted: {
			multiPageView.updatePages()
			multiPageView.currentIndex = 0
		}
	}

	Component {
		id: tokensPageComp

		ConnectedProductTokensPage {
			productId: container.storedProductId
			apiClient: container.tokenApiClient
		}
	}

	Component {
		id: permissionsPageComp

		ConnectedProductPermissionsPage {
			productId: container.storedProductId
			productVersion: container.model && container.model.m_productVersion ? String(container.model.m_productVersion) : ""
			publishedAt: container.model && container.model.m_publishedAt ? String(container.model.m_publishedAt) : ""
			permissionsProvider: container.permissionsProvider
		}
	}

	Component {
		id: administrationPageComp

		AdministrationView {
			productId: container.storedProductId
		}
	}

	Component {
		id: generalPageComp

		Item {
			id: generalPage
			anchors.fill: parent

			function updateGui(){
				if (!container.model){
					return
				}

				productIdInput.text = container.model.m_productId ? container.model.m_productId : ""
				nameInput.text = container.model.m_name ? container.model.m_name : ""
				descriptionInput.text = container.model.m_description ? container.model.m_description : ""
			}

			function updateModel(){
				if (!container.model){
					return
				}

				container.model.m_productId = productIdInput.text
				container.model.m_name = nameInput.text
				container.model.m_description = descriptionInput.text
			}

			Component.onCompleted: {
				generalPage.updateGui()
			}

			Column {
				anchors.top: parent.top
				anchors.topMargin: Style.marginXL
				anchors.horizontalCenter: parent.horizontalCenter
				width: Math.min(parent.width - 2 * Style.marginXL, Style.contentWidthMax)
				spacing: Style.marginXL

				GroupHeaderView {
					width: parent.width
					title: qsTr("General")
					groupView: generalGroup
				}

				GroupElementView {
					id: generalGroup
					width: parent.width

					TextInputElementView {
						id: productIdInput
						objectName: "ConnectedProductIdInput"
						name: qsTr("Product-ID")
						readOnly: container.storedProductId !== ""
						placeHolderText: qsTr("Enter the ID of the product")
						onEditingFinished: {
							container.doUpdateModel()
						}
						KeyNavigation.tab: nameInput
					}

					TextInputElementView {
						id: nameInput
						objectName: "ConnectedProductNameInput"
						name: qsTr("Name")
						placeHolderText: qsTr("Enter the name")
						onEditingFinished: {
							container.doUpdateModel()
						}
						KeyNavigation.tab: descriptionInput
						KeyNavigation.backtab: productIdInput
					}

					TextInputElementView {
						id: descriptionInput
						objectName: "ConnectedProductDescriptionInput"
						name: qsTr("Description")
						placeHolderText: qsTr("Enter the description")
						onEditingFinished: {
							container.doUpdateModel()
						}
						KeyNavigation.backtab: nameInput
					}
				}
			}
		}
	}
}
