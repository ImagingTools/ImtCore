import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0
import imtauthgui 1.0
import imtcontrols 1.0
import imtauthUsersSdl 1.0

Rectangle {
	id: root;
	objectName: "SuperuserPasswordPage"

	anchors.fill: parent;
	color: "transparent";

	property bool isSubmitting: false;
	property string errorMessage: "";

	signal passwordSetted();

	UserData {
		id: userDataModel;
		m_username: "su";
		m_name: "superuser";
	}

	Component.onCompleted: {
		userGeneralEditor.mailInput.forceActiveFocus();
	}

	function setSuperuserPassword(){
		root.errorMessage = "";
		root.isSubmitting = true;
		createSuperuserRequest.send();
	}

	GqlSdlRequestSender {
		id: createSuperuserRequest;
		requestType: 1; // Mutation
		gqlCommandId: ImtauthUsersSdlCommandIds.s_createSuperuser;

		inputObjectComp: Component {
			CreateSuperuserInput {
				m_password: userGeneralEditor.passwordInput.text;
				m_mail: userGeneralEditor.mailInput.text;
				m_name: userGeneralEditor.nameInput.text;
			}
		}

		sdlObjectComp: Component {
			CreateSuperuserPayload {
				onFinished: {
					root.isSubmitting = false;

					if (m_success){
						root.passwordSetted();
					}
					else{
						root.errorMessage = m_message || qsTr("Unable to create the administrator account");
					}
				}
			}
		}

		// Without this, a transport-level failure (network drop, 500, timeout) left
		// isSubmitting stuck true forever - no error, no way to retry, since nothing
		// ever cleared it (this is what used to hang behind a permanent full-screen
		// loading overlay before this page managed its own loading/error state).
		function onError(message, type){
			root.isSubmitting = false;
			root.errorMessage = message || qsTr("Unable to reach server");
		}
	}

	// Dimmed backdrop + centered card, same language as AuthorizationPage - this is
	// the other half of the same first-run flow (setup, then sign in).
	Rectangle {
		anchors.fill: parent;
		color: Style.overlayBackgroundColor;
		opacity: 0.5;
	}

	MouseArea {
		anchors.fill: parent;

		onClicked: {}
	}

	Rectangle {
		id: setupContainer;

		anchors.horizontalCenter: parent.horizontalCenter;
		anchors.verticalCenter: parent.verticalCenter;

		width: Style.sizeHintXXL;
		height: heroItem.height + bodyColumn.height;

		radius: Style.radiusXL;
		color: Style.baseColor;
		border.width: 1;
		border.color: Style.borderColor;

		clip: true;

		// Hero: an icon badge + purpose-built heading instead of the login page's
		// generic "Welcome to X" - this only ever appears once, on a fresh server
		// with no administrator yet, so it should read as a distinct, deliberate
		// setup step rather than a reskinned login form. AdminPanel is one of the
		// icon set's existing template icons (see Include/imtgui/Resources/Icons/
		// Template), same family as the Error/Alert/Ok icons already used by
		// PopupManager - not a one-off addition.
		Item {
			id: heroItem;

			width: parent.width;
			height: heroColumn.height + Style.marginXL + Style.marginL;

			Column {
				id: heroColumn;

				anchors.top: parent.top;
				anchors.topMargin: Style.marginXL;
				anchors.horizontalCenter: parent.horizontalCenter;

				width: parent.width - 2 * Style.marginXL;
				spacing: Style.marginM;

				Item {
					id: iconBadgeItem;
					width: parent.width;
					height: Style.iconSizeXXL;

					Rectangle {
						id: iconBadge;
						anchors.horizontalCenter: parent.horizontalCenter;
						width: Style.iconSizeXXL;
						height: Style.iconSizeXXL;
						radius: width / 2;
						color: Style.imaginToolsAccentColor;
						opacity: Style.popupIconChipOpacity;
					}

					Image {
						anchors.centerIn: iconBadge;
						width: Style.iconSizeMedium;
						height: width;
						sourceSize.width: width;
						sourceSize.height: height;
						source: "../../../" + Style.getIconPath("Icons/AdminPanel", Icon.State.On, Icon.Mode.Normal);
					}
				}

				Text {
					width: parent.width;

					color: Style.titleColor;
					font.family: Style.fontFamilyBold;
					font.pixelSize: Style.fontSizeXXL;

					horizontalAlignment: Text.AlignHCenter;
					wrapMode: Text.Wrap;

					text: qsTr("Create the administrator account");
				}

				Text {
					width: parent.width;

					color: Style.subtitleColor;
					font.family: Style.fontFamily;
					font.pixelSize: Style.fontSizeM;

					horizontalAlignment: Text.AlignHCenter;
					wrapMode: Text.Wrap;

					text: qsTr("This server doesn't have one yet. Set a name and password below - you'll sign in with them right after.");
				}
			}

			Rectangle {
				anchors.left: parent.left;
				anchors.right: parent.right;
				anchors.bottom: parent.bottom;
				height: 1;
				color: Style.borderColor;
			}
		}

		Column {
			id: bodyColumn;

			anchors.top: heroItem.bottom;
			anchors.topMargin: Style.marginL;
			anchors.horizontalCenter: parent.horizontalCenter;

			spacing: Style.marginL;
			width: setupContainer.width - 2 * Style.marginXL;

			UserGeneralEditor {
				id: userGeneralEditor;
				width: parent.width;
				userData: userDataModel;
				canHideGroup: false;
				readOnly: root.isSubmitting;
				usernameInput.readOnly: true;

				// Per-field inline validation text (TextInputElementView.showErrorWhenInvalid)
				// fires on every keystroke - live email regex, in particular - and resizes
				// its field immediately, which made the whole card (and the button below it)
				// visibly jump while typing. The Create button is already disabled until the
				// form is valid, so per-keystroke error text isn't pulling its weight here.
				usernameInput.showErrorWhenInvalid: false;
				nameInput.showErrorWhenInvalid: false;
				mailInput.showErrorWhenInvalid: false;
				confirmPasswordInput.showErrorWhenInvalid: false;

				onEmitUpdateModel: {
					updateModel();
				}

				onUserDataChanged: {
					updateGui();
				}
			}

			// Same accent-rail + icon-chip banner language as the app's popup error
			// messages (see PopupContainer.qml / Style.popupError*) - so a failed
			// attempt reads as "the same kind of error you always see here", not a
			// one-off. Sits right below the (still filled-in) form so nothing needs
			// retyping.
			Rectangle {
				id: errorBanner;

				visible: root.errorMessage !== "";
				width: parent.width;
				height: errorRow.height + 2 * Style.marginM;

				radius: Style.radiusM + 2;
				color: Style.popupErrorBackgroundColor;
				border.width: 1;
				border.color: Style.borderColor;
				clip: true;

				Rectangle {
					id: errorAccentRail;
					anchors.left: parent.left;
					anchors.top: parent.top;
					anchors.bottom: parent.bottom;
					width: 4;
					color: Style.popupErrorAccentColor;
				}

				Row {
					id: errorRow;

					anchors.left: parent.left;
					anchors.right: parent.right;
					anchors.verticalCenter: parent.verticalCenter;
					anchors.leftMargin: Style.marginM + errorAccentRail.width;
					anchors.rightMargin: Style.marginM;
					spacing: Style.marginS;

					Image {
						width: Style.iconSizeM;
						height: width;
						anchors.verticalCenter: parent.verticalCenter;
						sourceSize.width: width;
						sourceSize.height: height;
						source: "../../../" + Style.getIconPath("Icons/Error", Icon.State.On, Icon.Mode.Normal);
					}

					Text {
						width: errorRow.width - Style.iconSizeM - errorRow.spacing;

						text: root.errorMessage;
						color: Style.errorTextColor;
						font.family: Style.fontFamily;
						font.pixelSize: Style.fontSizeM;
						wrapMode: Text.Wrap;
					}
				}
			}

			Item {
				width: parent.width;
				height: buttonContent.height + Style.marginL;

				Column {
					id: buttonContent;
					anchors.centerIn: parent;
					spacing: Style.marginM;
					width: parent.width;

					Button {
						id: okButton;
						anchors.horizontalCenter: parent.horizontalCenter;

						enabled: !root.isSubmitting &&
								 userGeneralEditor.mailInput.acceptableInput &&
								 userGeneralEditor.confirmPasswordInput.acceptableInput &&
								 userGeneralEditor.passwordInput.text === userGeneralEditor.confirmPasswordInput.text;

						text: qsTr("Create account");

						onClicked: {
							root.setSuperuserPassword();
						}

						Loading {
							visible: root.isSubmitting;
							anchors.right: parent.right;
							anchors.rightMargin: Style.marginS;
							anchors.verticalCenter: parent.verticalCenter;
							width: Style.iconSizeS;
							height: Style.iconSizeS;
							color: "transparent";
							background.color: "transparent";
							indicatorSize: Style.iconSizeXS;
							z: 20;
						}
					}
				}
			}

			// Bottom breathing room, matching AuthorizationPage's card.
			Item {
				width: 1;
				height: Style.marginM;
			}
		}
	}
}
