import QtQuick 2.12
import Acf 1.0;
import imtqml 1.0
import imtcontrols 1.0

Rectangle{
    id:scrollContainer;

    height: targetItem && vertical ? targetItem.height : secondSize;
    width: vertical ? secondSize : targetItem.width;
    color: backgroundColor;
    radius: 4;

    visible: vertical ? targetItem.contentHeight > scrollContainer.height : targetItem.contentWidth > scrollContainer.width;

    property var targetItem: null;

    property bool vertical: true;

    property real minSize: 20;
    property real secondSize: 20;
    property int indicatorRadius: 4;
    property string backgroundColor: Style.color_scrollBackground !== undefined ? Style.color_scrollBackground  : "#efefef";
    property string indicatorColor: Style.color_scrollIndicator !== undefined ? Style.color_scrollIndicator : "lightgray";
    property string highlightColor: Style.color_scrollHighlight !== undefined ? Style.color_scrollHighlight : "lightgray";

    property bool canFade: false;
    property bool isMoving: false;
    property int pauseDuration: 300;
    property int indicatorMargin: 0;

    property bool notUsed: !targetItem ? true : vertical ? targetItem.contentHeight <= targetItem.height:
                                                           targetItem.contentWidth <= targetItem.width;


    property bool visibleState: !canFade ? 1 : isMoving ? 1 : 0;

    property int targetContentHeight: targetItem.contentHeight;
    property int targetContentWidth: targetItem.contentWidth;

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
        if(scrollContainer.visibleState){
            animTo.start();
        }
        else {
            animFrom.start();
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


    Rectangle{
        id: scrollIndicator;

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
        opacity: 0.6;

        onXChanged:{
            if(x !== 0 && !scrollContainer.vertical && scrollContainer.canFade){
                fadePause.stop();
                fadePause.start();
            }
        }

        onYChanged:{
            if(y !== 0 && scrollContainer.vertical && scrollContainer.canFade){
                fadePause.stop();
                fadePause.start();
            }
        }

    }
}

