import QtQuick 2.12
import Acf 1.0

MouseArea{
    id: controlArea;

    preventStealing: true;
    hoverEnabled: true;

    property Item movingItem: null;
    property Item wheelTargetItem: null;
    property bool insideMovingItem: false;
    property var coord: mapToItem(this,0,0);
    property var coordPressed: mapToItem(this,0,0);

    property bool hasWheelControl: true;
    property real scaleStep: 0.1;
	property int mainMargin: Style.sizeMainMargin;

    property bool wasMoving: false;
    property bool isPressed: false;

    signal deltaSignal(point delta)
    signal wheelDeltaSignal(int wheelDelta)
    signal positionSignal(point position)
	signal rightClicked(real mouseX, real mouseY)

    onPressed: {
        isPressed = true;
        let coord_ = mapToItem(this, mouse.x, mouse.y);
        this.coord = coord_;
        this.coordPressed = coord_;
        if(movingItem){
			let scaleOffsetX = (movingItem.width - movingItem.width * movingItem.scale)/2
			let scaleOffsetY = (movingItem.height - movingItem.height * movingItem.scale)/2

			let x_ = movingItem.x + scaleOffsetX
			let y_ = movingItem.y + scaleOffsetY;
			let width_ = movingItem.width * movingItem.scale
			let height_ = movingItem.height * movingItem.scale
			this.insideMovingItem = checkInsideMovingItem(x_, y_, width_, height_);
        }

        wasMoving = false;
    }
    onReleased: {
        isPressed = false;
    }

    onPositionChanged: {
        if(isPressed){
            var newCoords = mapToItem(this, mouse.x, mouse.y);
            var deltaX_ = newCoords.x - this.coord.x;
            var deltaY_ = newCoords.y - this.coord.y;
            this.coord = newCoords;

            controlArea.deltaSignal(Qt.point(deltaX_, deltaY_));

            wasMoving = true;
        }
        else {
            controlArea.positionSignal(Qt.point(mouse.x, mouse.y))
        }
    }

    onWheel: {
        wheelDeltaSignal(wheel.angleDelta.y)
    }

    onDeltaSignal: {
        //console.log("Delta:: ", delta.x, delta.y);
        if(movingItem){
            movingFunction(delta);
        }
    }

    onWheelDeltaSignal: {
        wheelFunction(wheelDelta);
    }

	onClicked: {
		if (mouse.button == Qt.RightButton){
			rightClicked(mouse.x, mouse.y);
		}
	}

    function movingFunction(delta){
        if(movingItem){
			let scaleOffsetX = (movingItem.width - movingItem.width * movingItem.scale)/2
			let scaleOffsetY = (movingItem.height - movingItem.height * movingItem.scale)/2

			let x_ = movingItem.x + scaleOffsetX
			let y_ = movingItem.y + scaleOffsetY;
			let width_ = movingItem.width * movingItem.scale
			let height_ = movingItem.height * movingItem.scale
			let withinBorders_ = withinBorders(delta, x_, y_, width_, height_);

            if(insideMovingItem && withinBorders_){
                movingItem.x += delta.x;
                movingItem.y += delta.y;
            }
        }
    }

    function checkInsideMovingItem(x_, y_, width_, height_){
        let ok = false
             ok = controlArea.coordPressed.x >= x_
                && controlArea.coordPressed.y >= y_
                && controlArea.coordPressed.x <= x_ + width_
                && controlArea.coordPressed.y <= y_ + height_
        return ok
    }

    function checkHoverItem(x_, y_, width_, height_, coord_){
        let ok = false
             ok = coord_.x >= x_
                && coord_.y >= y_
                && coord_.x <= x_ + width_
                && coord_.y <= y_ + height_
        return ok
    }



    function withinBorders(delta, x_, y_, width_, height_){
        let ok = false
             ok =
			 x_ + delta.x > controlArea.mainMargin
			 && y_ + delta.y > controlArea.mainMargin
			 && x_ + delta.x < controlArea.width - width_ - controlArea.mainMargin
			 && y_ + delta.y < controlArea.height - height_ - controlArea.mainMargin;
        return ok
    }

    function wheelFunction(wheelDelta){
		if(wheelTargetItem && hasWheelControl && withinWheelTargetItem()){
            if(wheelDelta > 0){//up
                wheelTargetItem.scale += scaleStep
            }
            else{//down
				if(wheelTargetItem.scale > 2* scaleStep){
                    wheelTargetItem.scale -= scaleStep
                }
            }
        }
    }

	function withinWheelTargetItem () {
		let ok = false;
		let scaleOffsetX = (wheelTargetItem.width - wheelTargetItem.width * wheelTargetItem.scale)/2
		let scaleOffsetY = (wheelTargetItem.height - wheelTargetItem.height * wheelTargetItem.scale)/2
		ok =
				mouseX >= wheelTargetItem.x + scaleOffsetX
				&& mouseX <= wheelTargetItem.x + scaleOffsetX + wheelTargetItem.width * wheelTargetItem.scale
				&& mouseY >= wheelTargetItem.y + scaleOffsetY
				&& mouseY <= wheelTargetItem.y + scaleOffsetY + wheelTargetItem.height * wheelTargetItem.scale

		return ok;

	}


}//controlArea
