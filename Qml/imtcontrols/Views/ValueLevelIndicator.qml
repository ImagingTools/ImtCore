import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0


Rectangle {
	id: indicator;

	width: row.width;
	height: 30;

	color: "transparent";

	property int count: 5;
	property int barWidth: 4;
	property alias spacing: row.spacing;
	property int percent: 0;

	property string colorActive: "red";
	property string colorDefault: "lightgray";

	Row{
		id: row;

		height: parent.height;
		spacing: 4;

		Repeater{
			model: indicator.count;
			delegate: Rectangle{
				anchors.bottom: parent.bottom;
				width: indicator.barWidth;
				height: (indicator.height/indicator.count) * (model.index + 1);
				color: isActive ? indicator.colorActive : indicator.colorDefault;

				property bool isActive: indicator.roundPercent(indicator.percent) >= (1/indicator.count *(model.index + 1) * 100);
			}
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
		for (let i = 1; i <= indicator.count; i++){
			let val = (100/indicator.count)*i ;
			if(percent_ <= val){
				retVal = val;
				break;
			}
		}
		return retVal;
	}
}

