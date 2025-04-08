import QtQuick 2.12
import Acf 1.0;
import imtcontrols 1.0

ControlBase {
	id: slider;

	rotation: orientation == Qt.Vertical ? -90 : 0;

	decorator: Style.sliderDecorator


	property real _backgroundWidth: width - controlWidth;
	property int backgroundHeight: 6;
	property string backgroundColor: "#ffc0cb";
	property string controlColor:  "#CB4154";

	property real backgroundOpacity: 1.;

	property real controlRecX: 0;
	property real controlWidth: 20;
	property real controlHeight: controlWidth;

	property int orientation: Qt.Horizontal;
	property real position: controlRecX/(width - controlWidth);

	property real from: 0.0;
	property real to: 1.0;
	property real value: position * (to - from) + from;

	property int majorTickInterval: 0;
	property int minorTickInterval: 0;

	property bool hasTicks: majorTickInterval > 0;
	property bool hasIndicator: false;
	property int majorTickHeight: 30;
	property int minorTickHeight: majorTickHeight/2;
	property int indicatorHeight: 60;

	property int ticksPosition: Enums.ticksBothSides
	property int indicatorPosition: Enums.sliderHintAbove
	property real controlCenterY: height/2;
	property int fontSize: Style.fontSizeNormal;

	property alias tooltipText: tooltip.text;
	property alias tooltipItem: tooltip;


	Component.onCompleted: {
		if(!value){
			value = from;
		}

		position = (value - from)/(to - from)
		controlRecX = position * (width - controlWidth)

		setSizeParams();
	}

	onDecoratorChanged: {
		decoratorChangedFuncBase();

		if(decorator_.to !== undefined){
			decorator_.to = to;
		}
		if(decorator_.from !== undefined){
			decorator_.from = from;
		}

		if(decorator_.value !== undefined){
			decorator_.value = position * (to - from) + from;
		}

		if(decorator_.compl !== undefined){
			decorator_.compl = true;
		}

		setBindTargets();

		setSizeParams()

		controlRecX = position * (width - controlWidth)

	}

	onWidthChanged: {
		controlRecX = position * (width - controlWidth)
	}

	function decoratorChangedFunc(){
		//console.log("Slider: redefinition of base function");
	}

	function setSizeParams(){

		if(!hasIndicator && !hasTicks){
			controlCenterY = controlHeight/2;
			height = controlHeight;
		}
		else if(hasIndicator && !hasTicks && indicatorPosition == Enums.sliderHintBelow/*Qt.AlignBottom*/){
			controlCenterY = controlHeight/2;
			height = controlHeight/2 + indicatorHeight;
		}
		else if(hasIndicator && !hasTicks && indicatorPosition == Enums.sliderHintAbove /*Qt.AlignTop*/){
			controlCenterY = indicatorHeight;
			height = controlHeight/2 + indicatorHeight;
		}
		else if(!hasIndicator && hasTicks && ticksPosition == Enums.ticksBothSides/*Qt.AlignVCenter*/){
			controlCenterY = majorTickHeight/2
			height = majorTickHeight;
		}
		else if(!hasIndicator && hasTicks && ticksPosition == Enums.ticksBelow){
			controlCenterY = controlHeight/2;
			height = controlHeight/2 + majorTickHeight
		}
		else if(!hasIndicator && hasTicks && ticksPosition == Enums.ticksAbove){
			controlCenterY = majorTickHeight;
			height = controlHeight/2 + majorTickHeight
		}
		else if(hasIndicator && hasTicks && ticksPosition == Enums.ticksBelow && indicatorPosition == Enums.sliderHintBelow){
			controlCenterY = controlHeight/2;
			height = controlHeight/2 + Math.max(majorTickHeight, indicatorHeight);
		}
		else if(hasIndicator && hasTicks && ticksPosition == Enums.ticksAbove && indicatorPosition == Enums.sliderHintAbove){
			controlCenterY = Math.max(majorTickHeight, indicatorHeight);
			height = controlHeight/2 + Math.max(majorTickHeight, indicatorHeight);
		}
		else if(hasIndicator && hasTicks && ticksPosition == Enums.ticksAbove && indicatorPosition == Enums.sliderHintBelow){
			controlCenterY = majorTickHeight;
			height = majorTickHeight + indicatorHeight;
		}
		else if(hasIndicator && hasTicks && ticksPosition == Enums.ticksBelow && indicatorPosition == Enums.sliderHintAbove){
			controlCenterY = indicatorHeight;
			height = majorTickHeight + indicatorHeight;
		}

		else if(hasIndicator && hasTicks && ticksPosition == Enums.ticksBothSides && indicatorPosition == Enums.sliderHintAbove){
			controlCenterY = Math.max(indicatorHeight, majorTickHeight/2)
			height = majorTickHeight/2 + controlCenterY;
		}
		else if(hasIndicator && hasTicks && ticksPosition == Enums.ticksBothSides && indicatorPosition == Enums.sliderHintBelow){
			controlCenterY =  majorTickHeight/2
			height = majorTickHeight/2 + indicatorHeight;
		}
	}

	MouseArea{
		id: ma;

		anchors.fill: parent;

		hoverEnabled: true;
		cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;
		property int mouseX_prev: 0;
		property bool canDrag: false;
		property bool canClick: true;
		property real pressedX: 0.0;

		onPressed: {
			pressedX = mouse.x;
			let withinContolX = (mouse.x >= slider.controlRecX && mouse.x <= slider.controlRecX + slider.controlWidth)
			let withinContolY = (mouse.y >= slider.controlCenterY - slider.controlHeight/2 && mouse.y<= slider.controlCenterY + slider.controlHeight/2)
			if(withinContolX && withinContolY){
				ma.canDrag = true;
			}
			if(tooltip.openST){
				tooltip.closeTooltip();
			}

			let add = -slider.controlWidth/2;
			let x_ = mouse.x + add;

			//if(x_ < (slider.controlRecX -slider.controlWidth/2) || x_ > slider.controlRecX + slider.controlWidth/2){
				if(x_ <= 0){
					slider.controlRecX = 0;
				}
				else if(x_ >= slider._backgroundWidth){
					slider.controlRecX = slider._backgroundWidth ;
				}
				else {
					//slider.controlRecX = x_;
					slider.controlRecX = ma.correctX(x_)
				}

				slider.position = slider.controlRecX/(slider._backgroundWidth);
				slider.value = slider.position * (slider.to - slider.from) + slider.from;
			// }


		}
		onReleased: {
			ma.canDrag = false;
			if(ma.canClick){
			}
			ma.canClick = true;

		}

		onPositionChanged:  {
			let add = -slider.controlWidth/2;
			let x_ = mouse.x + add;

			if(ma.canDrag){
				ma.canClick = false;

				if(x_ <= 0){
					slider.controlRecX = 0;
				}
				else if(x_  >= slider._backgroundWidth){
					slider.controlRecX = slider._backgroundWidth;
				}
				else {
					slider.controlRecX = x_;
				}

				slider.position = slider.controlRecX/(slider._backgroundWidth);
				if(slider.position > 0.99999){
					slider.position = 1.
				}
				if(slider.position < 0.00001){
					slider.position = 0.
				}
				slider.value = slider.position * (slider.to - slider.from) + slider.from;
			}

			else if(!slider.hasIndicator){
				let withinContolX = (mouse.x >= slider.controlRecX && mouse.x <= slider.controlRecX + slider.controlWidth)
				let withinContolY = (mouse.y >= slider.controlCenterY - slider.controlHeight/2 && mouse.y<= slider.controlCenterY + slider.controlHeight/2)

				let withinContol = withinContolX && withinContolY

				if(withinContol && !tooltip.openST){
					tooltip.openTooltip(mouseX, mouseY);
				}
				else if(!withinContol && tooltip.openST){
					tooltip.closeTooltip();
				}
			}

		}

		onExited: {
			if(tooltip.openST){
				tooltip.closeTooltip();
			}
		}

		function correctX(argX){
			if(slider.majorTickInterval > 0){
				let tickValue = slider.from
				while (tickValue <= slider.to){
					let tickPosition = (tickValue - slider.from)/(slider.to - slider.from)
					let tickX = tickPosition * (slider.width - slider.controlWidth)
					if(Math.abs((tickX - argX)) < 10){
						return tickX;
					}
					tickValue += slider.majorTickInterval;
				}
			}

			return argX;
		}
	}

	CustomTooltip{
		id: tooltip;
		text: Math.trunc(slider.value);

		fitToTextWidth: true;
	}

}
