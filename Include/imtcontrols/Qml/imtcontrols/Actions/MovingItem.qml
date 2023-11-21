import QtQuick 2.12


MouseArea{
    id: moving;

    width: containerItem ? containerItem.width : 0;
    height:  containerItem ? containerItem.height : 0;
    preventStealing: true;
    cursorShape: containsPress ? Qt.PointingHandCursor : Qt.ArrowCursor;

    property Item globalParent: null;
    property Item containerItem: null;
    property Item movingItem: null;

    property int alwaysVisibleSize: 150;
    property var coord: mapToItem(this,0,0);
    onPressed: {
        if(moving.movingItem && moving.globalParent){
            var coord_ = mapToItem(this,mouse.x,mouse.y);
            this.coord = coord_;
        }
    }
    onPositionChanged: {
        if(moving.movingItem && moving.globalParent){
            var newCoords = mapToItem(this, mouse.x,mouse.y);
            var deltaX = newCoords.x - this.coord.x;
            var deltaY = newCoords.y - this.coord.y;
            var newX = moving.movingItem.x + deltaX
            var newY = moving.movingItem.y + deltaY;

            var checkCoord = mapToItem(moving.globalParent, mouse.x, mouse.y);
            checkCoord.x -= mouse.x
            checkCoord.y -= mouse.y

            var ok = checkCoord.x + deltaX > Math.min(moving.alwaysVisibleSize - moving.movingItem.width, 10)
                    && checkCoord.y + deltaY > 10
                    && checkCoord.x + deltaX < moving.globalParent.width - Math.min(moving.alwaysVisibleSize, moving.movingItem.width + 10)
                    && checkCoord.y + deltaY < moving.globalParent.height - Math.min(moving.alwaysVisibleSize, moving.movingItem.height + 10) ;

            if(ok){
                moving.movingItem.x = newX;
                moving.movingItem.y = newY;
            }
        }
    }
}



