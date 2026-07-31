import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

DecoratorBase {
	id: tabPanelDecorator;

	width: content.width + 2 * Style.marginM;
	height: baseElement ? baseElement.height : Style.headerHeight

	property bool textIsCropped: textHelper.text != "" && textHelper.width > text.width;

	property bool isSelected: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.selected : false;
	property bool isHovered: tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.mouseArea
			? tabPanelDecorator.baseElement.mouseArea.containsMouse : false;
	// Slot is always reserved for closable tabs so hover does not resize the tab.
	property bool canClose: tabPanelDecorator.baseElement
			&& !tabPanelDecorator.baseElement.pinned
			&& tabPanelDecorator.baseElement.isCloseEnable;
	property bool showClose: tabPanelDecorator.canClose && (tabPanelDecorator.isSelected || tabPanelDecorator.isHovered);
	property int closeSlotSize: Style.iconSizeS;

	Connections {
		target: tabPanelDecorator.baseElement;

		function onStartContentLoading(){
			loading.start();
		}

		function onStopContentLoading(){
			loading.stop();
		}
	}

	// Light gray hover wash, inset from the tab edges - not on the selected tab,
	// whose own bottom accent bar already marks it.
	Rectangle {
		anchors.fill: tabPanelDecorator;
		anchors.topMargin: Style.marginXS;
		anchors.bottomMargin: Style.marginXS;
		anchors.leftMargin: 2;
		anchors.rightMargin: 2;
		radius: Style.marginS;
		color: Style.alternateBaseColor;
		visible: tabPanelDecorator.isHovered && !tabPanelDecorator.isSelected;
	}

	Rectangle {
		anchors.right: tabPanelDecorator.right;
		anchors.verticalCenter: tabPanelDecorator.verticalCenter;

		width: 1;
		height: tabPanelDecorator.height / 2;

		visible: tabPanelDecorator.baseElement
				 ? tabPanelDecorator.baseElement.index < tabPanelDecorator.baseElement.listView.count - 1 &&
				   !tabPanelDecorator.baseElement.selected &&
				   (tabPanelDecorator.baseElement.index + 1) !== tabPanelDecorator.baseElement.selectedIndex
				 : false;

		color: Style.borderColor;
		opacity: 0.55;
	}

	Rectangle {
		id: selection;

		anchors.bottom: tabPanelDecorator.bottom;
		anchors.left: tabPanelDecorator.left;
		anchors.right: tabPanelDecorator.right;

		height: tabPanelDecorator.isSelected ? 2 : 0;

		color: Style.textSelectedColor;
	}

	Row {
		id: content;
		anchors.centerIn: tabPanelDecorator;
		height: tabPanelDecorator.height;
		spacing: Style.marginM;
		visible: !loading.visible;

		Item {
			id: imagetabDelegate;
			anchors.verticalCenter: parent.verticalCenter;
			width: visible ? firsElementImage.width : 0;
			height: tabPanelDecorator.height;
			visible: firsElementImage.status === Image.Ready;

			Image {
				id: firsElementImage;
				anchors.centerIn: imagetabDelegate;
				width: Style.iconSizeM;
				height: width;
				source: tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.icon !== "" ? "../../../" + Style.getIconPath(tabPanelDecorator.baseElement.icon, "On", "Normal"): "";
				sourceSize.width: width;
				sourceSize.height: height;
				fillMode: Image.PreserveAspectFit;
				opacity: tabPanelDecorator.isSelected || tabPanelDecorator.isHovered ? 1.0 : Style.opacityHigh;
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
				font.family: tabPanelDecorator.isSelected || (tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.pinned) ? Style.fontFamilyBold : Style.fontFamily;
				font.bold: tabPanelDecorator.isSelected || (tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.pinned);
				font.pixelSize: Style.fontSizeM;
			}

			Text {
				id: text;

				anchors.verticalCenter: parent.verticalCenter;
				anchors.horizontalCenter: tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.pinned ||
										  tabPanelDecorator.baseElement && textHelper.width < tabPanelDecorator.baseElement.minWidth ? parent.horizontalCenter : undefined;

				width: tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.pinned ? textHelper.width : parent.width;

				color: tabPanelDecorator.isSelected ? Style.titleColor : (tabPanelDecorator.isHovered ? Style.textColor : Style.subtitleColor);
				font.family: tabPanelDecorator.isSelected || (tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.pinned) ? Style.fontFamilyBold : Style.fontFamily;
				font.bold: tabPanelDecorator.isSelected || (tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.pinned);
				font.pixelSize: Style.fontSizeM;
				text: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.text : "";

				elide: Text.ElideRight;
			}
		}

		// Fixed-width close slot: width never collapses, only the glyph fades in/out,
		// so hovering a tab does not resize it.
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

	// Spinner only over the label (not the whole tab / close slot), so a stuck
	// waitName load cannot block closing the tab.
	Loading {
		id: loading;
		// texttabDelegate is a grandchild (inside the `content` Row), not a
		// parent or sibling of this item, so it cannot be an anchor target.
		// Center over it by reading its geometry into a plain binding instead.
		anchors.verticalCenter: tabPanelDecorator.verticalCenter;
		x: content.x + texttabDelegate.x + (texttabDelegate.width - width) / 2;
		width: Style.controlHeightS;
		height: Style.controlHeightS;
		indicatorSize: Style.iconSizeS;
		background.color: "transparent";
		visible: false;
		z: 5;
	}
}


