import QtQuick 2.12
import Acf 1.0;
import imtcontrols 1.0

ControlBase {
	id: checkBox;

	decorator: Style.checkBoxDecorator

	property string text: "";

	property alias tooltipText: tooltip.text;
	property alias tooltipItem: tooltip;

	property string borderColor: "";
	property int radius: 0;
	property bool isActive: true;
	property bool tristate: false;

	property int checkState: Qt.Unchecked;

	property string checkIndicator: "image";

	property string imageSourceActive: "../../../" + Style.getIconPath("Icons/Ok", Icon.State.On, Icon.Mode.Normal);
	property string imageSourceNotActive: "../../../" + Style.getIconPath("Icons/Ok", Icon.State.Off, Icon.Mode.Disabled);

	property int fontPixelSize: Style.fontSizeXSmall;
	property bool fontBold: false;
	property bool isLeftText: false;

	property int mainMargin: 8;

	function nextCheckState(){
		if(!tristate){
			if(checkState == Qt.Unchecked){
				checkState = Qt.Checked;
			}
			else if(checkState == Qt.Checked){
				checkState = Qt.Unchecked;
			}
			else {
				checkState = Qt.Checked;
			}
		}
		else {
			if(checkState == Qt.Unchecked){
				checkState = Qt.PartiallyChecked;
			}
			else if(checkState == Qt.PartiallyChecked){
				checkState = Qt.Checked;
			}
			else if(checkState == Qt.Checked){
				checkState = Qt.Unchecked;
			}
		}
	}

	MouseArea {
		id: ma;

		anchors.fill: parent;
		hoverEnabled: true;

		cursorShape: Qt.PointingHandCursor;

		visible: checkBox.isActive;

		onClicked: {
			checkBox.nextCheckState();
		}

		onPressed: {
			if(tooltip.text !== ""){
				tooltip.closeTooltip();
			}
		}

		onPositionChanged: {
			if(tooltip.text !== ""){
				tooltip.show(mouseX, mouseY);
			}
		}

		onExited: {
			if(tooltip.text !== ""){
				tooltip.hide();
			}
		}
	}

	CustomTooltip{
		id: tooltip;

		fitToTextWidth: true;
	}


}
