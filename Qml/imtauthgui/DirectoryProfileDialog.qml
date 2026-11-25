// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * DirectoryProfileDialog
 *
 * The profile of an organization found in the directory, and what to do next with it.
 * OK asks it to join the address book; it is offered only when the organization is not there yet.
 */
Dialog {
	id: profileDialog

	width: Math.max(Style.sizeHintM, Math.min(ModalDialogManager.activeView.width - 100, Style.sizeHintL))

	// The row of the directory: tenantName, industry, country, offeredRoles, description, website, acceptsInvitationsFromUnknown
	property var profile: null
	// How the organization stands to this one: "partner", "sent", "received" or ""
	property string relation: ""
	property bool canConnect: false

	readonly property bool __canAdd: profileDialog.canConnect && profileDialog.relation === ""

	// The parameters of openDialog are set after the dialog is created: it is set up when it starts
	onStarted: {
		if (profileDialog.__canAdd)
			addButton(Enums.ok, qsTr("Add to address book"), true, false, "primary")
		addButton(Enums.cancel, qsTr("Close"), true)
	}

	function __placeText() {
		var industry = profileDialog.profile ? (profileDialog.profile.industry || "") : ""
		var country = profileDialog.profile ? (profileDialog.profile.country || "") : ""
		if (industry !== "" && country !== "")
			return industry + " · " + country
		return industry !== "" ? industry : country
	}

	function __nextStepText() {
		if (profileDialog.relation === "partner")
			return qsTr("Already in your address book: you can make a contract with it.")
		if (profileDialog.relation === "sent")
			return qsTr("Your request waits for its answer.")
		if (profileDialog.relation === "received")
			return qsTr("It asked to join your address book: answer the request in the address book.")
		if (profileDialog.profile && profileDialog.profile.acceptsInvitationsFromUnknown)
			return qsTr("Ask it to join your address book. It also accepts contracts from organizations it does not know yet.")
		return qsTr("Ask it to join your address book: after it accepts, you can make contracts with it.")
	}

	contentComp: Component {
		Item {
			width: profileDialog.width
			height: Math.min(profileColumn.height + 2 * Style.marginXL, ModalDialogManager.activeView.height - 200)

			CustomScrollbar {
				z: profileFlickable.z + 1
				anchors.right: parent.right
				anchors.top: profileFlickable.top
				anchors.bottom: profileFlickable.bottom
				secondSize: Style.marginM
				targetItem: profileFlickable
			}

			Flickable {
				id: profileFlickable

				anchors.fill: parent
				contentHeight: profileColumn.height + 2 * Style.marginXL
				boundsBehavior: Flickable.StopAtBounds
				clip: true

				Column {
					id: profileColumn

					y: Style.marginXL
					x: Style.marginXL
					width: profileFlickable.width - 2 * Style.marginXL

					spacing: Style.marginM

					BaseText {
						width: parent.width
						wrapMode: Text.WordWrap
						text: profileDialog.profile ? (profileDialog.profile.tenantName || "") : ""
						font.pixelSize: Style.fontSizeL
						font.bold: true
						color: Style.textColor
					}

					BaseText {
						width: parent.width
						wrapMode: Text.WordWrap
						visible: text !== ""
						text: profileDialog.__placeText()
						font.pixelSize: Style.fontSizeS
						color: Style.inactiveTextColor
					}

					GroupElementView {
						width: parent.width

						ElementView {
							name: qsTr("About")
							controlComp: Component {
								BaseText {
									width: Style.sizeHintS
									wrapMode: Text.WordWrap
									text: profileDialog.profile && profileDialog.profile.description
										  ? profileDialog.profile.description : qsTr("The organization tells nothing about itself.")
									font.pixelSize: Style.fontSizeM
									color: Style.textColor
								}
							}
						}

						ElementView {
							name: qsTr("Offers")
							visible: profileDialog.profile !== null && !!profileDialog.profile.offeredRoles
							controlComp: Component {
								BaseText {
									width: Style.sizeHintS
									wrapMode: Text.WordWrap
									text: profileDialog.profile ? (profileDialog.profile.offeredRoles || "") : ""
									font.pixelSize: Style.fontSizeM
									color: Style.textColor
								}
							}
						}

						ElementView {
							name: qsTr("Website")
							visible: profileDialog.profile !== null && !!profileDialog.profile.website
							controlComp: Component {
								BaseText {
									width: Style.sizeHintS
									elide: Text.ElideRight
									text: profileDialog.profile ? (profileDialog.profile.website || "") : ""
									font.pixelSize: Style.fontSizeM
									color: Style.linkColor
								}
							}
						}
					}

					BaseText {
						width: parent.width
						wrapMode: Text.WordWrap
						text: profileDialog.__nextStepText()
						font.pixelSize: Style.fontSizeS
						color: Style.inactiveTextColor
					}
				}
			}
		}
	}
}
