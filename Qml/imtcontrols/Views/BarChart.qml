import QtQuick 2.12
import Acf 1.0

Item {

	id: barChart;

	height: maxBarHeight;
	width: visibleElements ? elementWidth : 0;

	property bool visibleElements: true;
	property real maxBarHeight: 90;
	property real maxValue: 100;
	property real negativeValue;
	property real positiveValue;
	property real elementWidth: barWidth;
	property real barWidth: 30;
	property real borderWidth: 2;

	property real addToValue: 30;
	property real minValue: 0;


	property string color_positive: "#ff8a3d";
	property string color_negative: "#000000";
	property string backgroundColor: "#000000";

	property bool hasTooltip: true;
	property bool isPositiveTooltip: true;
	property bool isNegativeTooltip: false;
	property bool hasBottomRounding: false;

	property alias toolTipColor: tooltip.color;
	property alias toolTipFontColor: tooltip.fontColor;
	property alias toolTipFontSize: tooltip.fontPixelSize;
	property alias shownVal: tooltip.shownVal;
	property alias positiveBarHeight: barDown.height;
	property alias negativeBarHeight: barUp.height;


	property real parentY: 0;

	property bool canChangeFilling: false;
	property bool isFilled: true;
	property bool isSelected: false;

	signal clicked();

	onParentYChanged: {
		if(tooltip.openST){
			tooltip.closeTooltip();
		}
	}


	Rectangle{
		id: barDown;//positive

		anchors.bottom:  parent.bottom;
		anchors.horizontalCenter: parent.horizontalCenter;
		width: barChart.barWidth;
		height: (barChart.positiveValue !==0)?
					barChart.maxBarHeight/(barChart.maxValue - barChart.minValue) * barChart.positiveValue : 1;
		radius: width;//Math.min(width, height)
		color: barChart.isFilled ? barChart.color_positive : "transparent";
		visible: !barChart.visible ? false : (barChart.positiveValue == 0)? false : barChart.visibleElements;
		border.color: barChart.color_positive;
		border.width: barChart.isFilled ? 0 : barChart.borderWidth;
	}

	Rectangle{
		id: barDownPatch;//positive

		anchors.bottom:  parent.bottom;
		anchors.horizontalCenter: parent.horizontalCenter;
		width: barChart.barWidth;
		height: Math.min(width, barDown.height);
		color: barChart.isFilled ? barChart.color_positive : "transparent";
		visible: !barChart.hasBottomRounding;
		border.color: barChart.color_positive;
		border.width: barChart.isFilled ? 0 : barChart.borderWidth;

	}

	Rectangle{
		id: barDownBorderPatch;//positive

		anchors.bottom:  parent.bottom;
		anchors.horizontalCenter: parent.horizontalCenter;
		anchors.top: barDown_topRec.bottom;
		width: barChart.barWidth - 2*barDown.border.width;
		height: 2*width;
		color: barChart.backgroundColor;
		visible: !barChart.isFilled;

	}


	Rectangle{
		id: barDown_topRec;//positive

		anchors.top:  barDown.top;
		anchors.horizontalCenter: parent.horizontalCenter;
		width: barChart.barWidth;
		height: (barChart.positiveValue !==0)? barDown.height/2 : 1;
		//Math.min(barChart.maxBarHeight/barChart.maxValue*barChart.positiveValue, width/2) : 1
		color: barChart.isFilled ? barChart.color_positive : "transparent";
		visible: !barChart.visible ? false : (barChart.positiveValue == 0)? false : (barChart.negativeValue == 0) ? false : barChart.visibleElements;

	}

	Rectangle{
		id: barUp;//negative

		anchors.bottom: barDown.top;
		anchors.horizontalCenter: parent.horizontalCenter;
		width: barChart.barWidth;
		height: (barChart.negativeValue !==0)?
					barChart.maxBarHeight/(barChart.maxValue - barChart.minValue) * barChart.negativeValue : 1;
		radius: width//Math.min(width, height)
		color: barChart.isFilled ? barChart.color_negative : "transparent";
		visible: !barChart.visible ? false : (barChart.negativeValue == 0)? false : barChart.visibleElements;
		border.color: barChart.color_negative;
		border.width: barChart.isFilled ? 0 : barChart.borderWidth;

	}

	Rectangle{
		id: barUp_downRec;//negative

		anchors.bottom: barUp.bottom;
		anchors.horizontalCenter: parent.horizontalCenter;
		width: barChart.barWidth;
		height: (barChart.negativeValue !==0)? barUp.height/2 : 1;
		//Math.min(barChart.maxBarHeight/barChart.maxValue*barChart.negativeValue, width/2) : 1
		color: barChart.isFilled ? barChart.color_negative : "transparent";
		visible: !barChart.visible ? false : (barChart.negativeValue == 0) ? false : (barChart.positiveValue == 0) ? false : barChart.visibleElements;
		border.color: barChart.color_negative;
		border.width: barChart.isFilled ? 0 : barChart.borderWidth;

	}

	Rectangle{
		id: barUpBorderPatch;//positive

		anchors.bottom:  barUp.bottom;
		anchors.horizontalCenter: parent.horizontalCenter;
		anchors.top: barUp.top;
		anchors.topMargin: barUp.width;
		width: barChart.barWidth - 2*barDown.border.width;
		height: 2*width;
		color: barChart.backgroundColor;
		visible: !barChart.isFilled;

	}

	CustomTooltip{
		id: tooltip;

		fitToTextWidth: true;

		componentHeight: 30;
		fontPixelSize:  Style.fontSize_common;

		color: Style.color_buttonText;
		fontColor: "#ffffff";

		borderColor: Style.color_elementBorder;
		property string shownValPos: Math.trunc((barChart.positiveValue + barChart.addToValue)*1000)/1000;
		property string shownValNeg: Math.trunc((barChart.negativeValue + barChart.addToValue)*1000)/1000;
		property string shownVal: barChart.isPositiveTooltip ? shownValPos : barChart.isNegativeTooltip ? shownValNeg : "";

		text: shownVal;

		function openTooltip(xX, yY){
			if(barChart.isSelected && barChart.canChangeFilling){
				return;
			}
			var point = mapToItem(null, xX, yY);

			open(point.x - tooltipWidth/2, point.y - componentHeight - 4)

		}
	}

	PauseAnimation {
		id: pauseTooltip;

		duration: tooltip.waitingDuration;
		onFinished: {
			//tooltip.openTooltip(ma.width/2, ma.mouseY);
			tooltip.openTooltip(ma.width/2, barUp_downRec.y);
			// tooltip.openTooltipWithCoord(barChart.barWidth/2 - tooltip.tooltipWidth/2,  barDown.y - tooltip.componentHeight - 5);

		}
	}

	MouseArea{
		id: ma;

		anchors.fill: parent;

		visible: true;
		hoverEnabled: visible;
		cursorShape: Qt.PointingHandCursor;

		onClicked: {
			barChart.clicked();
		}

		onEntered: {
			if(barChart.hasTooltip){
				if(tooltip.text !== ""){
					pauseTooltip.restart();

				}
			}
		}

		onExited: {
			if(barChart.hasTooltip){
				if(tooltip.text !== ""){
					pauseTooltip.stop();
					tooltip.closeTooltip();
				}
			}
		}

	}

	function closeTooltip(){
		if(barChart.hasTooltip){
			if(tooltip.text !== ""){
				pauseTooltip.stop();
				tooltip.closeTooltip();
			}
		}
	}

}

