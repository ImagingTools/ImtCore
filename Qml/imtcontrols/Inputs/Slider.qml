import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0;
import imtcontrols 1.0

ControlBase {
	id: slider;

	decorator: Style.sliderDecorator

	/**
		\If orientation == Qt.Horizontal width should be set. Height is calculated.
		\Otherwise height should be set. Width is calculated.
	*/

	property real _backgroundWidth: mainSize - controlWidth;
	property int backgroundHeight: 6;
	property string backgroundColor: Style.backgroundColor//Style.selectedColor
	property string controlColor:  Style.imaginToolsAccentColor

	property real backgroundOpacity: 1.;

	property real controlRecX: 0;
	property real controlWidth: Style.controlHeightS;
	property real controlHeight: controlWidth;

	property int orientation: Qt.Horizontal;
	property bool isVertical: orientation == Qt.Vertical
	property real mainSize: orientation == Qt.Vertical ? height : width
	property real secondSize: orientation == Qt.Vertical ? width : height

	property real position: controlRecX/(mainSize - controlWidth);

	property real from: 0.0;
	property real to: 1.0;
	property real value: position * (to - from) + from;

	property int majorTickInterval: 0;
	property int minorTickInterval: 0;

	property bool hasTicks: majorTickInterval > 0;
	property bool hasIndicator: false;
	property int majorTickHeight: Style.controlHeightM;
	property int minorTickHeight: majorTickHeight/2;
	property int indicatorHeight: 2*Style.controlHeightM;

	property int ticksPosition: RelativePosition.verticalCenter
	property int indicatorPosition: RelativePosition.top
	property real controlCenterY: secondSize/2;
	property int fontSize: Style.fontSizeXXS;

	property alias tooltipText: tooltip.text;
	property alias tooltipItem: tooltip;

	property bool marginsChanged: false
	property bool hasTooltip: false;

	Component.onCompleted: {
		correctPositionParams();

		if(!value){
			value = from;
		}

		position = (value - from)/(to - from)
		controlRecX = position * (mainSize - controlWidth)

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

		controlRecX = position * (mainSize - controlWidth)

	}

	onValueChanged: {
		position = (value - from)/(to - from)
		controlRecX = position * (mainSize - controlWidth)
	}

	onWidthChanged: {
		controlRecX = position * (mainSize - controlWidth)
	}

	onTicksPositionChanged: {
		correctPositionParams();
	}
	onIndicatorPositionChanged: {
		correctPositionParams();
	}

	function decoratorChangedFunc(){
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
		let size
		if(!hasIndicator && !hasTicks){
			controlCenterY = controlHeight/2;
			size = controlHeight;
		}
		else if(hasIndicator && !hasTicks && indicatorPosition == RelativePosition.bottom){
			controlCenterY = controlHeight/2;
			size = controlHeight/2 + indicatorHeight;
		}
		else if(hasIndicator && !hasTicks && indicatorPosition == RelativePosition.top){
			controlCenterY = indicatorHeight;
			size = controlHeight/2 + indicatorHeight;
		}
		else if(!hasIndicator && hasTicks && ticksPosition == RelativePosition.verticalCenter){
			controlCenterY = majorTickHeight/2 + fontSize
			size = majorTickHeight + fontSize;
		}
		else if(!hasIndicator && hasTicks && ticksPosition == RelativePosition.bottom){
			controlCenterY = controlHeight/2
			size = controlHeight/2 + majorTickHeight + fontSize
		}
		else if(!hasIndicator && hasTicks && ticksPosition == RelativePosition.top){
			controlCenterY = majorTickHeight + fontSize;
			size = controlHeight/2 + majorTickHeight + fontSize
		}

		else if(hasIndicator && hasTicks && ticksPosition == RelativePosition.bottom && indicatorPosition == RelativePosition.bottom){
			controlCenterY = controlHeight/2;
			size = controlHeight/2 + Math.max(majorTickHeight, indicatorHeight);
		}
		else if(hasIndicator && hasTicks && ticksPosition == RelativePosition.top && indicatorPosition == RelativePosition.top){
			controlCenterY = Math.max(majorTickHeight, indicatorHeight);
			size = controlHeight/2 + Math.max(majorTickHeight, indicatorHeight);
		}
		else if(hasIndicator && hasTicks && ticksPosition == RelativePosition.top && indicatorPosition == RelativePosition.bottom){
			controlCenterY = majorTickHeight + fontSize;
			size = majorTickHeight + indicatorHeight + fontSize;
		}
		else if(hasIndicator && hasTicks && ticksPosition == RelativePosition.bottom && indicatorPosition == RelativePosition.top){
			controlCenterY = indicatorHeight;
			size = majorTickHeight + indicatorHeight + fontSize;
		}

		else if(hasIndicator && hasTicks && ticksPosition == RelativePosition.verticalCenter && indicatorPosition == RelativePosition.top){
			controlCenterY = Math.max(indicatorHeight, majorTickHeight/2)
			size = majorTickHeight/2 + controlCenterY;
		}
		else if(hasIndicator && hasTicks && ticksPosition == RelativePosition.verticalCenter && indicatorPosition == RelativePosition.bottom){
			controlCenterY =  majorTickHeight/2 + fontSize
			size = majorTickHeight/2 + indicatorHeight + fontSize;
		}

		if(orientation == Qt.Vertical){
			width = size;
		}
		else {
			height = size;
		}
	}

	MouseArea{
		id: ma;

		anchors.fill: parent;

		hoverEnabled: true;
		cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor;
		property bool canDrag: false;
		property bool canClick: true;

		onPressed: {
			let margin_ = Style.marginM
			let add = -slider.controlWidth/2;
			let mouseFirstCoord = slider.isVertical ? slider.mainSize - mouse.y : mouse.x
			let mouseSecondCoord = slider.isVertical ? mouse.x : mouse.y

			let x_ = mouseFirstCoord + add;

			let withinContolX = (mouseFirstCoord >= slider.controlRecX - margin_ && mouseFirstCoord <= slider.controlRecX + slider.controlWidth + margin_)
			let withinContolY = (mouseSecondCoord >= slider.controlCenterY - slider.controlHeight/2 - margin_ && mouseSecondCoord<= slider.controlCenterY + slider.controlHeight/2 + margin_)
			if(withinContolX && withinContolY){
				ma.canDrag = true;
			}
			if(tooltip.openST){
				tooltip.closeTooltip();
			}

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
			let mouseFirstCoord = slider.isVertical ? slider.mainSize - mouse.y : mouse.x
			let mouseSecondCoord = slider.isVertical ? mouse.x : mouse.y
			let x_ = mouseFirstCoord + add;

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

			else if(!slider.hasIndicator && slider.hasTooltip){
				let withinContolX = (mouseFirstCoord >= slider.controlRecX && mouseFirstCoord <= slider.controlRecX + slider.controlWidth)
				let withinContolY = (mouseSecondCoord >= slider.controlCenterY - slider.controlHeight/2 && mouseSecondCoord<= slider.controlCenterY + slider.controlHeight/2)

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
			// if(slider.majorTickInterval > 0){
			// 	let tickValue = slider.from
			// 	while (tickValue <= slider.to){
			// 		let tickPosition = (tickValue - slider.from)/(slider.to - slider.from)
			// 		let tickX = tickPosition * (slider.mainSize - slider.controlWidth)
			// 		if(Math.abs((tickX - argX)) < 10){
			// 			return tickX;
			// 		}
			// 		tickValue += slider.majorTickInterval;
			// 	}
			// }

			return argX;
		}
	}

	CustomTooltip{
		id: tooltip;
		text: Math.trunc(slider.value);

		fitToTextWidth: true;
	}

}
