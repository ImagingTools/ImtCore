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

	property string positiveColor: "lightgreen";
	property string negativeColor: "salmon";
	property string textColor: Style.textColor;

	property int count: 0;
	property int maxCount: 0;

	property int percent: 0;
	property int percentCritical: 100;
	property real value: 0;
	property string valueName: "";

	property bool hasPercentText: true;

	Component.onCompleted: {
		setCount();
	}

	onPercentChanged: {
		setCount();
	}

	function setCount(){
		count = Math.ceil((percent * maxColumnHeight)/ 100/(barHeight + spacing))
		maxCount = Math.ceil((100 * maxColumnHeight)/ 100/(barHeight + spacing))
	}

	Text{
		id: percentText;

		anchors.horizontalCenter: parent.horizontalCenter;
		anchors.bottom: columnContainer.top;

		anchors.bottomMargin: 4;

		font.family: Style.fontFamilyBold;
		font.pixelSize: Style.fontSizeLarge;

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
						property real barPercent: (1/indicator.maxCount *(model.index + 1) * 100);
						property bool isCritical: barPercent > indicator.roundPercent(indicator.percentCritical) ;

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
		font.pixelSize: Style.fontSizeNormal;

		color:  indicator.textColor;

		text: indicator.formatDigit(indicator.value);
		visible: indicator.value > 0;
	}

	Text{
		id: valueNameText;

		anchors.horizontalCenter: parent.horizontalCenter;
		anchors.bottom:  parent.bottom;

		anchors.bottomMargin: 0;

		font.family: Style.fontFamilyBold;
		font.pixelSize: Style.fontSizeNormal;

		color:  indicator.textColor;

		text: indicator.valueName;
		visible: text !== "";
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
			let val = (100/indicator.maxCount)*i ;
			if(percent_ <= val){
				retVal = val;
				break;
			}
		}
		return retVal;
	}

	function formatDigit(digit){

		var digitString = String(digit).replace(",",".")
		var digit_ = Number(digitString)

		var beforeDot = Math.floor(digit_)
		var afterDot = digitString.match(/\.\d{0,}/) === null ? "" : digitString.match(/\.\d{0,}/)[0]
		if(afterDot === ".")
			afterDot = ""
		//console.log(afterDot)
		var currVal = beforeDot
		var arr = []
		var count = 0

		var retVal = ""

		var str

		while(currVal >= 1000){
			if(currVal / 1000 >= 1){
				arr[count] = String(currVal).substring(String(currVal).length - 3)
				currVal = Math.floor(currVal / 1000)
				count++
			}

		}

		retVal = currVal

		for(var i = arr.length - 1; i >= 0; i--){
			//console.log(arr[i])
			retVal += " "
			retVal += String(arr[i])

		}

		//округление до сотых
//        if(afterDot !== "")
//            afterDot = String(Math.round(parseFloat(afterDot) * 100) / 100).match(/\.\d{0,}/)[0]


		retVal += afterDot

		retVal = retVal.replace(".", ",")

		//console.log("FORMAT_DIGIT: " + retVal)
		return retVal
	}

	function formatDigitRev(digit){
		return digit.replace(/\s/g,'')
	}

	function roundTo(digit, count){
		return Math.round(parseFloat(digit) * Math.pow(10,count)) / Math.pow(10,count)
	}
}

