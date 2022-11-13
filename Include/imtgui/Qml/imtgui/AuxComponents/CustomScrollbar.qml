import QtQuick 2.12

Rectangle{
    id:scrollContainer;

    height: vertical ? targetItem.height : secondSize;
    width: vertical ? secondSize : targetItem.width;
    color: backgroundColor;
    radius: 4;

    property var targetItem;
    property bool vertical: true;
    property real minSize: 30;
    property real secondSize: 20;
    property int indicatorRadius: 4;
    property string backgroundColor: "#efefef";
    property string indicatorColor: "gray";
    property string highlightColor: "#efefef";

    property bool canFade: false;
    property bool isMoving: false;
    property int pauseDuration: 300;


    property real koeff: vertical ? (scrollIndicator.height > scrollContainer.minSize ? 1 :(targetItem.height-scrollContainer.minSize)/(targetItem.contentHeight-targetItem.height)):
                                    (scrollIndicator.width > scrollContainer.minSize ? 1 :(targetItem.width-scrollContainer.minSize)/(targetItem.contentWidth-targetItem.width));


    property bool visibleState: !canFade ? 1 : isMoving ? 1 : (scrollContainerMA.containsMouse || scrollMA.containsMouse) ? 1 : 0;

    property bool inSideTarget: false;


    Component.onCompleted: {
        scrollContainer.opacity = !scrollContainer.canFade;
    }

    onVisibleStateChanged: {
        if(scrollContainer.visibleState){
            animTo.start();
        }
        else {
            animFrom.start();
        }

    }


    PauseAnimation {
        id: fadePause;

        duration: 300;
        onFinished: {
            scrollContainer.isMoving = false;
        }
        onStarted: {
            scrollContainer.isMoving = true;
        }
    }


    NumberAnimation {
        id: animTo;

        target: scrollContainer;
        property: "opacity";
        duration: scrollContainer.pauseDuration;
        from: 0; to: 1;
    }

    NumberAnimation {
        id: animFrom;

        target: scrollContainer;
        property: "opacity";
        duration: scrollContainer.pauseDuration;
        from: 1; to: 0;
    }

    NumberAnimation {
        id: animWheelY;

        target: scrollContainer.targetItem;
        property: "contentY";
        duration: 50;
        from: 0; to: 1;
    }

    NumberAnimation {
        id: animWheelX;

        target: scrollContainer.targetItem;
        property: "contentX";
        duration: 50;
        from: 0; to: 1;
    }


    MouseArea{
        id:scrollContainerMA;

        anchors.fill: parent;
        enabled: scrollContainer.visible;
        hoverEnabled: enabled;
        property int delta: 40;
        //cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
        onClicked: {
            var newCoords = mapToItem(scrollContainer,mouse.x,mouse.y);
            if(scrollContainer.vertical){
                scrollContainer.targetItem.contentY = (newCoords.y >= scrollIndicator.y + scrollIndicator.height) ?
                            (newCoords.y - scrollIndicator.height) / scrollContainer.koeff : newCoords.y / scrollContainer.koeff;
            }
            else{
                scrollContainer.targetItem.contentX = (newCoords.x >= scrollIndicator.x + scrollIndicator.width) ?
                            (newCoords.x - scrollIndicator.width) / scrollContainer.koeff : newCoords.x / scrollContainer.koeff;

            }

            scrollMA.coord = newCoords;
        }

        onPressed: {
            scrollContainer.color = scrollContainer.highlightColor;
        }
        onReleased: {
            scrollContainer.color = scrollContainer.backgroundColor;
        }

        onWheel: {

            if(scrollContainer.inSideTarget){
                wheel.accepted = false;
            }
            else {

                if(scrollContainer.vertical){

                    if(scrollContainer.targetItem.contentHeight > scrollContainer.targetItem.height){

                        if(wheel.angleDelta.y > 0){
                            if((scrollContainer.targetItem.contentY - scrollContainerMA.delta)>=0){
                                //scrollContainer.targetItem.contentY -= scrollContainerMA.delta;
                                animWheelY.from = scrollContainer.targetItem.contentY;
                                animWheelY.to = scrollContainer.targetItem.contentY - scrollContainerMA.delta;
                                animWheelY.start();

                            }
                            else{
                                //scrollContainer.targetItem.contentY = 0;
                                animWheelY.from = scrollContainer.targetItem.contentY;
                                animWheelY.to = 0;
                                animWheelY.start();
                            }
                        }
                        else{
                            if((scrollContainer.targetItem.contentY + scrollContainerMA.delta)< (scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height)){
                                //scrollContainer.targetItem.contentY += scrollContainerMA.delta;
                                animWheelY.from = scrollContainer.targetItem.contentY;
                                animWheelY.to = scrollContainer.targetItem.contentY + scrollContainerMA.delta;
                                animWheelY.start();
                            }
                            else{
                                //scrollContainer.targetItem.contentY = scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height;
                                animWheelY.from = scrollContainer.targetItem.contentY;
                                animWheelY.to = scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height;
                                animWheelY.start();

                            }
                        }
                    }

                }//vertical

                else{

                    if(scrollContainer.targetItem.contentWidth > scrollContainer.targetItem.width){

                        if(wheel.angleDelta.x > 0){
                            if((scrollContainer.targetItem.contentX - scrollContainerMA.delta) >= 0){
                                //scrollContainer.targetItem.contentX -= scrollContainerMA.delta;
                                animWheelX.from = scrollContainer.targetItem.contentX;
                                animWheelX.to = scrollContainer.targetItem.contentX - scrollContainerMA.delta;
                                animWheelX.start();

                            }
                            else{
                                //scrollContainer.targetItem.contentX = 0;
                                animWheelX.from = scrollContainer.targetItem.contentX;
                                animWheelX.to = 0;
                                animWheelX.start();
                            }
                        }
                        else{
                            if((scrollContainer.targetItem.contentX + scrollContainerMA.delta)< (scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width)){
                                //scrollContainer.targetItem.contentX += scrollContainerMA.delta;
                                animWheelX.from = scrollContainer.targetItem.contentX;
                                animWheelX.to = scrollContainer.targetItem.contentX + scrollContainerMA.delta;
                                animWheelX.start();
                            }
                            else{
                                //scrollContainer.targetItem.contentX = scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width;
                                animWheelX.from = scrollContainer.targetItem.contentX;
                                animWheelX.to = scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width;
                                animWheelX.start();

                            }
                        }
                    }

                }//horizontal

                 wheel.accepted = true;
            }//
        }//onWheel
    }

    Rectangle{
        id:scrollIndicator;

        x: scrollContainer.vertical ? (parent.width - width)/2 :
                                      (scrollIndicator.width > scrollContainer.minSize ?  scrollContainer.targetItem.contentX :
                                                                                         scrollContainer.targetItem.contentX*(scrollContainer.targetItem.width - scrollContainer.minSize)/(scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width));

        y: scrollContainer.vertical ? (scrollIndicator.height > scrollContainer.minSize ?  scrollContainer.targetItem.contentY :
                                                                                          scrollContainer.targetItem.contentY*(scrollContainer.targetItem.height - scrollContainer.minSize)/(scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height)):
                                      (parent.height - height)/2;


        width: scrollContainer.vertical ? (parent.width - 4) :
                                          scrollContainer.targetItem.contentWidth <= scrollContainer.targetItem.width ? scrollContainer.targetItem.width :
                                                                                                                        (scrollContainer.targetItem.width - (scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width)) < scrollContainer.minSize ? scrollContainer.minSize :
                                                                                                                                                                                                                                                                      (scrollContainer.targetItem.width - (scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width));

        height: scrollContainer.vertical ? (scrollContainer.targetItem.contentHeight <= scrollContainer.targetItem.height ? scrollContainer.targetItem.height :
                                                                                                                            (scrollContainer.targetItem.height - (scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height)) < scrollContainer.minSize ? scrollContainer.minSize :
                                                                                                                                                                                                                                                                             (scrollContainer.targetItem.height - (scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height))):
                                           (parent.height - 4);

        radius: scrollContainer.indicatorRadius;
        color: scrollContainer.indicatorColor;
        opacity: scrollMA.pressed ? 1 : scrollMA.containsMouse ? 0.8 : 0.6;

        onXChanged:{
            if(!scrollMA.dragEnabled && !scrollContainer.vertical){
                scrollMA.coord = Qt.point(scrollIndicator.x,scrollIndicator.y);
            }
            if(x !== 0 && !scrollContainer.vertical && scrollContainer.canFade){
                fadePause.stop();
                fadePause.start();

            }
        }

        onYChanged:{
            if(!scrollMA.dragEnabled && scrollContainer.vertical){
                scrollMA.coord = Qt.point(scrollIndicator.x,scrollIndicator.y);
            }
            if(y !== 0 && scrollContainer.vertical && scrollContainer.canFade){
                fadePause.stop();
                fadePause.start();

            }
        }


        MouseArea{
            id:scrollMA;

            anchors.fill: parent;
            property bool dragEnabled: false;
            property var coord :mapToItem(scrollContainer,0,0);
            enabled: parent.visible;
            hoverEnabled: enabled;
            //cursorShape: containsMouse ? Qt.PointingHandCursor: Qt.ArrowCursor;
            onWheel: {

                if(scrollContainer.inSideTarget){
                    wheel.accepted = false;
                }
                else {

                    if(scrollContainer.vertical){

                        if(scrollContainer.targetItem.contentHeight > scrollContainer.targetItem.height){

                            if(wheel.angleDelta.y > 0){
                                if((scrollContainer.targetItem.contentY - scrollContainerMA.delta)>=0){
                                    //scrollContainer.targetItem.contentY -= scrollContainerMA.delta;
                                    animWheelY.from = scrollContainer.targetItem.contentY;
                                    animWheelY.to = scrollContainer.targetItem.contentY - scrollContainerMA.delta;
                                    animWheelY.start();

                                }
                                else{
                                    //scrollContainer.targetItem.contentY = 0;
                                    animWheelY.from = scrollContainer.targetItem.contentY;
                                    animWheelY.to = 0;
                                    animWheelY.start();
                                }
                            }
                            else{
                                if((scrollContainer.targetItem.contentY + scrollContainerMA.delta)< (scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height)){
                                    //scrollContainer.targetItem.contentY += scrollContainerMA.delta;
                                    animWheelY.from = scrollContainer.targetItem.contentY;
                                    animWheelY.to = scrollContainer.targetItem.contentY + scrollContainerMA.delta;
                                    animWheelY.start();
                                }
                                else{
                                    //scrollContainer.targetItem.contentY = scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height;
                                    animWheelY.from = scrollContainer.targetItem.contentY;
                                    animWheelY.to = scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height;
                                    animWheelY.start();

                                }
                            }
                        }

                    }//vertical

                    else{

                        if(scrollContainer.targetItem.contentWidth > scrollContainer.targetItem.width){

                            if(wheel.angleDelta.x > 0){
                                if((scrollContainer.targetItem.contentX - scrollContainerMA.delta) >= 0){
                                    //scrollContainer.targetItem.contentX -= scrollContainerMA.delta;
                                    animWheelX.from = scrollContainer.targetItem.contentX;
                                    animWheelX.to = scrollContainer.targetItem.contentX - scrollContainerMA.delta;
                                    animWheelX.start();

                                }
                                else{
                                    //scrollContainer.targetItem.contentX = 0;
                                    animWheelX.from = scrollContainer.targetItem.contentX;
                                    animWheelX.to = 0;
                                    animWheelX.start();
                                }
                            }
                            else{
                                if((scrollContainer.targetItem.contentX + scrollContainerMA.delta)< (scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width)){
                                    //scrollContainer.targetItem.contentX += scrollContainerMA.delta;
                                    animWheelX.from = scrollContainer.targetItem.contentX;
                                    animWheelX.to = scrollContainer.targetItem.contentX + scrollContainerMA.delta;
                                    animWheelX.start();
                                }
                                else{
                                    //scrollContainer.targetItem.contentX = scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width;
                                    animWheelX.from = scrollContainer.targetItem.contentX;
                                    animWheelX.to = scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width;
                                    animWheelX.start();

                                }
                            }
                        }

                    }//horizontal

                    wheel.accepted = true;
                }//
            }//onWheel

            onPressed: {
                scrollMA.dragEnabled = true;
                var newCoords = mapToItem(scrollContainer,mouse.x,mouse.y);
                scrollMA.coord = newCoords;
            }
            onReleased: {
                scrollMA.dragEnabled = false;
            }
            onPositionChanged: {
                if(scrollMA.dragEnabled)
                {
                    var newCoords = mapToItem(scrollContainer,mouse.x,mouse.y);

                    if(vertical){
                        var deltaY = newCoords.y - scrollMA.coord.y;
                        if(scrollContainer.targetItem.contentY + deltaY <= 0){
                            scrollContainer.targetItem.contentY = 0;
                        }
                        else if(scrollContainer.targetItem.contentY + deltaY >= scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height){
                            scrollContainer.targetItem.contentY = scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height;
                        }
                        else {
                            scrollContainer.targetItem.contentY += deltaY/scrollContainer.koeff;
                        }
                    }
                    else{
                        var deltaX = newCoords.x - scrollMA.coord.x;
                        if(scrollContainer.targetItem.contentX + deltaX <= 0){
                            scrollContainer.targetItem.contentX = 0;
                        }
                        else if(scrollContainer.targetItem.contentX + deltaX >= scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width){
                            scrollContainer.targetItem.contentX = scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width;
                        }
                        else {
                            scrollContainer.targetItem.contentX += deltaX/scrollContainer.koeff;
                        }
                    }

                    scrollMA.coord = newCoords;

                }
            }

        }


    }
}
