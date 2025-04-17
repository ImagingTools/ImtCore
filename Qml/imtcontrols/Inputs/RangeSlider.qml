import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0;
import imtcontrols 1.0

ControlBase {
	id: slider;

	rotation: orientation == Qt.Vertical ? -90 : 0;

	decorator: Style.rangeSliderDecorator

	property real _backgroundWidth: width - controlWidth;
	property int backgroundHeight: 6;
	property string backgroundColor: "#ffc0cb";
	property string controlColor:  "#CB4154";

	property real backgroundOpacity: 1.;

	property real controlWidth: 20;
	property real controlHeight: controlWidth;

	property int orientation: Qt.Horizontal;

	property real from: 0.0;
	property real to: 1.0;

	property real controlRecXFirst: 0;
	property real positionFirst: controlRecXFirst/(width - controlWidth);
	property real valueFirst: positionFirst * (to - from) + from;

	property real controlRecXSecond: controlWidth;
	property real positionSecond: controlRecXSecond/(width - controlWidth);
	property real valueSecond: positionSecond * (to - from) + from;

	property int majorTickInterval: 0;
	property int minorTickInterval: 0;

	property bool hasTicks: majorTickInterval > 0;
	property bool hasIndicator: false;
	property int majorTickHeight: 30;
	property int minorTickHeight: majorTickHeight/2;
	property int indicatorHeight: 60;

	property int ticksPosition: RelativePosition.verticalCenter;
	property int indicatorPosition: RelativePosition.top
	property real controlCenterY: height/2;
	property int fontSize: Style.fontSizeNormal;

	property alias tooltipText: tooltip.text;
	property alias tooltipItem: tooltip;


	Component.onCompleted: {
		correctPositionParams();

		if(!valueFirst){
			valueFirst = from;
		}
		if(!valueSecond){
			valueSecond = from
		}
		if(valueFirst == from && valueSecond == from){
			ma.canMoveSecond = true;
		}
		positionFirst = (valueFirst - from)/(to - from)
		controlRecXFirst = positionFirst * (width - controlWidth)

		positionSecond = (valueSecond - from)/(to - from)
		controlRecXSecond = positionSecond * (width - controlWidth)

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

		if(decorator_.first !== undefined && decorator_.second !== undefined){
			var firstVal = positionFirst * (to - from) + from;//slider.valueFirst;
			var secondVal = positionSecond * (to - from) + from;//slider.valueSecond;
			decorator_.setValues(firstVal, secondVal);
		}
		if(decorator_.compl !== undefined){
			decorator_.compl = true;
		}

		setBindTargets();

		setSizeParams();

		controlRecXFirst = positionFirst * (width - controlWidth)
		controlRecXSecond = positionSecond * (width - controlWidth)
	}

	onWidthChanged: {
		controlRecXFirst = positionFirst * (width - controlWidth)
		controlRecXSecond = positionSecond * (width - controlWidth)
	}

	onTicksPositionChanged: {
		correctPositionParams();
	}
	onIndicatorPositionChanged: {
		correctPositionParams();
	}


	function decoratorChangedFunc(){
		//console.log("RangeSlider: redefinition of base function");
	}

	function correctPositionParams(){
		if(ticksPosition == RelativePosition.left){
			ticksPosition = RelativePosition.top;
		}
		else if(ticksPosition == RelativePosition.right){
			ticksPosition = RelativePosition.bottom;
		}
		else if(ticksPosition == RelativePosition.horizontalCenter){
			ticksPosition = RelativePosition.verticalCenter;
		}

		if(indicatorPosition == RelativePosition.left){
			indicatorPosition = RelativePosition.top;
		}
		else if(indicatorPosition == RelativePosition.right){
			indicatorPosition = RelativePosition.bottom;
		}
		else if(indicatorPosition == RelativePosition.horizontalCenter || indicatorPosition == RelativePosition.verticalCenter){
			indicatorPosition = RelativePosition.top;
		}
	}

	function setSizeParams(){

		if(!hasIndicator && !hasTicks){
			controlCenterY = controlHeight/2;
			height = controlHeight;
		}
		else if(hasIndicator && !hasTicks && indicatorPosition == RelativePosition.bottom){
			controlCenterY = controlHeight/2;
			height = controlHeight/2 + indicatorHeight;
		}
		else if(hasIndicator && !hasTicks && indicatorPosition == RelativePosition.top){
			controlCenterY = indicatorHeight;
			height = controlHeight/2 + indicatorHeight;
		}
		else if(!hasIndicator && hasTicks && ticksPosition == RelativePosition.verticalCenter){
			controlCenterY = majorTickHeight/2
			height = majorTickHeight;
		}
		else if(!hasIndicator && hasTicks && ticksPosition == RelativePosition.bottom){
			controlCenterY = controlHeight/2;
			height = controlHeight/2 + majorTickHeight
		}
		else if(!hasIndicator && hasTicks && ticksPosition == RelativePosition.top){
			controlCenterY = majorTickHeight;
			height = controlHeight/2 + majorTickHeight
		}
		else if(hasIndicator && hasTicks && ticksPosition == RelativePosition.bottom && indicatorPosition == RelativePosition.bottom){
			controlCenterY = controlHeight/2;
			height = controlHeight/2 + Math.max(majorTickHeight, indicatorHeight);
		}
		else if(hasIndicator && hasTicks && ticksPosition == RelativePosition.top && indicatorPosition == RelativePosition.top){
			controlCenterY = Math.max(majorTickHeight, indicatorHeight);
			height = controlHeight/2 + Math.max(majorTickHeight, indicatorHeight);
		}
		else if(hasIndicator && hasTicks && ticksPosition == RelativePosition.top && indicatorPosition == RelativePosition.bottom){
			controlCenterY = majorTickHeight;
			height = majorTickHeight + indicatorHeight;
		}
		else if(hasIndicator && hasTicks && ticksPosition == RelativePosition.bottom && indicatorPosition == RelativePosition.top){
			controlCenterY = indicatorHeight;
			height = majorTickHeight + indicatorHeight;
		}

		else if(hasIndicator && hasTicks && ticksPosition == RelativePosition.verticalCenter && indicatorPosition == RelativePosition.top){
			controlCenterY = Math.max(indicatorHeight, majorTickHeight/2)
			height = majorTickHeight/2 + controlCenterY;
		}
		else if(hasIndicator && hasTicks && ticksPosition == RelativePosition.verticalCenter && indicatorPosition == RelativePosition.bottom){
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
		property bool canMoveFirst: false;
		property bool canMoveSecond: false;

		onPressed: {
			let add = -slider.controlWidth/2;
			let x_ = mouse.x + add;

			let withinContolXFirst = (mouse.x >= slider.controlRecXFirst && mouse.x <= slider.controlRecXFirst + slider.controlWidth)
			let withinContolXSecond = (mouse.x >= slider.controlRecXSecond && mouse.x <= slider.controlRecXSecond + slider.controlWidth)
			let withinContolY = (mouse.y >= slider.controlCenterY - slider.controlHeight/2 && mouse.y<= slider.controlCenterY + slider.controlHeight/2)
			let withinContolFirst = withinContolXFirst && withinContolY
			let withinContolSecond = withinContolXSecond && withinContolY

			if(withinContolFirst || withinContolSecond){
				ma.canDrag = true;
			}

			if(tooltip.openST){
				tooltip.closeTooltip();
			}

			let isIntersection = slider.controlRecXFirst + slider.controlWidth > slider.controlRecXSecond;

			if(isIntersection && !ma.canMoveFirst && !ma.canMoveSecond){
				ma.canMoveFirst = true;
			}

			//in first handle
			if(x_ >= slider.controlRecXFirst && x_ <= slider.controlRecXFirst + slider.controlWidth){
				if(!isIntersection){
					ma.canMoveFirst = true;
					ma.canMoveSecond = false
				}
			}
			//in second handle
			else if(x_ >= slider.controlRecXSecond && x_ <= slider.controlRecXSecond + slider.controlWidth){
				if(!isIntersection){
					ma.canMoveFirst = false;
					ma.canMoveSecond = true;
				}
			}
			//to the left of the first handle
			else if(x_ < slider.controlRecXFirst){
				slider.controlRecXFirst = ma.correctX(x_);

				slider.positionFirst = slider.controlRecXFirst/(slider._backgroundWidth);
				slider.valueFirst = slider.positionFirst * (slider.to - slider.from) + slider.from;
				ma.canMoveFirst = true;
				ma.canMoveSecond = false
			}
			//to the right of the second handle
			else if(x_ > slider.controlRecXSecond){
				if(x_ >= slider._backgroundWidth){
					slider.controlRecXSecond = slider._backgroundWidth;
				}
				else {
					slider.controlRecXSecond = ma.correctX(x_);
				}

				slider.positionSecond = slider.controlRecXSecond/(slider._backgroundWidth);
				slider.valueSecond = slider.positionSecond * (slider.to - slider.from) + slider.from;

				ma.canMoveFirst = false;
				ma.canMoveSecond = true;
			}
			//between the handles
			else {
				if(x_ - (slider.controlRecXFirst + slider.controlWidth) <= slider.controlRecXSecond - x_){
					slider.controlRecXFirst = ma.correctX(x_)// //x_//- slider.controlWidth ;

					slider.positionFirst = slider.controlRecXFirst/(slider._backgroundWidth);
					slider.valueFirst = slider.positionFirst * (slider.to - slider.from) + slider.from;

					ma.canMoveFirst = true;
					ma.canMoveSecond = false
				}
				else {
					slider.controlRecXSecond = ma.correctX(x_);

					slider.positionSecond = slider.controlRecXSecond/(slider._backgroundWidth);
					slider.valueSecond = slider.positionSecond * (slider.to - slider.from) + slider.from;

					ma.canMoveFirst = false;
					ma.canMoveSecond = true;
				}

			}

		}
		onReleased: {
			ma.canDrag = false;

			let isIntersection = slider.controlRecXFirst + slider.controlWidth > slider.controlRecXSecond;
			if(!isIntersection){
				ma.canMoveFirst = false;
				ma.canMoveSecond = false;
			}
			if(ma.canClick){
			}
			ma.canClick = true;

		}

		onPositionChanged:  {
			let add = -slider.controlWidth/2;
			let x_ = mouse.x + add;

			if(ma.canDrag){
				ma.canClick = false;

				if(ma.canMoveFirst){//first
					if(x_ <= 0){
						slider.controlRecXFirst = 0;
					}
					else if(x_ >= slider.controlRecXSecond){
						slider.controlRecXFirst = slider.controlRecXSecond;
					}
					else {
						slider.controlRecXFirst = x_;
					}

					if(slider.controlRecXSecond - slider.controlRecXFirst < slider.controlWidth/2){
						slider.controlRecXFirst = slider.controlRecXSecond;
					}

					slider.positionFirst = slider.controlRecXFirst/(slider._backgroundWidth);
					if(slider.positionFirst < 0.00001){
						slider.positionFirst = 0.
					}
					slider.valueFirst = slider.positionFirst * (slider.to - slider.from) + slider.from;

				}

				else if(ma.canMoveSecond){//second
					if(x_ <= slider.controlRecXFirst){
						slider.controlRecXSecond = slider.controlRecXFirst;
					}
					else if(x_  >= slider._backgroundWidth){
						slider.controlRecXSecond = slider._backgroundWidth;
					}
					else {
						slider.controlRecXSecond = x_;
					}

					slider.positionSecond = slider.controlRecXSecond/(slider._backgroundWidth);
					if(slider.positionSecond > 0.99999){
						slider.positionSecond = 1.
					}
					slider.valueSecond = slider.positionSecond * (slider.to - slider.from) + slider.from;

				}

			}

			else  if(!slider.hasIndicator){
				let withinContolXFirst = (mouse.x >= slider.controlRecXFirst && mouse.x <= slider.controlRecXFirst + slider.controlWidth)
				let withinContolXSecond = (mouse.x >= slider.controlRecXSecond && mouse.x <= slider.controlRecXSecond + slider.controlWidth)
				let withinContolY = (mouse.y >= slider.controlCenterY - slider.controlHeight/2 && mouse.y<= slider.controlCenterY + slider.controlHeight/2)
				let withinContolFirst = withinContolXFirst && withinContolY
				let withinContolSecond = withinContolXSecond && withinContolY

				if(withinContolFirst && !withinContolSecond && !tooltip.openST){
					tooltip.text = Math.trunc(valueFirst);
					tooltip.openTooltip(mouseX, mouseY);
				}
				if(!withinContolFirst && withinContolSecond && !tooltip.openST){
					tooltip.text = Math.trunc(valueSecond);
					tooltip.openTooltip(mouseX, mouseY);
				}
				else if(!withinContolFirst && !withinContolSecond && tooltip.openST){
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

		fitToTextWidth: true;
	}

}
