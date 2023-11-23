import QtQuick 2.12

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
    property real scaleStep: 0.22;

    property bool wasMoving: false;
    property bool isPressed: false;

    signal deltaSignal(point delta)
    signal wheelDeltaSignal(int wheelDelta)
    signal positionSignal(point position)

    onPressed: {
        isPressed = true;
        let coord_ = mapToItem(this, mouse.x, mouse.y);
        this.coord = coord_;
        this.coordPressed = coord_;
        if(movingItem){
            this.insideMovingItem = checkInsideMovingItem(movingItem.x, movingItem.y, movingItem.width, movingItem.height);
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
        movingFunction(delta);
    }

    onWheelDeltaSignal: {
        wheelFunction(wheelDelta);
    }

    function movingFunction(delta){
        if(movingItem){
            let withinBorders_ = withinBorders(delta, movingItem.x, movingItem.y, movingItem.width, movingItem.height);

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
             x_ + delta.x > 10
             && y_ + delta.y > 10
             && x_ + delta.x < controlArea.width - width_ - 10
             && y_ + delta.y < controlArea.height - height_ - 10;
        return ok
    }

    function wheelFunction(wheelDelta){
        if(wheelTargetItem && hasWheelControl){
            if(wheelDelta > 0){//up
                wheelTargetItem.scale += scaleStep
            }
            else{//down
                if(wheelTargetItem.scale > scaleStep){
                    wheelTargetItem.scale -= scaleStep
                }
            }
        }
    }


}//controlArea
