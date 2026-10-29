import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

/**
	Shown while a server the application cannot work without is unreachable.

	The application stays in place under a dimmed backdrop - the open page and documents are kept -
	and a dialog-like card says what is missing, that the connection is being restored and for how long.
*/
Rectangle {
	id: container;

	color: "transparent";

	// Headline of the card (kept as "text" for existing callers).
	property alias text: titleText.text;
	property alias description: descriptionText.text;
	// True while the first connection is still being made; false once a connection was lost.
	property bool loadingVisible: false;
	// Offers a manual retry, for connections the client re-establishes itself.
	property bool retryVisible: false;
	property int elapsedSeconds: 0;

	signal retryClicked();

	onVisibleChanged: {
		container.elapsedSeconds = 0;
	}

	Timer {
		interval: 1000;
		repeat: true;
		running: container.visible;
		onTriggered: {
			container.elapsedSeconds++;
		}
	}

	Rectangle {
		anchors.fill: parent;
		color: Style.overlayBackgroundColor;
		opacity: 0.4;
	}

	MouseArea {
		anchors.fill: parent;
		hoverEnabled: true;
		acceptedButtons: Qt.AllButtons;
		onWheel: {
			wheel.accepted = true;
		}
		onClicked: {}
	}

	Rectangle {
		id: card;
		objectName: "ConnectionLostCard";

		anchors.centerIn: parent;
		width: Math.min(parent.width - 2 * Style.marginXL, Style.sizeHintL);
		height: cardContent.height + 2 * Style.marginXL;

		color: Style.dialogBackgroundColor;
		radius: Style.radiusM;
		border.width: 1;
		border.color: Style.borderColor;

		Column {
			id: cardContent;

			anchors.left: parent.left;
			anchors.right: parent.right;
			anchors.top: parent.top;
			anchors.margins: Style.marginXL;
			spacing: Style.marginM;

			Row {
				width: parent.width;
				spacing: Style.marginM;

				Image {
					id: stateIcon;
					anchors.verticalCenter: parent.verticalCenter;
					width: Style.iconSizeM;
					height: width;
					sourceSize.width: width;
					sourceSize.height: height;
					source: "qrc:/" + Style.getIconPath(container.loadingVisible ? "Icons/Info" : "Icons/Alert", Icon.State.On, Icon.Mode.Normal);
				}

				Text {
					id: titleText;
					anchors.verticalCenter: parent.verticalCenter;
					width: parent.width - stateIcon.width - parent.spacing;
					color: Style.titleColor;
					font.family: Style.fontFamilyBold;
					font.pixelSize: Style.fontSizeL;
					font.bold: true;
					wrapMode: Text.Wrap;
				}
			}

			Text {
				id: descriptionText;
				width: parent.width;
				visible: text !== "";
				color: Style.textColor;
				font.family: Style.fontFamily;
				font.pixelSize: Style.fontSizeM;
				wrapMode: Text.Wrap;
			}

			Row {
				spacing: Style.marginS;

				Loading {
					anchors.verticalCenter: parent.verticalCenter;
					width: Style.iconSizeS;
					height: width;
					indicatorSize: Style.iconSizeS;
					background.color: "transparent";
					Component.onCompleted: {
						start();
					}
				}

				Text {
					anchors.verticalCenter: parent.verticalCenter;
					color: Style.subtitleColor;
					font.family: Style.fontFamily;
					font.pixelSize: Style.fontSizeS;
					text: (container.loadingVisible ? qsTr("Connecting...") : qsTr("Reconnecting..."))
						  + (container.elapsedSeconds >= 3 ? " " + container.elapsedSeconds + " " + qsTr("s") : "");
				}
			}

			Item {
				width: parent.width;
				height: retryButton.visible ? retryButton.height + Style.marginS : 0;

				Button {
					id: retryButton;
					objectName: "RetryConnectionButton";
					anchors.right: parent.right;
					anchors.bottom: parent.bottom;
					visible: container.retryVisible;
					text: qsTr("Retry now");
					variant: "primary";
					widthFromDecorator: true;

					onClicked: {
						container.elapsedSeconds = 0;
						container.retryClicked();
					}
				}
			}
		}
	}
}
