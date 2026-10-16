import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

DecoratorBase {
	id: tabPanelDecorator;

	width: content.width + 2 * Style.marginS;
	height: baseElement ? baseElement.height : Style.headerHeight

	property bool textIsCropped: textHelper.text != "" && textHelper.width > text.width;

	property bool isSelected: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.selected : false;
	property bool isHovered: tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.mouseArea
			? tabPanelDecorator.baseElement.mouseArea.containsMouse : false;
	property bool isPinned: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.pinned : false;
	property bool canClose: tabPanelDecorator.baseElement
			&& !tabPanelDecorator.isPinned
			&& tabPanelDecorator.baseElement.isCloseEnable;
	property bool showClose: tabPanelDecorator.canClose && (tabPanelDecorator.isSelected || tabPanelDecorator.isHovered);
	// VS Code: a modified tab always shows a dot where the close button sits; pointing at the dot turns it into the close button.
	property bool isDirty: tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.dirty !== undefined ? tabPanelDecorator.baseElement.dirty : false;
	property bool showDirtyDot: tabPanelDecorator.canClose && tabPanelDecorator.isDirty && !closeButton.hovered;
	property int closeSlotSize: Style.iconSizeM;

	property bool showBackground: tabPanelDecorator.isSelected || tabPanelDecorator.isHovered;
	property string backgroundColor: tabPanelDecorator.isSelected ? Style.baseColor : Style.toolButtonHoverColor;
	property string stripColor: tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.tabPanel
			? tabPanelDecorator.baseElement.tabPanel.color : Style.tabPanelBackgroundColor;
	property int cornerRadius: Style.radiusM;
	property int flareRadius: Style.radiusS;

	property bool signalLoading: false;
	property bool isLoading: tabPanelDecorator.signalLoading || (tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.contentLoading : false);

	Connections {
		target: tabPanelDecorator.baseElement;

		function onStartContentLoading(){
			tabPanelDecorator.signalLoading = true;
		}

		function onStopContentLoading(){
			tabPanelDecorator.signalLoading = false;
		}
	}

	// Rounded top corners only: the second rectangle squares off the bottom ones.
	Rectangle {
		id: background;

		anchors.fill: tabPanelDecorator;
		anchors.topMargin: 1;
		radius: tabPanelDecorator.cornerRadius;
		color: tabPanelDecorator.backgroundColor;
		visible: tabPanelDecorator.showBackground;
	}

	Rectangle {
		anchors.left: tabPanelDecorator.left;
		anchors.right: tabPanelDecorator.right;
		anchors.bottom: tabPanelDecorator.bottom;
		height: tabPanelDecorator.cornerRadius;
		color: tabPanelDecorator.backgroundColor;
		visible: tabPanelDecorator.showBackground;
	}

	// Concave corners that flow the selected tab into the page below.
	Item {
		id: leftFlare;

		anchors.right: tabPanelDecorator.left;
		anchors.bottom: tabPanelDecorator.bottom;
		width: tabPanelDecorator.flareRadius;
		height: tabPanelDecorator.flareRadius;
		clip: true;
		visible: tabPanelDecorator.isSelected;

		Rectangle {
			anchors.fill: parent;
			color: tabPanelDecorator.backgroundColor;
		}

		Rectangle {
			x: -tabPanelDecorator.flareRadius;
			y: -tabPanelDecorator.flareRadius;
			width: 2 * tabPanelDecorator.flareRadius;
			height: width;
			radius: tabPanelDecorator.flareRadius;
			color: tabPanelDecorator.stripColor;
		}
	}

	Item {
		id: rightFlare;

		anchors.left: tabPanelDecorator.right;
		anchors.bottom: tabPanelDecorator.bottom;
		width: tabPanelDecorator.flareRadius;
		height: tabPanelDecorator.flareRadius;
		clip: true;
		visible: tabPanelDecorator.isSelected;

		Rectangle {
			anchors.fill: parent;
			color: tabPanelDecorator.backgroundColor;
		}

		Rectangle {
			x: 0;
			y: -tabPanelDecorator.flareRadius;
			width: 2 * tabPanelDecorator.flareRadius;
			height: width;
			radius: tabPanelDecorator.flareRadius;
			color: tabPanelDecorator.stripColor;
		}
	}

	Row {
		id: content;

		anchors.left: tabPanelDecorator.left;
		anchors.leftMargin: Style.marginS;
		anchors.top: background.top;
		anchors.bottom: tabPanelDecorator.bottom;
		spacing: Style.spacingS;

		Item {
			id: imagetabDelegate;
			anchors.verticalCenter: parent.verticalCenter;
			width: visible ? firsElementImage.width : 0;
			height: parent.height;
			visible: firsElementImage.status === Image.Ready;

			Image {
				id: firsElementImage;
				anchors.centerIn: imagetabDelegate;
				width: Style.iconSizeS;
				height: width;
				source: tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.icon !== "" ? "../../../" + Style.getIconPath(tabPanelDecorator.baseElement.icon, "On", tabPanelDecorator.isSelected ? "Active" : "Normal"): "";
				sourceSize.width: width;
				sourceSize.height: height;
				fillMode: Image.PreserveAspectFit;
			}
		}

		Item {
			id: texttabDelegate;

			anchors.verticalCenter: parent.verticalCenter;

			width: tabPanelDecorator.baseElement && textHelper.width < tabPanelDecorator.baseElement.minWidth ? tabPanelDecorator.baseElement.minWidth :
																												tabPanelDecorator.baseElement && textHelper.width > tabPanelDecorator.baseElement.maxWidth ? tabPanelDecorator.baseElement.maxWidth : textHelper.width + 2;

			height: parent.height;

			clip: true;

			Text {
				id: textHelper;

				visible: false;

				text: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.text : "";
				font.family: Style.fontFamily;
				font.pixelSize: Style.fontSizeM;
			}

			Text {
				id: text;

				anchors.verticalCenter: parent.verticalCenter;
				anchors.left: parent.left;

				width: parent.width;
				horizontalAlignment: tabPanelDecorator.isPinned ? Text.AlignHCenter : Text.AlignLeft;
				visible: !tabPanelDecorator.isLoading;

				color: tabPanelDecorator.isSelected ? Style.textColor : Style.subtitleColor;
				font.family: Style.fontFamily;
				font.pixelSize: Style.fontSizeM;
				text: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.text : "";

				elide: Text.ElideRight;
			}

			SkeletonBlock {
				anchors.verticalCenter: parent.verticalCenter;
				anchors.left: parent.left;
				width: parent.width;
				height: Style.iconSizeXXS;
				visible: tabPanelDecorator.isLoading;
				animated: visible;
			}
		}

		// The pin of a pinned tab takes the place of its close button.
		Item {
			id: closeSlot;

			anchors.verticalCenter: parent.verticalCenter;
			width: tabPanelDecorator.canClose || tabPanelDecorator.isPinned ? tabPanelDecorator.closeSlotSize : 0;
			height: tabPanelDecorator.closeSlotSize;
			visible: tabPanelDecorator.canClose || tabPanelDecorator.isPinned;

			Image {
				id: pinImage;
				anchors.centerIn: parent;
				width: Style.iconSizeXS;
				height: width;
				visible: tabPanelDecorator.isPinned;
				source: tabPanelDecorator.isPinned ? "../../../" + Style.getIconPath("Icons/Pin", "On", tabPanelDecorator.isSelected ? "Active" : "Normal") : "";
				sourceSize.width: width;
				sourceSize.height: height;
				fillMode: Image.PreserveAspectFit;
			}

			Rectangle {
				id: dirtyDot;
				objectName: "DirtyIndicator"
				anchors.centerIn: parent;
				width: Style.spacingS;
				height: width;
				radius: width / 2;
				color: tabPanelDecorator.isSelected ? Style.textColor : Style.subtitleColor;
				visible: tabPanelDecorator.showDirtyDot;
			}

			ToolButton {
				id: closeButton;
				objectName: "CloseButton"
				anchors.centerIn: parent;
				width: tabPanelDecorator.closeSlotSize;
				height: tabPanelDecorator.closeSlotSize;
				visible: tabPanelDecorator.canClose;
				opacity: tabPanelDecorator.showDirtyDot ? 0 : tabPanelDecorator.showClose || closeButton.hovered ? 1 : 0;
				enabled: tabPanelDecorator.showClose || tabPanelDecorator.isDirty;
				iconSource: "qrc:/" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);
				decorator: Component {
					ToolButtonDecorator {
						radius: Style.radiusS;
						icon.width: 10;
					}
				}

				onClicked: {
					tabPanelDecorator.baseElement.closeSignal();
				}
			}
		}
	}
}
