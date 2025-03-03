import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

DecoratorBase {
	id: tabPanelDecorator;

	width: content.width + 2 * Style.size_mainMargin;
	height: baseElement ? baseElement.height : 50

	property bool textIsCropped: textHelper.text != "" && textHelper.width > text.width;

	Connections {
		target: tabPanelDecorator.baseElement;

		function onStartContentLoading(){
			loading.start();
		}

		function onStopContentLoading(){
			loading.stop();
		}
	}

	Rectangle {
		id: bg;
		anchors.fill: tabPanelDecorator;
		color: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.selected ? Style.alternateBaseColor: "transparent": "transparent";
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

		color: Style.borderColor2;
	}

	Rectangle {
		id: selection;

		anchors.bottom: tabPanelDecorator.bottom;
		anchors.left: tabPanelDecorator.left;
		anchors.right: tabPanelDecorator.right;

		height: 2;

		color: Style.tabSelectedColor;
		visible: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.selected: false;
	}

	Row {
		id: content;
		anchors.centerIn: tabPanelDecorator;
		height: tabPanelDecorator.height;
		spacing: Style.size_mainMargin;
		// visible: !loading.visible;

		Item {
			id: imagetabDelegate;
			anchors.verticalCenter: parent.verticalCenter;
			width: visible ? firsElementImage.width : 0;
			height: tabPanelDecorator.height;
			visible: firsElementImage.status === Image.Ready;

			Image {
				id: firsElementImage;
				anchors.centerIn: imagetabDelegate;
				width: 20;
				height: width;
				source: tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.icon !== "" ? "../../../" + Style.getIconPath(tabPanelDecorator.baseElement.icon, "On", "Normal"): "";
				sourceSize.width: width;
				sourceSize.height: height;
				fillMode: Image.PreserveAspectFit;
			}
		}
		
		Loading {
			id: loading;
			anchors.fill: imagetabDelegate;
			indicatorSize: 20;
			color: tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.selected ? Style.alternateBaseColor: "transparent";
			visible: false;
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
				font.family: tabPanelDecorator.baseElement.index === 0 ? Style.fontFamilyBold : Style.fontFamily;
				font.bold: tabPanelDecorator.baseElement.index === 0;
				font.pixelSize: Style.fontSize_common;
			}

			Text {
				id: text;

				anchors.verticalCenter: parent.verticalCenter;
				anchors.horizontalCenter: tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.index === 0 ||
										  tabPanelDecorator.baseElement && textHelper.width < tabPanelDecorator.baseElement.minWidth ? parent.horizontalCenter : undefined;

				width: tabPanelDecorator.baseElement && tabPanelDecorator.baseElement.index === 0 ? textHelper.width : parent.width;

				color: Style.textColor;
				font.family: tabPanelDecorator.baseElement.index === 0 ? Style.fontFamilyBold : Style.fontFamily;
				font.bold: tabPanelDecorator.baseElement.index === 0;
				font.pixelSize: Style.fontSize_common;
				text: tabPanelDecorator.baseElement ? tabPanelDecorator.baseElement.text : "";

				elide: Text.ElideRight;
			}
		}

		ToolButton {
			id: closeButton;
			anchors.verticalCenter: parent.verticalCenter;
			width: Style.iconSizeSmall;
			height: width;
			visible: !tabPanelDecorator.baseElement.firstElement && tabPanelDecorator.baseElement.isCloseEnable;
			iconSource: "../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);
			decorator: Component {
				ToolButtonDecorator {
					color: "transparent";
					icon.width: 16;
				}
			}

			onClicked: {
				tabPanelDecorator.baseElement.closeSignal();
			}
		}
	}
}


