import QtQuick 2.12
import Acf 1.0


MouseArea{
    id: changeSize;

    width: rootItem ? rootItem.width + addToSize : 0;
    height:  rootItem ? rootItem.height + addToSize : 0;

    cursorShape: containsPress ? Qt.PointingHandCursor : Qt.ArrowCursor;

	property int addToSize: Style.controlHeightS;
	property int minWidth : Style.sizeHintXXS;
	property int minHeight: Style.sizeHintXXS;

    property Item rootItem: null;

    property var coord: mapToItem(this,0,0);

    Component.onCompleted: {
        if(rootItem){
            changeSize.anchors.centerIn = changeSize.rootItem;
        }
    }

    onRootItemChanged: {
        if(rootItem){
            changeSize.anchors.centerIn = changeSize.rootItem;
        }
    }


    onPressed: {
        this.coord = mapToItem(this,mouse.x,mouse.y)
    }
    onPositionChanged: {
        if(this.rootItem){
            var newCoords = mapToItem(this,mouse.x,mouse.y);
            var deltaX = newCoords.x - this.coord.x;
            var deltaY = newCoords.y - this.coord.y;
            this.coord = newCoords;
            var width_;
            var height_;

            if(newCoords.x >= this.width/2){
                width_ = this.rootItem.width + deltaX;
            }
            else {
                width_  = this.rootItem.width - deltaX;
            }

            if(newCoords.y >= this.height/2){
                height_  = this.rootItem.height + deltaY;
            }
            else {
                height_  =  this.rootItem.height - deltaY;
            }

			if(width_ > this.minWidth){
                this.rootItem.width = width_;
            }
            if(height_ > this.minHeight){
                this.rootItem.height = height_;
            }

        }
    }
}

