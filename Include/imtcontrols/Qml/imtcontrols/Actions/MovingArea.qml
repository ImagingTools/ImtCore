import QtQuick 2.12

MouseArea{
    id: moving;

    preventStealing: true;

    property Item movingItem: null;
    property Item wheelTargetItem: null;
    property bool insideMovingItem: false;
    property var coord: mapToItem(this,0,0);
    property var coordPressed: mapToItem(this,0,0);

    property bool hasWheelControl: true;
    property real scaleStep: 0.22;

    property bool wasMoving: false;

    signal deltaSignal(point delta)
    signal wheelDeltaSignal(int wheelDelta)

    onClicked: {
        let coord_ = mapToItem(this, mouse.x, mouse.y);
        this.coord = coord_;
        this.coordPressed = coord_;
        this.insideMovingItem = checkInsideMovingItem();
    }

    onPressed: {
        let coord_ = mapToItem(this, mouse.x, mouse.y);
        this.coord = coord_;
        this.coordPressed = coord_;
        this.insideMovingItem = checkInsideMovingItem();
        wasMoving = false;
    }

    onPositionChanged: {
        var newCoords = mapToItem(this, mouse.x, mouse.y);
        var deltaX_ = newCoords.x - this.coord.x;
        var deltaY_ = newCoords.y - this.coord.y;
        this.coord = newCoords;

        moving.deltaSignal(Qt.point(deltaX_, deltaY_));
        wasMoving = true;

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
            let withinBorders_ = withinBorders(delta);

            if(insideMovingItem && withinBorders_){
                movingItem.x += delta.x;
                movingItem.y += delta.y;
            }
        }
    }

    function checkInsideMovingItem(){
        let ok = false
        if(movingItem){
             ok = moving.coordPressed.x >= movingItem.x
                && moving.coordPressed.y >= movingItem.y
                && moving.coordPressed.x <= movingItem.x + movingItem.width
                && moving.coordPressed.y <= movingItem.y + movingItem.height
        }
        return ok
    }



    function withinBorders(delta){
        let ok = false
        if(movingItem){
             ok =
             movingItem.x + delta.x > 10
             && movingItem.y + delta.y > 10
             && movingItem.x + delta.x < moving.width - moving.movingItem.width - 10
             && movingItem.y + delta.y < moving.height - moving.movingItem.height - 10;
        }
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

    //реализация

}//moving
