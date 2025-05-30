import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0


Rectangle {
	id: indicator;

	width: row.width;
	height: Style.controlHeightM;

	color: "transparent";

	property int count: 5;
	property int barWidth: continuous ? 8 : 4;
	property int spacing: indicator.continuous ? 0 : Style.spacingXS;
	property int percent: 0;

	property string colorActive: "red";
	property string colorDefault: "lightgray";

	property bool constantHeight: continuous;
	property bool continuous: false;
	property string constantColor: "";

	Row{
		id: row;

		height: parent.height;
		spacing: indicator.spacing;

		Repeater{
			model: indicator.count;
			delegate: Rectangle{
				anchors.bottom: parent.bottom;
				width: indicator.barWidth;
				height: indicator.constantHeight ? indicator.height : (indicator.height/indicator.count) * (model.index + 1);
				color: indicator.constantColor !== "" ? indicator.constantColor : isActive ? indicator.colorActive : indicator.colorDefault;
				property real barPercent: Math.floor(1/indicator.count *(model.index + 1) * 100);
				property bool isActive: indicator.roundPercent(indicator.percent) >= barPercent;
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
			let val = Math.ceil((100/indicator.count)*i) ;
			if(percent_ <= val){
				retVal = val;
				break;
			}
		}
		return retVal;
	}
}

