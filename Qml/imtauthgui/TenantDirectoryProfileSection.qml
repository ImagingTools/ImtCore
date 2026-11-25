// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * TenantDirectoryProfileSection
 *
 * The organization in the directory of the organizations, a section of TenantConnectionCodePage.
 * The two switches apply at once; under them the profile is shown as the others see it, and it is changed
 * in DirectoryProfileEditDialog. An empty profile is offered to be filled.
 */
Column {
	id: profileSection

	spacing: Style.marginM

	property string tenantId: ""
	// ContractsApiClient
	property var contractsApiClient: null
	property bool canEdit: false

	// The profile as saved: isListed, acceptsInvitationsFromUnknown, description, industry, country, website, offeredRoleIds
	property var __saved: ({})
	property bool __isLoaded: false
	// The profile could not be read: why, empty when it was read
	property string __readError: ""
	// A switch was turned: its change is being stored
	property bool __isSwitching: false

	readonly property string __tenantName: profileSection.contractsApiClient && profileSection.contractsApiClient.directoryProfile
		? (profileSection.contractsApiClient.directoryProfile.tenantName || "") : ""
	readonly property bool __hasContent: (profileSection.__saved.description || "") !== "" || (profileSection.__saved.industry || "") !== ""
		|| (profileSection.__saved.country || "") !== "" || (profileSection.__saved.website || "") !== ""
		|| (profileSection.__saved.offeredRoleIds || []).length > 0

	function updateGui() {
		if (!profileSection.contractsApiClient || profileSection.tenantId === "")
			return
		profileSection.contractsApiClient.fetchDirectoryProfile(profileSection.tenantId)
		profileSection.contractsApiClient.fetchPartyRoles(profileSection.tenantId, false)
	}

	Component.onCompleted: {
		profileSection.updateGui()
	}

	onTenantIdChanged: {
		profileSection.updateGui()
	}

	function __fillProfile() {
		var profile = profileSection.contractsApiClient ? profileSection.contractsApiClient.directoryProfile : null
		profileSection.__saved = {"isListed": profile ? profile.isListed === true : false,
								  "acceptsInvitationsFromUnknown": profile ? profile.acceptsInvitationsFromUnknown === true : false,
								  "description": profile ? (profile.description || "") : "",
								  "industry": profile ? (profile.industry || "") : "",
								  "country": profile ? (profile.country || "") : "",
								  "website": profile ? (profile.website || "") : "",
								  "offeredRoleIds": profile ? (profile.offeredRoleIds || []).slice() : []}
		profileSection.__readError = ""
		profileSection.__isLoaded = false
		listedSwitch.checked = profileSection.__saved.isListed
		unknownSwitch.checked = profileSection.__saved.acceptsInvitationsFromUnknown
		profileSection.__isLoaded = true
	}

	// A switch stores the saved profile with its new state at once
	function __storeSwitches() {
		if (!profileSection.__isLoaded || !profileSection.canEdit || profileSection.__isSwitching)
			return
		if (listedSwitch.checked === profileSection.__saved.isListed && unknownSwitch.checked === profileSection.__saved.acceptsInvitationsFromUnknown)
			return
		var profile = {}
		for (var key in profileSection.__saved)
			profile[key] = profileSection.__saved[key]
		profile.isListed = listedSwitch.checked
		profile.acceptsInvitationsFromUnknown = unknownSwitch.checked
		profileSection.__isSwitching = true
		profileSection.contractsApiClient.setDirectoryProfile(profileSection.tenantId, profile)
	}

	function __roleNames(roleIds) {
		var names = []
		var roles = profileSection.contractsApiClient ? profileSection.contractsApiClient.partyRoles : []
		for (var i = 0; i < roles.length; i++) {
			if ((roleIds || []).indexOf(roles[i].roleId) >= 0)
				names.push(roles[i].name)
		}
		return names.join(", ")
	}

	function __placeText() {
		var industry = profileSection.__saved.industry || ""
		var country = profileSection.__saved.country || ""
		if (industry !== "" && country !== "")
			return industry + " · " + country
		return industry !== "" ? industry : country
	}

	function editProfile() {
		ModalDialogManager.openDialog(editDialogComp, {"profile": profileSection.__saved})
	}

	Connections {
		target: profileSection.contractsApiClient

		function onDirectoryProfileReceived() {
			profileSection.__fillProfile()
		}

		function onDirectoryProfileReadFailed(message) {
			profileSection.__isLoaded = false
			profileSection.__readError = message !== "" ? message : qsTr("The server returned no data.")
		}

		function onDirectoryProfileSaved() {
			if (profileSection.__isSwitching) {
				profileSection.__isSwitching = false
				PopupManager.addSuccessMessage(listedSwitch.checked
											   ? qsTr("Your organization is shown in the directory")
											   : qsTr("The settings of the directory were saved"), true)
			}
			else {
				PopupManager.addSuccessMessage(qsTr("The profile was saved"), true)
			}
			profileSection.contractsApiClient.fetchDirectoryProfile(profileSection.tenantId)
		}

		function onCommandFailed(command, errorMessage) {
			if (command !== "SetDirectoryProfile")
				return
			PopupManager.addErrorMessage(errorMessage, true)
			if (profileSection.__isSwitching) {
				profileSection.__isSwitching = false
				profileSection.__fillProfile()
			}
		}
	}

	GroupHeaderView {
		width: parent.width
		title: qsTr("Directory of the organizations")
	}

	BaseText {
		width: parent.width
		text: qsTr("A listed organization is found by the others on their page Find an organization. Only this profile is shown: the members and the contracts stay hidden.")
		font.pixelSize: Style.fontSizeS
		color: Style.inactiveTextColor
		wrapMode: Text.WordWrap
	}

	// The profile could not be read: the switches and the card wait for it
	Rectangle {
		width: parent.width
		height: readErrorColumn.height + 2 * Style.marginL
		visible: profileSection.__readError !== ""
		color: Style.baseColor
		radius: Style.radiusM
		border.width: 1
		border.color: Style.errorTextColor

		Column {
			id: readErrorColumn

			anchors.top: parent.top
			anchors.topMargin: Style.marginL
			anchors.left: parent.left
			anchors.leftMargin: Style.marginL
			anchors.right: parent.right
			anchors.rightMargin: Style.marginL

			spacing: Style.marginS

			BaseText {
				width: parent.width
				wrapMode: Text.WordWrap
				text: qsTr("The profile of the organization could not be read: %1").arg(profileSection.__readError)
				font.pixelSize: Style.fontSizeM
				color: Style.errorTextColor
			}

			Button {
				text: qsTr("Try again")
				onClicked: {
					profileSection.updateGui()
				}
			}
		}
	}

	GroupElementView {
		width: parent.width
		visible: profileSection.__isLoaded

		SwitchElementView {
			id: listedSwitch
			name: qsTr("Show in the directory")
			description: listedSwitch.checked
						 ? (profileSection.__tenantName !== "" ? qsTr("The others find you by the name \"%1\"").arg(profileSection.__tenantName) : qsTr("The others find you in the search"))
						 : qsTr("Now the others cannot find you in the search")
			readOnly: !profileSection.canEdit || profileSection.__isSwitching
			onCheckedChanged: {
				profileSection.__storeSwitches()
			}
		}

		SwitchElementView {
			id: unknownSwitch
			name: qsTr("Contracts from unknown organizations")
			description: qsTr("Also organizations outside your address book")
			readOnly: !profileSection.canEdit || profileSection.__isSwitching
			onCheckedChanged: {
				profileSection.__storeSwitches()
			}
		}
	}

	// --- The profile as the others see it ---
	Rectangle {
		width: parent.width
		height: previewColumn.height + 2 * Style.marginL
		visible: profileSection.__isLoaded
		color: Style.baseColor
		radius: Style.radiusM
		border.width: 1
		border.color: Style.borderColor

		Column {
			id: previewColumn

			anchors.top: parent.top
			anchors.topMargin: Style.marginL
			anchors.left: parent.left
			anchors.leftMargin: Style.marginL
			anchors.right: parent.right
			anchors.rightMargin: Style.marginL

			spacing: Style.marginS

			Item {
				width: parent.width
				height: Math.max(previewName.height, previewState.height)

				BaseText {
					id: previewName
					anchors.left: parent.left
					anchors.right: previewState.left
					anchors.rightMargin: Style.marginM
					elide: Text.ElideRight
					text: profileSection.__tenantName
					font.pixelSize: Style.fontSizeL
					font.bold: true
					color: Style.textColor
				}

				BaseText {
					id: previewState
					anchors.right: parent.right
					anchors.verticalCenter: parent.verticalCenter
					text: listedSwitch.checked ? qsTr("Found in the search") : qsTr("Hidden from the search")
					font.pixelSize: Style.fontSizeS
					color: listedSwitch.checked ? Style.successColor : Style.inactiveTextColor
				}
			}

			BaseText {
				width: parent.width
				visible: profileSection.__hasContent && text !== ""
				wrapMode: Text.WordWrap
				text: profileSection.__placeText()
				font.pixelSize: Style.fontSizeS
				color: Style.inactiveTextColor
			}

			BaseText {
				width: parent.width
				visible: profileSection.__hasContent
				wrapMode: Text.WordWrap
				text: (profileSection.__saved.description || "") !== "" ? profileSection.__saved.description : qsTr("No description yet.")
				font.pixelSize: Style.fontSizeM
				color: (profileSection.__saved.description || "") !== "" ? Style.textColor : Style.inactiveTextColor
			}

			BaseText {
				width: parent.width
				visible: profileSection.__hasContent && text !== ""
				wrapMode: Text.WordWrap
				text: (profileSection.__saved.offeredRoleIds || []).length > 0
					  ? qsTr("Offers: %1").arg(profileSection.__roleNames(profileSection.__saved.offeredRoleIds)) : ""
				font.pixelSize: Style.fontSizeS
				color: Style.textColor
			}

			BaseText {
				width: parent.width
				visible: profileSection.__hasContent && text !== ""
				elide: Text.ElideRight
				text: profileSection.__saved.website || ""
				font.pixelSize: Style.fontSizeS
				color: Style.linkColor
			}

			// An empty profile: what to fill and why
			BaseText {
				width: parent.width
				visible: !profileSection.__hasContent
				wrapMode: Text.WordWrap
				text: qsTr("Tell the others about your organization: what it does, its industry and the roles it offers. An empty profile is hard to choose.")
				font.pixelSize: Style.fontSizeM
				color: Style.textColor
			}

			Item {
				width: parent.width
				height: editButton.height

				Button {
					id: editButton
					anchors.right: parent.right
					visible: profileSection.canEdit
					variant: profileSection.__hasContent ? "default" : "primary"
					text: profileSection.__hasContent ? qsTr("Edit profile") : qsTr("Fill in the profile")
					onClicked: {
						profileSection.editProfile()
					}
				}
			}
		}
	}

	Component {
		id: editDialogComp

		DirectoryProfileEditDialog {
			tenantId: profileSection.tenantId
			contractsApiClient: profileSection.contractsApiClient
		}
	}
}
