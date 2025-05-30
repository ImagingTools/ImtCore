import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0;
import imtcontrols 1.0

CustomTextField {
	id: tfc;

	textFieldLeftMargin: searchIcon.width + 2 * margin;
	textFieldRightMargin: iconClear.width + 2 * margin;
	placeHolderText: qsTr("Enter text for search");
	width: Style.sizeHintM;
	height: Style.controlHeightM;
	
	signal searchChanged()
	
	onTextChanged: {
		timer.restart();
	}

	Timer {
		id: timer;
		interval: 500;
		onTriggered: {
			tfc.searchChanged()
		}
	}

	Image {
		id: searchIcon
		anchors.left: parent.left
		anchors.leftMargin: Style.marginM
		anchors.verticalCenter: parent.verticalCenter
		width: Style.buttonWidthXS;
		height: width;
		source: "../../../" + Style.getIconPath("Icons/Search", Icon.State.On, Icon.Mode.Normal);
		sourceSize.width: width
		sourceSize.height: height
	}
	
	ToolButton {
		id: iconClear;
		
		z: 999;
		
		anchors.verticalCenter: tfc.verticalCenter;
		anchors.right: tfc.right;
		anchors.rightMargin: Style.marginM;
		
		width: Style.buttonWidthXS;
		height: width;
		
		visible: tfc.text != "";
		
		iconSource: "../../../" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);
		decorator: Component {
			ToolButtonDecorator {
				color: "transparent";
				icon.width: Style.iconSizeS;
			}
		}
		
		onClicked: {
			tfc.text = "";
		}
	}
}
