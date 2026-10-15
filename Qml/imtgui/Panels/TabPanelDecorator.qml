import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

DecoratorBase {
	id: tabPanelDecorator;

	width: content.width + 2 * Style.marginS + dividerGap;
	height: baseElement ? baseElement.height : Style.headerHeight

	property bool textIsCropped: textHelper.text != "" && textHelper.width > text.width;

	property int tabCount: tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.listView ? tabPanelDecorator.baseElement.listView.count : 0;
	property bool isSelected: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.selected : false;
	property bool isHovered: tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.mouseArea
			? tabPanelDecorator.baseElement.mouseArea.containsMouse : false;
	property bool isPinned: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.pinned : false;
	property bool isLastTab: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.index >= tabPanelDecorator.tabCount - 1 : true;
	property bool canClose: tabPanelDecorator.baseElement
			&& !tabPanelDecorator.isPinned
			&& tabPanelDecorator.baseElement.isCloseEnable;
	property bool showClose: tabPanelDecorator.canClose && (tabPanelDecorator.isSelected || tabPanelDecorator.isHovered);
	property int closeSlotSize: Style.iconSizeS;
	// Room for the divider to the next tab.
	property int dividerGap: tabPanelDecorator.isLastTab ? 0 : Style.spacingXS + 1;

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

	Rectangle {
		id: background;

		anchors.left: tabPanelDecorator.left;
		anchors.top: tabPanelDecorator.top;
		anchors.bottom: tabPanelDecorator.bottom;
		anchors.topMargin: Style.spacingXS;
		anchors.bottomMargin: Style.spacingXS;
		width: tabPanelDecorator.width - tabPanelDecorator.dividerGap;
		radius: Style.radiusM;
		color: tabPanelDecorator.isSelected ? Style.neutralSubtleColor : (tabPanelDecorator.isHovered ? Style.buttonHoverColor : "transparent");
	}

	// Same gap on both sides: the list spacing follows it.
	Rectangle {
		anchors.left: background.right;
		anchors.leftMargin: Style.spacingXS;
		anchors.verticalCenter: tabPanelDecorator.verticalCenter;
		width: 1;
		height: tabPanelDecorator.height / 2;
		color: Style.borderColor;
		visible: !tabPanelDecorator.isLastTab;
	}

	Row {
		id: content;
		anchors.horizontalCenter: background.horizontalCenter;
		height: tabPanelDecorator.height;
		spacing: Style.spacingS;

		Image {
			id: pinImage;
			anchors.verticalCenter: parent.verticalCenter;
			width: Style.iconSizeXS;
			height: width;
			visible: tabPanelDecorator.isPinned;
			source: tabPanelDecorator.isPinned ? "../../../" + Style.getIconPath("Icons/Pin", "On", tabPanelDecorator.isSelected ? "Active" : "Normal") : "";
			sourceSize.width: width;
			sourceSize.height: height;
			fillMode: Image.PreserveAspectFit;
		}

		Item {
			id: imagetabDelegate;
			anchors.verticalCenter: parent.verticalCenter;
			width: visible ? firsElementImage.width : 0;
			height: tabPanelDecorator.height;
			visible: firsElementImage.status === Image.Ready;

			Image {
				id: firsElementImage;
				anchors.centerIn: imagetabDelegate;
				width: Style.iconSizeS;
				height: width;
				source: tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.icon !== "" ? "../../../" + Style.getIconPath(tabPanelDecorator.baseElement.icon, "On", tabPanelDecorator.isSelected || tabPanelDecorator.isHovered ? "Active" : "Normal"): "";
				sourceSize.width: width;
				sourceSize.height: height;
				fillMode: Image.PreserveAspectFit;
			}
		}

		Item {
			id: texttabDelegate;

			anchors.verticalCenter: parent.verticalCenter;

			width: tabPanelDecorator.baseElement && textHelper.width < tabPanelDecorator.baseElement.minWidth ? tabPanelDecorator.baseElement.minWidth :
																												tabPanelDecorator.baseElement && textHelper.width > tabPanelDecorator.baseElement.maxWidth ? tabPanelDecorator.baseElement.maxWidth : textHelper.width + 10;

			height: tabPanelDecorator.height;

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
				anchors.horizontalCenter: tabPanelDecorator.isPinned ||
										  tabPanelDecorator.baseElement && textHelper.width < tabPanelDecorator.baseElement.minWidth ? parent.horizontalCenter : undefined;

				width: tabPanelDecorator.isPinned ? textHelper.width : parent.width;
				visible: !tabPanelDecorator.isLoading;

				color: tabPanelDecorator.isSelected || tabPanelDecorator.isHovered ? Style.titleColor : Style.subtitleColor;
				font.family: Style.fontFamily;
				font.pixelSize: Style.fontSizeM;
				text: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.text : "";

				elide: Text.ElideRight;
			}

			SkeletonBlock {
				anchors.verticalCenter: parent.verticalCenter;
				anchors.horizontalCenter: parent.horizontalCenter;
				width: parent.width - 10;
				height: Style.iconSizeXXS;
				visible: tabPanelDecorator.isLoading;
				animated: visible;
			}
		}

		Item {
			id: closeSlot;

			anchors.verticalCenter: parent.verticalCenter;
			width: tabPanelDecorator.canClose ? tabPanelDecorator.closeSlotSize : 0;
			height: tabPanelDecorator.closeSlotSize;
			visible: tabPanelDecorator.canClose;

			ToolButton {
				id: closeButton;
				objectName: "CloseButton"
				anchors.centerIn: parent;
				width: tabPanelDecorator.closeSlotSize;
				height: tabPanelDecorator.closeSlotSize;
				opacity: tabPanelDecorator.showClose ? 1 : 0;
				enabled: tabPanelDecorator.showClose;
				iconSource: "qrc:/" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);
				decorator: Component {
					ToolButtonDecorator {
						color: "transparent";
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
