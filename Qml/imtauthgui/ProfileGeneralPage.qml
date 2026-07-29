// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0
import imtauthgui 1.0

/**
 * ProfileGeneralPage
 *
 * General tab of the ProfileView — editable name / email, and an inline password
 * change section (no separate modal). The avatar / username identity is already
 * shown once in the ProfileView banner above the page list, so it is not repeated
 * here.
 *
 * Editing does NOT write into the shared profile document as the user types —
 * only Save (via apiClient.setProfile) does. This keeps the ProfileView banner
 * showing the last-saved name, not whatever is currently typed but unsaved.
 *
 * Transport-free: all server operations go through the injected `apiClient`
 * (ProfileApiClient contract). The page owns its document as ViewBase.model.
 * `applyProfile()` is the single entry point that (re)populates it — called both
 * from this page's own apiClient subscription and, redundantly, by the
 * orchestrator (ProfileView) right after the fetch resolves, so the first-ever
 * load is not dependent on exactly one propagation path.
 */
ViewBase {
	id: generalPage

	anchors.fill: parent
	commandsPanelVisible: false
	contentColor: Style.baseColor

	property var apiClient: null
	readonly property var profileData: generalPage.model

	function applyProfile(profile) {
		if (!profile)
			return
		generalPage.model = profile
		generalPage.doUpdateGui()
	}

	Component.onCompleted: {
		if (generalPage.apiClient && generalPage.apiClient.lastProfile)
			generalPage.applyProfile(generalPage.apiClient.lastProfile)
	}

	Connections {
		target: generalPage.apiClient ? generalPage.apiClient : null
		function onProfileReceived(profile) {
			generalPage.applyProfile(profile)
		}
		function onProfileSaved() {
			banner.show(qsTr("Profile saved"), false)
		}
		function onProfileOperationFailed(message) {
			banner.show(message || qsTr("Unable to save the profile."), true)
		}
	}

	// Populates the input fields from the last-loaded/last-saved document.
	// Deliberately one-way: typing into the fields never writes back here —
	// see the file header comment.
	function updateGui() {
		if (!generalPage.profileData)
			return
		nameInput.text = generalPage.profileData.m_name
		mailInput.text = generalPage.profileData.m_email
	}

	function save() {
		if (!generalPage.profileData || !generalPage.apiClient)
			return
		generalPage.apiClient.setProfile(generalPage.profileData.m_id, nameInput.text, mailInput.text)
	}

	ProfileNoticeBanner {
		id: banner
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.margins: Style.marginXL
	}

	CustomScrollbar {
		id: scrollbar
		z: parent.z + 1
		anchors.right: parent.right
		anchors.top: flickable.top
		anchors.bottom: flickable.bottom
		secondSize: Style.marginM
		targetItem: flickable
	}

	Flickable {
		id: flickable
		anchors.top: banner.visible ? banner.bottom : parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.margins: Style.marginXL
		anchors.topMargin: banner.visible ? Style.marginM : Style.marginXL
		contentWidth: width
		contentHeight: bodyColumn.height + Style.marginXL
		boundsBehavior: Flickable.StopAtBounds
		clip: true

		Column {
			id: bodyColumn
			width: parent.width
			spacing: Style.marginXL

			// --- Editable details ---
			Column {
				width: parent.width
				spacing: Style.marginL

				Column {
					width: parent.width
					spacing: Style.marginXS

					Text {
						text: qsTr("Name")
						font.pixelSize: Style.fontSizeS
						color: Style.inactiveTextColor
					}

					CustomTextField {
						id: nameInput
						objectName: "NameInput"
						width: parent.width
						height: Style.controlHeightM
						readOnly: generalPage.readOnly
						placeHolderText: qsTr("Enter the name")
					}
				}

				Column {
					width: parent.width
					spacing: Style.marginXS

					Text {
						text: qsTr("Email Address")
						font.pixelSize: Style.fontSizeS
						color: Style.inactiveTextColor
					}

					RegularExpressionValidator {
						id: mailValid
						regularExpression: /\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*/
					}

					CustomTextField {
						id: mailInput
						objectName: "EmailInput"
						width: parent.width
						height: Style.controlHeightM
						readOnly: generalPage.readOnly
						textInputValidator: mailValid
						placeHolderText: qsTr("Enter the email")
					}
				}

				Row {
					spacing: Style.marginM

					Button {
						id: saveElementView
						text: qsTr("Save")
						enabled: generalPage.profileData
							? (nameInput.text !== generalPage.profileData.m_name || mailInput.text !== generalPage.profileData.m_email)
							: false
						decorator: Component {
							ButtonDecorator {
								widthDefault: Style.buttonWidthXXL
							}
						}
						onClicked: {
							generalPage.save()
						}
					}
				}
			}

			Rectangle {
				width: parent.width
				height: 1
				color: Style.borderColor
			}

			// --- Security: password change, inline (no modal) ---
			Column {
				id: passwordSection
				width: parent.width
				spacing: Style.marginXS
				visible: generalPage.profileData ? generalPage.profileData.m_systemId === "" : false

				readonly property bool needsCurrentPassword: !AuthorizationController.loggedUserIsSuperuser()

				// True while waiting for AuthorizationController's async result —
				// guards against reacting to an unrelated password-change signal
				// and against double-submitting while a request is in flight.
				property bool submitting: false

				// Functions, not properties: these read Items (newPasswordInput etc.)
				// declared later in this file. A property binding evaluated eagerly
				// against a not-yet-constructed sibling id can fail, so the checks
				// are deferred to call time instead.
				function passwordsFilled() {
					return newPasswordInput.text !== "" && confirmPasswordInput.text !== ""
				}

				function passwordsMatch() {
					return newPasswordInput.text === confirmPasswordInput.text
				}

				function canSubmit() {
					return !passwordSection.submitting && newPasswordInput.text !== "" && passwordSection.passwordsMatch()
						&& (!needsCurrentPassword || currentPasswordInput.text !== "")
				}

				function reset() {
					currentPasswordInput.text = ""
					newPasswordInput.text = ""
					confirmPasswordInput.text = ""
				}

				function submit() {
					if (!passwordSection.canSubmit())
						return
					passwordSection.submitting = true
					AuthorizationController.changePassword(generalPage.profileData.m_username, currentPasswordInput.text, newPasswordInput.text)
				}

				Connections {
					target: AuthorizationController
					function onChangePasswordSuccessfully() {
						if (!passwordSection.submitting)
							return
						passwordSection.submitting = false
						banner.show(qsTr("Password changed"), false)
						passwordSection.reset()
						passwordCard.visible = false
					}
					function onChangePasswordFailed() {
						if (!passwordSection.submitting)
							return
						passwordSection.submitting = false
						banner.show(qsTr("Unable to change the password. Check your current password and try again."), true)
					}
				}

				Text {
					text: qsTr("Password")
					font.pixelSize: Style.fontSizeS
					color: Style.inactiveTextColor
				}

				Row {
					visible: !passwordCard.visible
					spacing: Style.marginM

					Text {
						anchors.verticalCenter: parent.verticalCenter
						text: qsTr("••••••••••")
						font.pixelSize: Style.fontSizeM
						color: Style.textColor
					}

					Button {
						anchors.verticalCenter: parent.verticalCenter
						text: qsTr("Change")
						decorator: Component {
							ButtonDecorator {
								height: Style.buttonHeightS
								widthDefault: Style.buttonWidthXXL
							}
						}
						onClicked: {
							passwordSection.reset()
							passwordCard.visible = true
						}
					}
				}

				Rectangle {
					id: passwordCard
					width: parent.width
					height: passwordCardColumn.height + 2 * Style.marginL
					visible: false
					radius: Style.radiusL
					color: Style.baseColor
					border.width: 1
					border.color: Style.borderColor

					Column {
						id: passwordCardColumn
						anchors.left: parent.left
						anchors.right: parent.right
						anchors.top: parent.top
						anchors.margins: Style.marginL
						spacing: Style.marginM

						Column {
							width: parent.width
							spacing: Style.marginXS
							visible: passwordSection.needsCurrentPassword

							Text {
								text: qsTr("Current password")
								font.pixelSize: Style.fontSizeS
								color: Style.inactiveTextColor
							}

							CustomTextField {
								id: currentPasswordInput
								objectName: "CurrentPasswordInput"
								width: parent.width
								height: Style.controlHeightM
								echoMode: TextInput.Password
								placeHolderText: qsTr("Enter your current password")
							}
						}

						Row {
							width: parent.width
							spacing: Style.marginM

							Column {
								width: (parent.width - Style.marginM) / 2
								spacing: Style.marginXS

								Text {
									text: qsTr("New password")
									font.pixelSize: Style.fontSizeS
									color: Style.inactiveTextColor
								}

								CustomTextField {
									id: newPasswordInput
									objectName: "NewPasswordInput"
									width: parent.width
									height: Style.controlHeightM
									echoMode: TextInput.Password
									placeHolderText: qsTr("Enter a new password")
								}
							}

							Column {
								width: (parent.width - Style.marginM) / 2
								spacing: Style.marginXS

								Text {
									text: qsTr("Confirm new password")
									font.pixelSize: Style.fontSizeS
									color: Style.inactiveTextColor
								}

								CustomTextField {
									id: confirmPasswordInput
									objectName: "ConfirmPasswordInput"
									width: parent.width
									height: Style.controlHeightM
									echoMode: TextInput.Password
									placeHolderText: qsTr("Re-enter the new password")
								}
							}
						}

						Text {
							text: qsTr("Passwords don't match")
							visible: passwordSection.passwordsFilled() && !passwordSection.passwordsMatch()
							font.pixelSize: Style.fontSizeS
							color: Style.errorTextColor
						}

						Row {
							spacing: Style.marginM

							Button {
								text: passwordSection.submitting ? qsTr("Changing password…") : qsTr("Update password")
								enabled: passwordSection.canSubmit()
								decorator: Component {
									ButtonDecorator {
										widthDefault: Style.buttonWidthXXL
									}
								}
								onClicked: {
									passwordSection.submit()
								}
							}

							Button {
								text: qsTr("Cancel")
								enabled: !passwordSection.submitting
								decorator: Component {
									ButtonDecorator {
										widthDefault: Style.buttonWidthXXL
									}
								}
								onClicked: {
									passwordSection.reset()
									passwordCard.visible = false
								}
							}
						}
					}
				}
			}
		}
	}
}
