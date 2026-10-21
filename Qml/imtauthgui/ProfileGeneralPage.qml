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

	// Width of the label column of the form rows, as in the document editors.
	readonly property int labelWidth: Math.min(Style.sizeHintS, Math.round(bodyColumn.width * 0.35))

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

		// Same layout as the document editors: a section title over a card of label | field rows.
		Column {
			id: bodyColumn
			anchors.horizontalCenter: parent.horizontalCenter
			width: Math.min(parent.width, Style.contentWidthMax)
			spacing: Style.marginM

			GroupHeaderView {
				width: parent.width
				title: qsTr("Account")
			}

			RegularExpressionValidator {
				id: mailValid
				regularExpression: /\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*/
			}

			Rectangle {
				width: parent.width
				height: accountRows.height
				radius: Style.radiusM
				color: Style.baseColor
				border.width: 1
				border.color: Style.borderColor

				Column {
					id: accountRows
					width: parent.width

						Item {
							width: parent.width
							height: Style.controlHeightM + 2 * Style.marginM

							Text {
								anchors.left: parent.left
								anchors.leftMargin: Style.marginL
								anchors.verticalCenter: parent.verticalCenter
								width: generalPage.labelWidth
								text: qsTr("Name")
								color: Style.textColor
								font.family: Style.fontFamilyBold
								font.pixelSize: Style.fontSizeM
								font.bold: true
								elide: Text.ElideRight
							}

							CustomTextField {
								id: nameInput
								objectName: "NameInput"
								anchors.right: parent.right
								anchors.rightMargin: Style.marginL
								anchors.verticalCenter: parent.verticalCenter
								width: parent.width - generalPage.labelWidth - 3 * Style.marginL
								height: Style.controlHeightM
								readOnly: generalPage.readOnly
								placeHolderText: qsTr("Enter the name")
							}
						}

						Rectangle {
							x: Style.marginL
							width: parent.width - 2 * Style.marginL
							height: 1
							color: Style.borderColor
						}

						Item {
							width: parent.width
							height: Style.controlHeightM + 2 * Style.marginM

							Text {
								anchors.left: parent.left
								anchors.leftMargin: Style.marginL
								anchors.verticalCenter: parent.verticalCenter
								width: generalPage.labelWidth
								text: qsTr("Email Address")
								color: Style.textColor
								font.family: Style.fontFamilyBold
								font.pixelSize: Style.fontSizeM
								font.bold: true
								elide: Text.ElideRight
							}

							CustomTextField {
								id: mailInput
								objectName: "EmailInput"
								anchors.right: parent.right
								anchors.rightMargin: Style.marginL
								anchors.verticalCenter: parent.verticalCenter
								width: parent.width - generalPage.labelWidth - 3 * Style.marginL
								height: Style.controlHeightM
								readOnly: generalPage.readOnly
								textInputValidator: mailValid
								placeHolderText: qsTr("Enter the email")
							}
						}
				}
			}

			Item {
				width: parent.width
				height: saveElementView.height

				Button {
					id: saveElementView
					anchors.right: parent.right
					text: qsTr("Save")
					variant: "primary"
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

			// --- Security: password change, inline (no modal) ---
			Column {
				id: passwordSection
				width: parent.width
				spacing: Style.marginM
				visible: generalPage.profileData ? generalPage.profileData.m_systemId === "" : false

				readonly property bool needsCurrentPassword: !AuthorizationController.loggedUserIsSuperuser()

				// The new-password rows are shown while the user is changing the password.
				property bool editing: false

				// True while waiting for AuthorizationController's async result -
				// guards against reacting to an unrelated password-change signal
				// and against double-submitting while a request is in flight.
				property bool submitting: false

				// Functions, not properties: these read Items declared later in this file.
				function passwordsFilled() {
					return newPasswordInput.text !== "" && confirmPasswordInput.text !== ""
				}

				function passwordsMatch() {
					return newPasswordInput.text === confirmPasswordInput.text
				}

				// PasswordPolicyController instance, supplied by the injected transport.
				readonly property var policy: generalPage.apiClient ? generalPage.apiClient.passwordPolicy : null

				// Mirrors the server policy so the user is not sent to the server just to be rejected.
				function policyViolations() {
					if (!passwordSection.policy)
						return []

					let login = generalPage.profileData ? generalPage.profileData.m_username : ""
					return passwordSection.policy.validate(login, newPasswordInput.text)
				}

				function policyHint() {
					if (!passwordSection.policy)
						return ""

					let violations = passwordSection.policyViolations()
					if (violations.length > 0)
						return passwordSection.policy.describeFailure(violations, "")

					return passwordSection.policy.requirementsText()
				}

				function canSubmit() {
					return !passwordSection.submitting && newPasswordInput.text !== "" && passwordSection.passwordsMatch()
						&& (!needsCurrentPassword || currentPasswordInput.text !== "")
						&& passwordSection.policyViolations().length === 0
				}

				// apiClient is injected by binding, so the policy may appear after this page.
				onPolicyChanged: {
					if (passwordSection.policy)
						passwordSection.policy.load()
				}

				Component.onCompleted: {
					if (passwordSection.policy)
						passwordSection.policy.load()
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
						passwordSection.editing = false
					}
					function onChangePasswordFailed(message, violatedRules) {
						if (!passwordSection.submitting)
							return
						passwordSection.submitting = false

						let fallback = qsTr("Unable to change the password. Check your current password and try again.")
						banner.show(passwordSection.policy
									? passwordSection.policy.describeFailure(violatedRules, fallback)
									: fallback, true)
					}
				}

				Item {
					width: parent.width
					height: Style.marginS
				}

				GroupHeaderView {
					width: parent.width
					title: qsTr("Password")
				}

				Rectangle {
					width: parent.width
					height: passwordRows.height
					radius: Style.radiusM
					color: Style.baseColor
					border.width: 1
					border.color: Style.borderColor

					Column {
						id: passwordRows
						width: parent.width

						Item {
							width: parent.width
							height: Style.controlHeightM + 2 * Style.marginM
							visible: !passwordSection.editing

							Text {
								anchors.left: parent.left
								anchors.leftMargin: Style.marginL
								anchors.verticalCenter: parent.verticalCenter
								width: generalPage.labelWidth
								text: qsTr("Password")
								color: Style.textColor
								font.family: Style.fontFamilyBold
								font.pixelSize: Style.fontSizeM
								font.bold: true
							}

							Text {
								anchors.left: parent.left
								anchors.leftMargin: generalPage.labelWidth + 2 * Style.marginL
								anchors.verticalCenter: parent.verticalCenter
								text: "••••••••••"
								color: Style.subtitleColor
								font.pixelSize: Style.fontSizeM
							}

							Button {
								anchors.right: parent.right
								anchors.rightMargin: Style.marginL
								anchors.verticalCenter: parent.verticalCenter
								text: qsTr("Change")
								decorator: Component {
									ButtonDecorator {
										widthDefault: Style.buttonWidthXXL
									}
								}
								onClicked: {
									passwordSection.reset()
									passwordSection.editing = true
								}
							}
						}

						Item {
							width: parent.width
							height: Style.controlHeightM + 2 * Style.marginM
							visible: passwordSection.editing && passwordSection.needsCurrentPassword

							Text {
								anchors.left: parent.left
								anchors.leftMargin: Style.marginL
								anchors.verticalCenter: parent.verticalCenter
								width: generalPage.labelWidth
								text: qsTr("Current password")
								color: Style.textColor
								font.family: Style.fontFamilyBold
								font.pixelSize: Style.fontSizeM
								font.bold: true
								elide: Text.ElideRight
							}

							CustomTextField {
								id: currentPasswordInput
								objectName: "CurrentPasswordInput"
								anchors.right: parent.right
								anchors.rightMargin: Style.marginL
								anchors.verticalCenter: parent.verticalCenter
								width: parent.width - generalPage.labelWidth - 3 * Style.marginL
								height: Style.controlHeightM
								echoMode: TextInput.Password
								placeHolderText: qsTr("Enter your current password")
							}
						}

						Rectangle {
							x: Style.marginL
							width: parent.width - 2 * Style.marginL
							height: 1
							color: Style.borderColor
							visible: passwordSection.editing && passwordSection.needsCurrentPassword
						}

						Item {
							width: parent.width
							height: Style.controlHeightM + 2 * Style.marginM
							visible: passwordSection.editing

							Text {
								anchors.left: parent.left
								anchors.leftMargin: Style.marginL
								anchors.verticalCenter: parent.verticalCenter
								width: generalPage.labelWidth
								text: qsTr("New password")
								color: Style.textColor
								font.family: Style.fontFamilyBold
								font.pixelSize: Style.fontSizeM
								font.bold: true
								elide: Text.ElideRight
							}

							CustomTextField {
								id: newPasswordInput
								objectName: "NewPasswordInput"
								anchors.right: parent.right
								anchors.rightMargin: Style.marginL
								anchors.verticalCenter: parent.verticalCenter
								width: parent.width - generalPage.labelWidth - 3 * Style.marginL
								height: Style.controlHeightM
								echoMode: TextInput.Password
								placeHolderText: qsTr("Enter a new password")
							}
						}

						Rectangle {
							x: Style.marginL
							width: parent.width - 2 * Style.marginL
							height: 1
							color: Style.borderColor
							visible: passwordSection.editing
						}

						Item {
							width: parent.width
							height: Style.controlHeightM + 2 * Style.marginM
							visible: passwordSection.editing

							Text {
								anchors.left: parent.left
								anchors.leftMargin: Style.marginL
								anchors.verticalCenter: parent.verticalCenter
								width: generalPage.labelWidth
								text: qsTr("Confirm new password")
								color: Style.textColor
								font.family: Style.fontFamilyBold
								font.pixelSize: Style.fontSizeM
								font.bold: true
								elide: Text.ElideRight
							}

							CustomTextField {
								id: confirmPasswordInput
								objectName: "ConfirmPasswordInput"
								anchors.right: parent.right
								anchors.rightMargin: Style.marginL
								anchors.verticalCenter: parent.verticalCenter
								width: parent.width - generalPage.labelWidth - 3 * Style.marginL
								height: Style.controlHeightM
								echoMode: TextInput.Password
								placeHolderText: qsTr("Re-enter the new password")
							}
						}

						Column {
							x: Style.marginL
							width: parent.width - 2 * Style.marginL
							spacing: Style.marginXS
							// Not from the children's visible: a hidden parent reports its children hidden too.
							visible: passwordSection.editing && ((passwordSection.passwordsFilled() && !passwordSection.passwordsMatch()) || passwordSection.policyHint() !== "")

							Text {
								id: mismatchText
								text: qsTr("Passwords don't match")
								visible: passwordSection.passwordsFilled() && !passwordSection.passwordsMatch()
								font.pixelSize: Style.fontSizeS
								color: Style.errorTextColor
							}

							Text {
								id: policyText
								width: parent.width
								wrapMode: Text.WordWrap
								text: passwordSection.policyHint()
								visible: text !== ""
								font.pixelSize: Style.fontSizeS
								color: passwordSection.policyViolations().length > 0 ? Style.errorTextColor : Style.inactiveTextColor
							}

							Item {
								width: 1
								height: Style.marginS
							}
						}
					}
				}

				Item {
					width: parent.width
					height: passwordButtons.height
					visible: passwordSection.editing

					Row {
						id: passwordButtons
						anchors.right: parent.right
						spacing: Style.marginM

						Button {
							text: passwordSection.submitting ? qsTr("Changing password…") : qsTr("Update password")
							variant: "primary"
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
								passwordSection.editing = false
							}
						}
					}
				}
			}
		}
	}
}
