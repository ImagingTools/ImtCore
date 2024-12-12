import QtQuick 2.12
import Acf 1.0

Rectangle{
    id: scrollContainer;

    height: targetItem && vertical ? targetItem.height : !decorator_ ? secondSize : secondSizeDecorator;
    width: targetItem && !vertical ? targetItem.width : !decorator_ ? secondSize : secondSizeDecorator;
    color: backgroundColor;
    radius: 2;

	visible: vertical ? targetItem.contentHeight - allowableGapVert > scrollContainer.height : targetItem.contentWidth - allowableGapHor > scrollContainer.width;

    property var targetItem: null;

    property bool vertical: true;

    property real minSize: 20;
    property real secondSize: 20;
    property int indicatorRadius: 2;
    property string backgroundColor: decorator_ ? "transparent" : Style.color_scrollBackground !== undefined ? Style.color_scrollBackground  : "#efefef";
    property string indicatorColor: decorator_ ? "transparent" : Style.color_scrollIndicator !== undefined ? Style.color_scrollIndicator : "lightgray";
    property string highlightColor: decorator_ ? "transparent" : Style.color_scrollHighlight !== undefined ? Style.color_scrollHighlight : "lightgray";

    property bool canFade: false;
    property bool isMoving: false;
    property int pauseDuration: 300;
    property int indicatorMargin: 0;
    property real minimum: 0.0000000001;

    property bool notUsed: !targetItem ? true : vertical ? targetItem.contentHeight <= targetItem.height:
                                                           targetItem.contentWidth <= targetItem.width;
    property bool hideNotUsed: true;


    property real koeff: vertical ? koeffVert : koeffHoriz;

    property real koeffVert: !targetItem ? 1 : !vertical ? 1 : Math.abs((targetItem.contentHeight - targetItem.height)) > minimum ? (targetItem.height - scrollIndicator.height)/(targetItem.contentHeight - targetItem.height): 1;
    property real koeffHoriz: !targetItem ? 1 : vertical ? 1 : Math.abs((targetItem.contentWidth - targetItem.width)) > minimum ? (targetItem.width - scrollIndicator.width)/(targetItem.contentWidth - targetItem.width): 1;

    property bool visibleState: !canFade ? 1 : isMoving ? 1 : (scrollContainerMA.containsMouse || scrollMA.containsMouse) ? 1 : 0;
    property bool inSideTarget: false;
    property bool alwaysVisible: false;
    property bool canDragOutOfBounds: false;

    property int targetContentHeight: !targetItem ? 0 : targetItem.contentHeight == undefined ? 0 : targetItem.contentHeight;
    property int targetContentWidth: !targetItem ? 0 : targetItem.contentWidth == undefined ? 0 : targetItem.contentWidth;

    /*for Qt Style decoration*/
    property Component decorator : Style.scrollBarDecorator ? Style.scrollBarDecorator : null
    property var decorator_ : null;

    property real scrollIndicatorY: scrollIndicator.y;
    property real scrollIndicatorX: scrollIndicator.x;

    property real secondSizeDecorator: !decorator_ ? 0 : vertical ? decorator_.background.width : decorator_.background.height;

	property real allowableGapHor: 0;
	property real allowableGapVert: 0;

    signal contentXSignal(real contentX);
    signal contentYSignal(real contentY);
    signal movingSignal();

    onDecoratorChanged: {
        if(decorator_){
            decorator_.destroy();
        }

        if(!decorator){
            return;
        }

        decorator_ = decorator.createObject(scrollContainer);
        decorator_.baseElement = scrollContainer;

        if(scrollContainer.vertical){
            decorator_.position = scrollIndicatorY/scrollContainer.height;
        }
        else {
            decorator_.position = scrollIndicatorX/scrollContainer.width;
        }

    }

    onScrollIndicatorYChanged: {
        if(decorator_ && vertical && scrollContainer.height !== 0){
            decorator_.position = scrollIndicatorY/scrollContainer.height;
        }
    }

    onScrollIndicatorXChanged: {
        if(decorator_ && !vertical && scrollContainer.width !== 0){
            decorator_.position = scrollIndicatorX/scrollContainer.width;
        }
    }

    function setContentPositionFunc(position_){
        if (scrollContainer.koeff == 0){
            return
        }

        if(scrollContainer.vertical){
            scrollContainer.targetItem.contentY =
            position_ * scrollContainer.height / scrollContainer.koeff + scrollContainer.targetItem.originY;
        }
        else {
            scrollContainer.targetItem.contentX =
            position_ * scrollContainer.width / scrollContainer.koeff + scrollContainer.targetItem.originX;
        }
    }

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
        if(scrollContainer.alwaysVisible){
            return;
        }
        if(targetItem){
            if(vertical){
				visible = targetItem.contentHeight - allowableGapVert > scrollContainer.height;
            }
            else {
				visible = targetItem.contentWidth - allowableGapHor > scrollContainer.width;
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
        onFinished: {
            scrollContainer.contentYSignal(scrollContainer.targetItem.contentY);
        }
    }

    NumberAnimation {
        id: animWheelX;

        target: scrollContainer.targetItem;
        property: "contentX";
        duration: 50;
        from: 0; to: 1;
        onFinished: {
            scrollContainer.contentXSignal(scrollContainer.targetItem.contentX);
        }
    }

    MouseArea{
        id:scrollContainerMA;
        anchors.fill: parent;
        visible: scrollContainer.visible;
        enabled: visible;
        hoverEnabled: enabled;
        property int delta: 40;

        onClicked: {
            if (scrollContainer.koeff == 0){
                return
            }

            var newCoords = mapToItem(scrollContainer, mouse.x, mouse.y);
            if(scrollContainer.vertical){
                scrollContainer.targetItem.contentY = (newCoords.y >= scrollIndicator.y + scrollIndicator.height) ?
                            (newCoords.y - scrollIndicator.height) / scrollContainer.koeff + scrollContainer.targetItem.originY : newCoords.y / scrollContainer.koeff + scrollContainer.targetItem.originY;

                scrollContainer.contentYSignal(scrollContainer.targetItem.contentY);
            }
            else{//horiz
                scrollContainer.targetItem.contentX = (newCoords.x >= scrollIndicator.x + scrollIndicator.width) ?
                            (newCoords.x - scrollIndicator.width) / scrollContainer.koeff + scrollContainer.targetItem.originX : newCoords.x / scrollContainer.koeff + scrollContainer.targetItem.originX;

                scrollContainer.contentXSignal(scrollContainer.targetItem.contentX);
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
                                animWheelY.from = scrollContainer.targetItem.contentY;
                                animWheelY.to = scrollContainer.targetItem.contentY - scrollContainerMA.delta;
                                animWheelY.start();
                            }
                            else {
                                animWheelY.from = scrollContainer.targetItem.contentY;
                                animWheelY.to = scrollContainer.targetItem.originY;
                                animWheelY.start();
                            }
                        }
                        else {//down
                            if((scrollContainer.targetItem.contentY - scrollContainer.targetItem.originY + scrollContainerMA.delta)< (scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height)){
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
                                animWheelX.from = scrollContainer.targetItem.contentX;
                                animWheelX.to = scrollContainer.targetItem.contentX - scrollContainerMA.delta;
                                animWheelX.start();
                            }
                            else{
                                animWheelX.from = scrollContainer.targetItem.contentX;
                                animWheelX.to = scrollContainer.targetItem.originX;
                                animWheelX.start();
                            }
                        }
                        else {//right
                            if((scrollContainer.targetItem.contentX  - scrollContainer.targetItem.originX + scrollContainerMA.delta)< (scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width)){
                                animWheelX.from = scrollContainer.targetItem.contentX;
                                animWheelX.to = scrollContainer.targetItem.contentX + scrollContainerMA.delta;
                                animWheelX.start();
                            }
                            else{
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
           scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width == 0 ? (scrollContainer.targetItem.contentX - scrollContainer.targetItem.originX):
                                                                                               (scrollContainer.targetItem.contentX - scrollContainer.targetItem.originX)*(scrollContainer.targetItem.width - scrollIndicator.width)/(scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width)

        y: !scrollContainer.vertical  ? (parent.height - height)/2:
            scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height == 0 ? (scrollContainer.targetItem.contentY - scrollContainer.targetItem.originY) :
                                                                                                (scrollContainer.targetItem.contentY - scrollContainer.targetItem.originY)*(scrollContainer.targetItem.height - scrollIndicator.height)/(scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height)

        width: scrollContainer.vertical  ? (parent.width - 2*scrollContainer.indicatorMargin) :
                                           scrollContainer.targetItem.contentWidth === 0 ? scrollContainer.minimum:
                                          (scrollContainer.targetItem.contentWidth <= scrollContainer.targetItem.width ? scrollContainer.targetItem.width :
                                                                                                                           (scrollContainer.targetItem.width / scrollContainer.targetItem.contentWidth * scrollContainer.targetItem.width)) < scrollContainer.minSize ?
                                                                                                                               scrollContainer.minSize :
                                                                                                                               (scrollContainer.targetItem.width / scrollContainer.targetItem.contentWidth * scrollContainer.targetItem.width);

        height: !scrollContainer.vertical  ? (parent.height - 2*scrollContainer.indicatorMargin) :
                                             scrollContainer.targetItem.contentHeight === 0 ? scrollContainer.minimum:
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
            scrollContainer.movingSignal();
        }

        onYChanged:{
            if(!scrollMA.dragEnabled && scrollContainer.vertical){
                scrollMA.coord = Qt.point(scrollIndicator.x,scrollIndicator.y);
            }
            if(y !== 0 && scrollContainer.vertical && scrollContainer.canFade){
                fadePause.stop();
                fadePause.start();
            }
            scrollContainer.movingSignal();
        }

        MouseArea{
            id:scrollMA;

            anchors.fill: parent;
            visible: scrollContainer.visible;
            enabled: visible;
            hoverEnabled: enabled;

            property bool dragEnabled: false;

            onVisibleChanged: {
				if(!visible && scrollMA){
					scrollMA.dragEnabled = false;
                }
            }

            property var coord: mapToItem(scrollContainer,0,0);

            onClicked: {}
            onWheel: {
                if(scrollContainer.inSideTarget){
                    wheel.accepted = false;
                }
                else {
                    if(scrollContainer.vertical){
                        if(scrollContainer.targetItem.contentHeight > scrollContainer.targetItem.height){
                            if(wheel.angleDelta.y > 0){//up
                                if((scrollContainer.targetItem.contentY - scrollContainer.targetItem.originY - scrollContainerMA.delta)>=0){
                                    animWheelY.from = scrollContainer.targetItem.contentY;
                                    animWheelY.to = scrollContainer.targetItem.contentY - scrollContainerMA.delta;
                                    animWheelY.start();
                                }
                                else {
                                    animWheelY.from = scrollContainer.targetItem.contentY;
                                    animWheelY.to = scrollContainer.targetItem.originY;
                                    animWheelY.start();
                                }
                            }
                            else {//down
                                if((scrollContainer.targetItem.contentY - scrollContainer.targetItem.originY + scrollContainerMA.delta)< (scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height)){
                                    animWheelY.from = scrollContainer.targetItem.contentY;
                                    animWheelY.to = scrollContainer.targetItem.contentY + scrollContainerMA.delta;
                                    animWheelY.start();
                                }
                                else{
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
                                    animWheelX.from = scrollContainer.targetItem.contentX;
                                    animWheelX.to = scrollContainer.targetItem.contentX - scrollContainerMA.delta;
                                    animWheelX.start();
                                }
                                else{
                                    animWheelX.from = scrollContainer.targetItem.contentX;
                                    animWheelX.to = scrollContainer.targetItem.originX;
                                    animWheelX.start();
                                }
                            }
                            else {//right
                                if((scrollContainer.targetItem.contentX  - scrollContainer.targetItem.originX + scrollContainerMA.delta)< (scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width)){
                                    animWheelX.from = scrollContainer.targetItem.contentX;
                                    animWheelX.to = scrollContainer.targetItem.contentX + scrollContainerMA.delta;
                                    animWheelX.start();
                                }
                                else{
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
            onExited: {}
            onPositionChanged: {
                if(scrollMA.dragEnabled){
                    var newCoords = mapToItem(scrollContainer, mouse.x, mouse.y);
                    if(scrollContainer.vertical){
                        var deltaY = newCoords.y - scrollMA.coord.y;
                        if(scrollContainer.targetItem.contentY - scrollContainer.targetItem.originY + deltaY/scrollContainer.koeff <= 0){
                            if(!scrollContainer.canDragOutOfBounds){
                                scrollContainer.targetItem.contentY = scrollContainer.targetItem.originY;
                            }
                            else {
                                scrollContainer.targetItem.contentY += deltaY/scrollContainer.koeff;
                            }
                        }
                        else if(scrollContainer.targetItem.contentY - scrollContainer.targetItem.originY + deltaY/scrollContainer.koeff >= scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height){
                            if(!scrollContainer.canDragOutOfBounds){
                                scrollContainer.targetItem.contentY  = scrollContainer.targetItem.contentHeight - scrollContainer.targetItem.height + scrollContainer.targetItem.originY;
                            }
                            else {
                                scrollContainer.targetItem.contentY += deltaY/scrollContainer.koeff;
                            }
                        }
                        else {
                            scrollContainer.targetItem.contentY += deltaY/scrollContainer.koeff;
                        }
                        scrollContainer.contentYSignal(scrollContainer.targetItem.contentY);
                    }
                    else{//horiz
                        var deltaX = newCoords.x - scrollMA.coord.x;
                        if(scrollContainer.targetItem.contentX - scrollContainer.targetItem.originX + deltaX/scrollContainer.koeff <= 0){
                            if(!scrollContainer.canDragOutOfBounds){
                                scrollContainer.targetItem.contentX = scrollContainer.targetItem.originX;
                            }
                            else {
                                scrollContainer.targetItem.contentX += deltaX/scrollContainer.koeff;
                            }
                        }
                        else if(scrollContainer.targetItem.contentX - scrollContainer.targetItem.originX + deltaX/scrollContainer.koeff >= scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width){
                            if(!scrollContainer.canDragOutOfBounds){
                                scrollContainer.targetItem.contentX = scrollContainer.targetItem.contentWidth - scrollContainer.targetItem.width + scrollContainer.targetItem.originX;
                            }
                            else {
                                scrollContainer.targetItem.contentX += deltaX/scrollContainer.koeff;
                            }
                        }
                        else {
                            scrollContainer.targetItem.contentX += deltaX/scrollContainer.koeff;
                        }
                        scrollContainer.contentXSignal(scrollContainer.targetItem.contentX);
                    }

                    scrollMA.coord = newCoords;
                }
            }
        }
    }
}
