import QtQuick 2.12
import Acf 1.0

Rectangle{
    id: scrollContainer;

    height: targetItem && vertical ? targetItem.height : !decorator ? secondSize : secondSizeDecorator;
    width: targetItem && !vertical ? targetItem.width : !decorator ? secondSize : secondSizeDecorator;
    color: backgroundColor;
    radius: 4;

    visible: vertical ? targetItem.contentHeight > scrollContainer.height : targetItem.contentWidth > scrollContainer.width;

    property var targetItem: null;

    property bool vertical: true;

    property real minSize: 20;
    property real secondSize: 20;
    property int indicatorRadius: 4;
    property string backgroundColor: decorator ? "transparent" : Style.color_scrollBackground !== undefined ? Style.color_scrollBackground  : "#efefef";
    property string indicatorColor: decorator ? "transparent" : Style.color_scrollIndicator !== undefined ? Style.color_scrollIndicator : "lightgray";
    property string highlightColor: decorator ? "transparent" : Style.color_scrollHighlight !== undefined ? Style.color_scrollHighlight : "lightgray";

    property bool canFade: false;
    property bool isMoving: false;
    property int pauseDuration: 300;
    property int indicatorMargin: 0;

    property bool notUsed: !targetItem ? true : vertical ? targetItem.contentHeight <= targetItem.height:
                                                           targetItem.contentWidth <= targetItem.width;
    property bool hideNotUsed: true;

    property real koeff: !targetItem ? 1 : vertical ? (targetItem.height - scrollIndicator.height)/(targetItem.contentHeight - targetItem.height):
                                    (targetItem.width - scrollIndicator.width)/(targetItem.contentWidth - targetItem.width);


    property bool visibleState: !canFade ? 1 : isMoving ? 1 : (scrollContainerMA.containsMouse || scrollMA.containsMouse) ? 1 : 0;
    property bool inSideTarget: false;

    property int targetContentHeight: targetItem.contentHeight;
    property int targetContentWidth: targetItem.contentWidth;


    /*for Qt Style decoration*/
    property Component decoratorComponent : Style.isQtStyle ? DecoratorsQt.scrollBarDecorator : null;
    property var decorator : null;

    property real scrollIndicatorY: scrollIndicator.y;
    property real scrollIndicatorX: scrollIndicator.x;

    property real secondSizeDecorator: !decorator ? 0 : vertical ? decorator.background.width : decorator.background.height;


    onDecoratorComponentChanged: {
        if(decorator){
            decorator.destroy();
        }

        if(!decoratorComponent){
            return;
        }

        decorator = decoratorComponent.createObject(scrollContainer);
        decorator.baseElement = scrollContainer;

        if(scrollContainer.vertical){
            decorator.position = scrollIndicatorY/scrollContainer.height;
        }
        else {
            decorator.position = scrollIndicatorX/scrollContainer.width;
        }

    }

    onScrollIndicatorYChanged: {
        if(decorator && vertical){
            decorator.position = scrollIndicatorY/scrollContainer.height;
        }
    }

    onScrollIndicatorXChanged: {
        if(decorator && !vertical){
            decorator.position = scrollIndicatorX/scrollContainer.width;
        }
    }

    function setContentPositionFunc(position_){
        if(scrollContainer.vertical){
            scrollContainer.targetItem.contentY =
            position_ * scrollContainer.height / scrollContainer.koeff + scrollContainer.targetItem.originY;
        }
        else {
            scrollContainer.targetItem.contentX =
            position_ * scrollContainer.width / scrollContainer.koeff + scrollContainer.targetItem.originX;
        }

    }
    /*for Qt Style decoration*/


    Component.onCompleted: {
        scrollContainer.opacity = !scrollContainer.canFade;
    }

    onHeightChanged: {
        setVisibleFunc();
    }

    onWidthChanged: {
        setVisibleFunc();
    }

    onTargetContentHeightChanged: {
        setVisibleFunc();
    }

    onTargetContentWidthChanged: {
        setVisibleFunc();
    }

    onVisibleStateChanged: {
        if(canFade){
            if(scrollContainer.visibleState){
                animTo.start();
            }
            else {
                animFrom.start();
            }
        }
    }

    function setVisibleFunc(){
        if(targetItem){
            if(vertical){
                visible = targetItem.contentHeight > scrollContainer.height;
            }
            else {
                visible = targetItem.contentWidth > scrollContainer.width;
            }
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

        //        visible: scrollContainer.visible && !scrollContainer.notUsed;
        visible: scrollContainer.visible;
        enabled: visible;
        hoverEnabled: enabled;
        property int delta: 40;
        preventStealing: true;
        //cursorShape: containsMouse ? Qt.PointingHandCursor : Qt.ArrowCursor
        onClicked: {
            var newCoords = mapToItem(scrollContainer, mouse.x, mouse.y);
            if(scrollContainer.vertical){
                scrollContainer.targetItem.contentY = (newCoords.y >= scrollIndicator.y + scrollIndicator.height) ?
                            (newCoords.y - scrollIndicator.height) / scrollContainer.koeff + scrollContainer.targetItem.originY : newCoords.y / scrollContainer.koeff + scrollContainer.targetItem.originY;
            }

            else{//horiz
                scrollContainer.targetItem.contentX = (newCoords.x >= scrollIndicator.x + scrollIndicator.width) ?
                            (newCoords.x - scrollIndicator.width) / scrollContainer.koeff + scrollContainer.targetItem.originX : newCoords.x / scrollContainer.koeff + scrollContainer.targetItem.originX;

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

                        if(wheel.angleDelta.y > 0){//up
                            if((scrollContainer.targetItem.contentY - scrollContainer.targetItem.originY - scrollContainerMA.delta)>=0){
                                //scrollContainer.targetItem.contentY -= scrollContainerMA.delta;
                                animWheelY.from = scrollContainer.targetItem.contentY;
                                animWheelY.to = scrollContainer.targetItem.contentY - scrollContainerMA.delta;
                                animWheelY.start();

                            }
                            else {
                                //scrollContainer.targetItem.contentY = scrollContainer.targetItem.originY;
                                animWheelY.from = scrollContainer.targetItem.contentY;
                                animWheelY.to = scrollContainer.targetItem.originY;
                                animWheelY.start();
                            }
                        }
                        else {//down
                            if((scrollContainer.targetItem.contentY - scrollContainer.targetItem.originY + scrollContainerMA.delta)< (scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height)){
                                //scrollContainer.targetItem.contentY += scrollContainerMA.delta;
                                animWheelY.from = scrollContainer.targetItem.contentY;
                                animWheelY.to = scrollContainer.targetItem.contentY + scrollContainerMA.delta;
                                animWheelY.start();
                            }
                            else{
                                //scrollContainer.targetItem.contentY = scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height + scrollContainer.targetItem.originY;
                                animWheelY.from = scrollContainer.targetItem.contentY;
                                animWheelY.to = scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height + scrollContainer.targetItem.originY;
                                animWheelY.start();

                            }
                        }
                    }

                }//vertical

                else{//horiz

                    if(scrollContainer.targetItem.contentWidth > scrollContainer.targetItem.width){

                        if(wheel.angleDelta.y > 0){//left
                            if((scrollContainer.targetItem.contentX - scrollContainer.targetItem.originX - scrollContainerMA.delta) >= 0){
                                //scrollContainer.targetItem.contentX -= scrollContainerMA.delta;
                                animWheelX.from = scrollContainer.targetItem.contentX;
                                animWheelX.to = scrollContainer.targetItem.contentX - scrollContainerMA.delta;
                                animWheelX.start();

                            }
                            else{
                                //scrollContainer.targetItem.contentX = scrollContainer.targetItem.originX;
                                animWheelX.from = scrollContainer.targetItem.contentX;
                                animWheelX.to = scrollContainer.targetItem.originX;
                                animWheelX.start();
                            }
                        }
                        else {//right
                            if((scrollContainer.targetItem.contentX  - scrollContainer.targetItem.originX + scrollContainerMA.delta)< (scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width)){
                                //scrollContainer.targetItem.contentX += scrollContainerMA.delta;
                                animWheelX.from = scrollContainer.targetItem.contentX;
                                animWheelX.to = scrollContainer.targetItem.contentX + scrollContainerMA.delta;
                                animWheelX.start();
                            }
                            else{
                                //scrollContainer.targetItem.contentX = scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width  + scrollContainer.targetItem.originY;
                                animWheelX.from = scrollContainer.targetItem.contentX;
                                animWheelX.to = scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width  + scrollContainer.targetItem.originY;
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
           (scrollContainer.targetItem.contentX - scrollContainer.targetItem.originX)*(scrollContainer.targetItem.width - scrollIndicator.width)/(scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width)

        y: !scrollContainer.vertical ? (parent.height - height)/2:
           (scrollContainer.targetItem.contentY - scrollContainer.targetItem.originY)*(scrollContainer.targetItem.height - scrollIndicator.height)/(scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height)

        width: scrollContainer.vertical ? (parent.width - 2*scrollContainer.indicatorMargin) :
                                          (scrollContainer.targetItem.contentWidth <= scrollContainer.targetItem.width ? scrollContainer.targetItem.width :
                                                                                                                           (scrollContainer.targetItem.width / scrollContainer.targetItem.contentWidth * scrollContainer.targetItem.width)) < scrollContainer.minSize ?
                                                                                                                               scrollContainer.minSize :
                                                                                                                               (scrollContainer.targetItem.width / scrollContainer.targetItem.contentWidth * scrollContainer.targetItem.width);

        height: !scrollContainer.vertical ? (parent.height - 2*scrollContainer.indicatorMargin) :
                                            (scrollContainer.targetItem.contentHeight <= scrollContainer.targetItem.height ? scrollContainer.targetItem.height :
                                                                                                                             (scrollContainer.targetItem.height / scrollContainer.targetItem.contentHeight * scrollContainer.targetItem.height)) < scrollContainer.minSize ?
                                                                                                                                 scrollContainer.minSize :
                                                                                                                                 (scrollContainer.targetItem.height / scrollContainer.targetItem.contentHeight * scrollContainer.targetItem.height);


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

            //            visible: parent.visible && !scrollContainer.notUsed;
            visible: scrollContainer.visible;
            enabled: visible;
            hoverEnabled: enabled;
            preventStealing: true;

            property bool dragEnabled: false;

            onDragEnabledChanged: {
                //console.log("onDragEnabledChanged", dragEnabled)
            }

            property var coord: mapToItem(scrollContainer,0,0);
            //cursorShape: containsMouse ? Qt.PointingHandCursor: Qt.ArrowCursor;
            onWheel: {

                if(scrollContainer.inSideTarget){
                    wheel.accepted = false;
                }
                else {

                    if(scrollContainer.vertical){

                        if(scrollContainer.targetItem.contentHeight > scrollContainer.targetItem.height){

                            if(wheel.angleDelta.y > 0){//up
                                if((scrollContainer.targetItem.contentY - scrollContainer.targetItem.originY - scrollContainerMA.delta)>=0){
                                    //scrollContainer.targetItem.contentY -= scrollContainerMA.delta;
                                    animWheelY.from = scrollContainer.targetItem.contentY;
                                    animWheelY.to = scrollContainer.targetItem.contentY - scrollContainerMA.delta;
                                    animWheelY.start();

                                }
                                else {
                                    //scrollContainer.targetItem.contentY = scrollContainer.targetItem.originY;
                                    animWheelY.from = scrollContainer.targetItem.contentY;
                                    animWheelY.to = scrollContainer.targetItem.originY;
                                    animWheelY.start();
                                }
                            }
                            else {//down
                                if((scrollContainer.targetItem.contentY - scrollContainer.targetItem.originY + scrollContainerMA.delta)< (scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height)){
                                    //scrollContainer.targetItem.contentY += scrollContainerMA.delta;
                                    animWheelY.from = scrollContainer.targetItem.contentY;
                                    animWheelY.to = scrollContainer.targetItem.contentY + scrollContainerMA.delta;
                                    animWheelY.start();
                                }
                                else{
                                    //scrollContainer.targetItem.contentY = scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height + scrollContainer.targetItem.originY;
                                    animWheelY.from = scrollContainer.targetItem.contentY;
                                    animWheelY.to = scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height + scrollContainer.targetItem.originY;
                                    animWheelY.start();

                                }
                            }
                        }

                    }//vertical

                    else{//horiz

                        if(scrollContainer.targetItem.contentWidth > scrollContainer.targetItem.width){

                            if(wheel.angleDelta.y > 0){//left
                                if((scrollContainer.targetItem.contentX - scrollContainer.targetItem.originX - scrollContainerMA.delta) >= 0){
                                    //scrollContainer.targetItem.contentX -= scrollContainerMA.delta;
                                    animWheelX.from = scrollContainer.targetItem.contentX;
                                    animWheelX.to = scrollContainer.targetItem.contentX - scrollContainerMA.delta;
                                    animWheelX.start();

                                }
                                else{
                                    //scrollContainer.targetItem.contentX = scrollContainer.targetItem.originX;
                                    animWheelX.from = scrollContainer.targetItem.contentX;
                                    animWheelX.to = scrollContainer.targetItem.originX;
                                    animWheelX.start();
                                }
                            }
                            else {//right
                                if((scrollContainer.targetItem.contentX  - scrollContainer.targetItem.originX + scrollContainerMA.delta)< (scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width)){
                                    //scrollContainer.targetItem.contentX += scrollContainerMA.delta;
                                    animWheelX.from = scrollContainer.targetItem.contentX;
                                    animWheelX.to = scrollContainer.targetItem.contentX + scrollContainerMA.delta;
                                    animWheelX.start();
                                }
                                else{
                                    //scrollContainer.targetItem.contentX = scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width  + scrollContainer.targetItem.originY;
                                    animWheelX.from = scrollContainer.targetItem.contentX;
                                    animWheelX.to = scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width  + scrollContainer.targetItem.originY;
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
            onExited: {
                //                scrollMA.dragEnabled = false;
            }
            onPositionChanged: {
                //console.log("onPositionChanged", scrollMA.dragEnabled);
                if(scrollMA.dragEnabled)
                {
                    var newCoords = mapToItem(scrollContainer, mouse.x, mouse.y);

                    if(scrollContainer.vertical){
                        var deltaY = newCoords.y - scrollMA.coord.y;
                        if(scrollContainer.targetItem.contentY - scrollContainer.targetItem.originY + deltaY <= 0){
                            scrollContainer.targetItem.contentY = scrollContainer.targetItem.originY;
                        }
                        else if(scrollContainer.targetItem.contentY - scrollContainer.targetItem.originY + deltaY >= scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height){
                            scrollContainer.targetItem.contentY  = scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height + scrollContainer.targetItem.originY;
                        }
                        else {
                            scrollContainer.targetItem.contentY += deltaY/scrollContainer.koeff;
                        }
                    }
                    else{//horiz
                        var deltaX = newCoords.x - scrollMA.coord.x;
                        if(scrollContainer.targetItem.contentX - scrollContainer.targetItem.originX + deltaX <= 0){
                            scrollContainer.targetItem.contentX = scrollContainer.targetItem.originX;
                        }
                        else if(scrollContainer.targetItem.contentX - scrollContainer.targetItem.originX + deltaX >= scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width){
                            scrollContainer.targetItem.contentX = scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width + scrollContainer.targetItem.originX;
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
