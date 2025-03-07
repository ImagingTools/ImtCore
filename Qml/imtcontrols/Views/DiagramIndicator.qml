import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0


Rectangle{
	id: bigIndicatorContainer;

	width: 0;
	height: width * coeff;
	color: "transparent";

	property Item root: null;
	property Item rootItem: null;
	property Item diagramItem: null;
	property bool hiddenBackground: true;
	property bool noMouseArea: true;
	property real barX: 0;
	property real barY: 0;
	property string text: "";

	property real coeff: 1;
	property real textVerticalOffset: 0;
	property int mainMarginVertical: 40;
	property int textMargin: 3*Style.sizeMainMargin;

	property alias textObj: bigIndicatorText;

	Component.onCompleted: {
		Events.subscribeEvent("AppSizeChanged", onAppSizeChanged);
		if(rootItem){
			x = barX - width/2;
			y = barY - height - mainMarginVertical;
		}
	}

	Component.onDestruction: {
		Events.unSubscribeEvent("AppSizeChanged", onAppSizeChanged);
		if(diagramItem){
			diagramItem.bigIndicatorItem = null;
			diagramItem.selectedIndex = -1;
		}
	}

	onDiagramItemChanged: {
		if(diagramItem){
			diagramItem.bigIndicatorItem = bigIndicatorContainer;
		}
	}

	function onAppSizeChanged(parameters){
		if(diagramItem){
			ModalDialogManager.closeByComp(diagramItem.bigIndicatorComp)
			diagramItem.bigIndicatorItem = null;
			diagramItem.selectedIndex = -1;
		}
	}

	onRootItemChanged: {
		if(rootItem){
			//console.log("ROOT_ITEM_CHANGED", barX, width)
			x = barX - width/2;
			y = barY - height - mainMarginVertical;
		}
	}
	onBarXChanged: {
		x = barX - width/2;
	}


	onWidthChanged: {
		x = barX - width/2;
	}

	onBarYChanged: {
		y = barY - height - mainMarginVertical;
	}

	onHeightChanged: {
		y = barY - height - mainMarginVertical;
	}

	Text{
		id: bigIndicatorText;

		anchors.horizontalCenter: parent.horizontalCenter;
		anchors.verticalCenter:  parent.verticalCenter;
		anchors.verticalCenterOffset: bigIndicatorContainer.textVerticalOffset;

		font.family: Style.fontFamily;
		font.pixelSize: Style.fontSizeNormal;
		color: Style.color_first;
		text: bigIndicatorContainer.text;
		z: 10;
		onWidthChanged: {
			if(width + bigIndicatorContainer.textMargin > bigIndicatorContainer.width){
				bigIndicatorContainer.width = width + bigIndicatorContainer.textMargin;
			}
		}
	}

}

