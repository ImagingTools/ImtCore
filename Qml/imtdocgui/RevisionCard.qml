import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0

Rectangle {
	id: revisionCard;

	property int revision: 0;
	property string userName: "";
	property string timestamp: "";
	property string description: "";
	property bool isActive: false;
	property bool selectable: false;
	property bool selected: false;

	signal clicked();

	height: revisionContent.height + 2 * Style.marginL;
	radius: Style.radiusM;
	clip: true;
	color: Style.baseColor;

	border.width: revisionCard.selectable && revisionCard.selected ? 2 : Style.buttonBorderWidth;
	border.color: revisionCard.selectable && revisionCard.selected ? Style.imaginToolsAccentColor : (cardMouseArea.containsMouse ? Style.borderColor2 : Style.borderColor);

	Rectangle {
		visible: revisionCard.selectable && revisionCard.selected;
		anchors.fill: parent;
		anchors.margins: 1;
		radius: Style.radiusM;
		color: Style.imaginToolsAccentColor;
		opacity: Style.opacityFaint;
	}

	MouseArea {
		id: cardMouseArea;
		anchors.fill: parent;
		hoverEnabled: revisionCard.selectable;
		cursorShape: revisionCard.selectable ? Qt.PointingHandCursor : Qt.ArrowCursor;
		onClicked: {
			if (revisionCard.selectable){
				revisionCard.clicked();
			}
		}
	}

	Column {
		id: revisionContent;
		anchors.left: parent.left;
		anchors.leftMargin: Style.marginL;
		anchors.right: parent.right;
		anchors.rightMargin: Style.marginL;
		anchors.verticalCenter: parent.verticalCenter;
		spacing: Style.marginM;

		Item {
			width: parent.width;
			height: avatarBubble.height;

			Rectangle {
				id: avatarBubble;
				anchors.left: parent.left;
				anchors.verticalCenter: parent.verticalCenter;
				width: Style.iconSizeL;
				height: width;
				radius: height / 2;
				color: revisionCard.isActive ? Style.imaginToolsAccentColor : Style.borderColor2;

				Text {
					anchors.centerIn: parent;
					text: revisionCard.userName.length > 0 ? revisionCard.userName.charAt(0).toUpperCase() : "?";
					font.family: Style.fontFamilyBold;
					font.pixelSize: Style.fontSizeM;
					color: "white";
				}
			}

			Text {
				id: timestampLabel;
				anchors.right: parent.right;
				anchors.verticalCenter: parent.verticalCenter;
				text: revisionCard.timestamp;
				font.family: Style.fontFamily;
				font.pixelSize: Style.fontSizeS;
				color: Style.inactiveTextColor;
				elide: Text.ElideLeft;
			}

			Column {
				anchors.left: avatarBubble.right;
				anchors.leftMargin: Style.marginM;
				anchors.right: timestampLabel.left;
				anchors.rightMargin: Style.marginM;
				anchors.verticalCenter: parent.verticalCenter;
				spacing: Style.spacingXXS;

				Row {
					spacing: Style.marginM;

					Text {
						id: userLabel;
						text: revisionCard.userName;
						font.family: Style.fontFamilyBold;
						font.pixelSize: Style.fontSizeL;
						color: Style.textColor;
						elide: Text.ElideRight;
					}

					Rectangle {
						visible: revisionCard.isActive;
						anchors.verticalCenter: parent.verticalCenter;
						width: currentLabel.implicitWidth + Style.marginM;
						height: currentLabel.implicitHeight + Style.marginXS;
						radius: height / 2;
						color: Style.imaginToolsAccentColor;

						Text {
							id: currentLabel;
							anchors.centerIn: parent;
							text: qsTr("Current");
							font.family: Style.fontFamilyBold;
							font.pixelSize: Style.fontSizeS;
							color: "white";
						}
					}
				}

				Text {
					text: qsTr("Revision") + " " + revisionCard.revision;
					font.family: Style.fontFamily;
					font.pixelSize: Style.fontSizeS;
					color: Style.subtitleColor;
				}
			}
		}

		Text {
			width: parent.width;
			text: revisionCard.description !== "" ? revisionCard.description : qsTr("No description provided");
			font.family: Style.fontFamily;
			font.pixelSize: Style.fontSizeM;
			font.italic: revisionCard.description === "";
			color: revisionCard.description !== "" ? Style.textColor : Style.inactiveTextColor;
			wrapMode: Text.Wrap;
		}
	}
}
