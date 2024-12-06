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
	property string backgroundColor: "#000000";

	property bool hasTooltip: true;

	property bool hasBottomRounding: false;

	property alias toolTipColor: tooltip.color;
	property alias toolTipFontColor: tooltip.fontColor;
	property alias toolTipFontSize: tooltip.fontPixelSize;
	property alias shownVal: tooltip.shownVal;
	property alias positiveBarHeight: mainRec.height;

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


	Item{
		id: mainRec;

		anchors.bottom:  parent.bottom;
		anchors.horizontalCenter: parent.horizontalCenter;
		width: barChart.barWidth;
		height: (barChart.positiveValue !==0)?
					barChart.maxBarHeight/(barChart.maxValue - barChart.minValue) * barChart.positiveValue : 1;

		clip: true;


		Rectangle{
			id: barDown;//positive

			anchors.horizontalCenter: parent.horizontalCenter;
			anchors.bottom:  parent.bottom;
			anchors.bottomMargin: -vertOffset;

			width: parent.width;
			height: parent.height + vertOffset;
			radius: width;

			color: barChart.isFilled ? barChart.color_positive : "transparent";
			visible: !barChart.visible ? false : (barChart.positiveValue == 0) ? false : barChart.visibleElements;
			border.color: barChart.color_positive;
			border.width: barChart.isFilled ? 0 : barChart.borderWidth;

			property real vertOffset: mainRec.height > width/2 ? width/2 : 0;
		}

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
		property string shownVal:  shownValPos;

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
			tooltip.openTooltip(ma.width/2, mainRec.y);

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

