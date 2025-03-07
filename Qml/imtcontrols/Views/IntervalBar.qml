import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0


Rectangle {
	id: indicator;

	width: Math.max(columnWidth, percentText.width, valueText.width, valueNameText.width);
	height: columnContainer.height + columnContainer.anchors.bottomMargin
			+ percentText.visible * (percentText.height + percentText.anchors.bottomMargin);

	color: "transparent";

	property int maxColumnHeight: 200;//!!!
	property int columnWidth: 60;//!!!

	property int barHeight: 8;
	property int spacing: 8;

	property string positiveColor: Style.positiveAccentColor;
	property string negativeColor: Style.negativeAccentColor;
	property string textColor: Style.textColor;

	property int count: 0;
	property int maxCount: 0;

	property int percent: 0;
	property int percentCritical: 100;
	property int percentMax: 100;

	property real percentCoeff: percentMax/100;

	property real value: 0;
	property string valueName: "";

	property bool hasPercentText: true;
	property bool hasMouseArea: false;

	signal clicked();

	Component.onCompleted: {
		setCount();
	}

	onPercentChanged: {
		setCount();
	}

	function setCount(){
		count = Math.ceil((percent * maxColumnHeight)/ percentMax/(barHeight + spacing))
		maxCount = Math.ceil(maxColumnHeight/(barHeight + spacing))

	}

	Text{
		id: percentText;

		anchors.horizontalCenter: parent.horizontalCenter;
		anchors.bottom: columnContainer.top;

		anchors.bottomMargin: 4;

		font.family: Style.fontFamilyBold;
		font.pixelSize: Style.fontSizeXXXLarge;

		color: indicator.percent >= indicator.percentCritical ? indicator.negativeColor: indicator.textColor;

		text: indicator.percent + " %";
		visible: indicator.hasPercentText && indicator.percent > 0;
	}

	Item{
		id: columnContainer;

		anchors.horizontalCenter: parent.horizontalCenter;
		anchors.bottom: parent.bottom;

		anchors.bottomMargin: valueText.visible ? 2 + valueText.visible * (valueText.height + valueText.anchors.bottomMargin):
												 2 + valueNameText.visible * (valueNameText.height + valueNameText.anchors.bottomMargin);

		width: indicator.columnWidth;
		height: column.height;

		Column{
			id: column;

			anchors.horizontalCenter: parent.horizontalCenter;
			anchors.bottom: parent.bottom;

			width: parent.width;
			rotation: 180;

			Repeater{
				model: indicator.count;
				delegate: Item{
					width: indicator.columnWidth;
					height: indicator.barHeight + indicator.spacing;
					Rectangle{

						width: indicator.columnWidth;
						height: indicator.barHeight;

						color: isCritical ? indicator.negativeColor : indicator.positiveColor;

						property real barPercent: (indicator.percentCoeff/indicator.maxCount *(model.index + 1) *100);
						property bool isCritical: barPercent  > indicator.percentCritical;

					}
				}
			}
		}
	}

	Text{
		id: valueText;

		anchors.horizontalCenter: parent.horizontalCenter;
		anchors.bottom:  parent.bottom;

		anchors.bottomMargin: valueNameText.visible *(valueNameText.height + valueNameText.anchors.bottomMargin);

		font.family: Style.fontFamilyBold;
		font.pixelSize: Style.fontSizeXLarge;

		color:  indicator.textColor;

		text: Functions.formatDigit(indicator.value);
		visible: indicator.value > 0;
	}

	Text{
		id: valueNameText;

		anchors.horizontalCenter: parent.horizontalCenter;
		anchors.bottom:  parent.bottom;

		anchors.bottomMargin: 0;

		font.family: Style.fontFamilyBold;
		font.pixelSize: Style.fontSizeXLarge;

		color:  indicator.textColor;

		text: indicator.valueName;
		visible: text !== "";
	}

	MouseArea{
		id: legendMA;

		anchors.bottom: parent.bottom;
		anchors.top: valueText.top;

		width: parent.width;

		visible: indicator.hasMouseArea;
		hoverEnabled: visible;
		cursorShape: Qt.PointingHandCursor;
		onClicked: {
			indicator.clicked();
		}
	}


	function roundPercent(percent_){
		if(percent_ < 1){
			return 0;
		}
		if(percent_ >= 100){
			return 100;
		}

		let retVal;
		for (let i = 1; i <= indicator.maxCount; i++){
			let val = Math.ceil((100/indicator.maxCount)*i);
			if(percent_ <= val){
				retVal = val;
				break;
			}
		}
		return retVal;
	}


}

