import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

Rectangle {
	id: root;
	
	height: titleText_.height + topPadding + bottomPadding;
	
	color: "transparent";
	
	property bool opened: true;
	
	property GroupElementView groupView: null;
	property alias title: titleText_.text;
	property alias titleText: titleText_;
	property int topPadding: 0
	property int leftPadding: 0
	property int rightPadding: Style.marginM
	property int bottomPadding: 0
	property Item controlItem: controlLoader.item
	property Component controlComp: baseControlComp
	
	onOpenedChanged: {
		if (!groupView){
			console.error("property 'groupView' is invalid");
			return;
		}

		// The group owns the collapse/expand animation via its 'opened' state and a
		// Behavior on height. Driving it here keeps the group's height binding intact
		// (reactive to content changes), instead of overwriting height imperatively.
		groupView.opened = opened;
	}

	onGroupViewChanged: {
		if (groupView){
			groupView.opened = root.opened;
		}
	}

	Loader {
		id: controlLoader;
		
		anchors.verticalCenter: parent.verticalCenter;
		anchors.right: parent.right;
		
		sourceComponent: root.controlComp
	}
	
	Component {
		id: baseControlComp;
		
		ToolButton {
			id: openButton;
			
			anchors.verticalCenter: parent.verticalCenter;
			anchors.right: parent.right;
			anchors.rightMargin: root.rightPadding;
			
			height: Style.buttonHeightS;
			width: height;
			visible: root.groupView != null
			
			iconSource: root.opened
						? "../../../" + Style.getIconPath("Icons/Up", Icon.State.On, Icon.Mode.Normal)
						: "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);
			
			onClicked: {
				root.opened = !root.opened;
			}
		}
	}
	
	Text {
		id: titleText_;
		
		anchors.top: parent.top
		anchors.topMargin: root.topPadding
		anchors.left: parent.left;
		anchors.leftMargin: root.leftPadding
		
		color: Style.textColor;
		font.family: Style.fontFamilyBold;
		font.pixelSize: Style.fontSizeXXL;
	}
}


