import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0;
import imtcontrols 1.0

CustomTextField {
	id: tfc;
	objectName: "SearchTextInput"

	textFieldLeftMargin: searchIcon.width + 2 * margin;
	textFieldRightMargin: iconClear.width + 2 * margin;
	placeHolderText: qsTr("Enter text for search");
	width: Style.sizeHintXS;
	height: Style.controlHeightM;
	
	signal searchChanged()

	// A directly-declared onTextChanged here would shadow CustomTextField's own
	// internal onTextChanged (which pushes the text property into the visible
	// TextInput) - Connections attaches independently so both keep working.
	Connections {
		target: tfc
		function onTextChanged(){
			timer.restart();
		}
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
		source: "qrc:/" + Style.getIconPath("Icons/Search", Icon.State.On, Icon.Mode.Normal);
		sourceSize.width: width
		sourceSize.height: height
	}
	
	function clear(){
		iconClear.clicked()
	}
	
	ToolButton {
		id: iconClear;
		objectName: "ClearText"
		
		z: 999;
		
		anchors.verticalCenter: tfc.verticalCenter;
		anchors.right: tfc.right;
		anchors.rightMargin: Style.marginM;
		
		width: Style.buttonWidthXS;
		height: width;
		
		visible: tfc.text != "";
		
		iconSource: "qrc:/" + Style.getIconPath("Icons/Close", Icon.State.On, Icon.Mode.Normal);
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
