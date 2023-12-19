import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0

Item{
    id: drawer;

    anchors.left: edge == Qt.LeftEdge ? parent.left : undefined;
    anchors.right: edge == Qt.RightEdge ? parent.right : undefined;
    anchors.top: edge == Qt.TopEdge ? parent.top : undefined;
    anchors.bottom: edge == Qt.BottomEdge ? parent.bottom : undefined;

    clip: true;

    property int edge: Qt.LeftEdge;//Qt.TopEdge, Qt.BottomEdge, Qt.RightEdge
    property Component content : emptyComp;
    property var steps: [];
    property int mainStep: 0;
    property int currentStep: 0;
    property int currentStepIndex: 0;
    property bool isOpen: hiddenItem.addToMargin > 0;

    property int edgeSize: 12;
    property string controlColor: "#ffe4c4";

    Component.onCompleted: {
    }

    onStepsChanged: {
        if(steps.length){
            mainStep = steps[steps.length - 1];
        }
    }

    function addStep(step){
        steps.push(step);
    }

    Component{
        id: emptyComp;
        Item{}
    }

    Item{
        id: hiddenItem;

        anchors.left: drawer.edge == Qt.LeftEdge ? parent.left : undefined;
        anchors.right: drawer.edge == Qt.RightEdge ? parent.right : undefined;
        anchors.top: drawer.edge == Qt.TopEdge ? parent.top : undefined;
        anchors.bottom: drawer.edge == Qt.BottomEdge ? parent.bottom : undefined;

        anchors.leftMargin: drawer.edge == Qt.LeftEdge ? - width + addToMargin : 0;
        anchors.rightMargin: drawer.edge == Qt.RightEdge ? - width + addToMargin : 0;
        anchors.topMargin: drawer.edge == Qt.TopEdge ? - height + addToMargin : 0;
        anchors.bottomMargin: drawer.edge == Qt.BottomEdge ? - height + addToMargin : 0;

        property real addToMargin: 0;

        Loader{
            id: loader;

            sourceComponent: drawer.content;
            onLoaded: {
                let width_ = loader.item.width;
                let height_ = loader.item.height;

                hiddenItem.width = width_;
                hiddenItem.height = height_;

                if(drawer.edge == Qt.LeftEdge || drawer.edge == Qt.RightEdge){
                    drawer.width = width_ + drawer.edgeSize;
                    drawer.height = height_;
                    if(!drawer.steps.length){
                        drawer.mainStep = width_;
                    }
                }
                else if(drawer.edge == Qt.TopEdge || drawer.edge == Qt.BottomEdge){
                    drawer.width = width_;
                    drawer.height = height_ + drawer.edgeSize;
                    if(!drawer.steps.length){
                        drawer.mainStep = height_;
                    }
                }

            }
        }

    }

    Rectangle{
        id: controlRec;

        anchors.left: drawer.edge == Qt.LeftEdge ? hiddenItem.right : undefined;
        anchors.right: drawer.edge == Qt.RightEdge ? hiddenItem.left : undefined;
        anchors.top: drawer.edge == Qt.TopEdge ? hiddenItem.bottom : undefined;
        anchors.bottom: drawer.edge == Qt.BottomEdge ? hiddenItem.top : undefined;

        height: (drawer.edge == Qt.LeftEdge || drawer.edge == Qt.RightEdge) ? parent.height : drawer.edgeSize;
        width: (drawer.edge == Qt.LeftEdge || drawer.edge == Qt.RightEdge) ? drawer.edgeSize : parent.width;


        color: drawer.controlColor;
        radius: 4;

        Rectangle{
            x: drawer.edge == Qt.LeftEdge ? 0 : drawer.edge == Qt.RightEdge ? parent.width - width : drawer.edge == Qt.TopEdge ? 0 : drawer.edge == Qt.BottomEdge ? 0 : 0;
            y: drawer.edge == Qt.LeftEdge ? 0 : drawer.edge == Qt.RightEdge ? 0 : drawer.edge == Qt.TopEdge ? 0 : drawer.edge == Qt.BottomEdge ? parent.height - height : 0;
            width: drawer.edgeSize/2;
            height: width;
            color: parent.color;
        }

        Rectangle{
            x: drawer.edge == Qt.LeftEdge ? 0 : drawer.edge == Qt.RightEdge ? parent.width - width : drawer.edge == Qt.TopEdge ? parent.width - width : drawer.edge == Qt.BottomEdge ? parent.width - width : 0;
            y: drawer.edge == Qt.LeftEdge ? parent.height - height : drawer.edge == Qt.RightEdge ? parent.height - height : drawer.edge == Qt.TopEdge ? 0 : drawer.edge == Qt.BottomEdge ? parent.height - height : 0;

            width: drawer.edgeSize/2;
            height: width;
            color: parent.color;
        }

        Image{
            id: icon;

            anchors.centerIn: parent;
            width: drawer.edgeSize - 4;
            height: width;
            sourceSize.width: width;
            sourceSize.height: height;

            property string imageName: drawer.isOpen ? "Icons/Delete" : "Icons/Up";
            source: "../../../" + Style.getIconPath(imageName, Icon.State.On, Icon.Mode.Normal);

            rotation: drawer.isOpen &&  (drawer.edge == Qt.LeftEdge || drawer.edge == Qt.RightEdge) ? 90 :
                                      drawer.isOpen ? 0 :  drawer.edge == Qt.LeftEdge ? 90 :  drawer.edge == Qt.RightEdge ? 270 :
                                                                                 drawer.edge == Qt.TopEdge ? 180 : 0;


        }

        MouseArea{
            anchors.fill: parent;

            property point startPoint;
            property point startPointConst;
            property int lastDeltaX: 0;
            property int lastDeltaY: 0;
            onPressed: {
                startPoint = mapToItem(drawer.parent, mouse.x, mouse.y);
                startPointConst = mapToItem(drawer.parent, mouse.x, mouse.y);
            }
            onReleased: {
                let position = mapToItem(drawer.parent, mouse.x, mouse.y)
                let deltaX;
                let deltaY;

                if(position.x.toFixed(1) == startPointConst.x.toFixed(1) && position.y.toFixed(1) == startPointConst.y.toFixed(1)){
                    deltaX = drawer.edge == Qt.LeftEdge ? -1 : 1;
                    deltaY = drawer.edge == Qt.TopEdge ? -1 : 1;
                }
                else {
                    deltaX = lastDeltaX;
                    deltaY = lastDeltaY;
                }

                if(drawer.edge == Qt.LeftEdge){
                    if(deltaX < 0){
                        animMargin.from = hiddenItem.addToMargin;
                        animMargin.to = drawer.mainStep;
                        animMargin.start();
                    }
                    else {
                        animMargin.from = hiddenItem.addToMargin;
                        animMargin.to = 0;
                        animMargin.start();
                    }
                }
                else if(drawer.edge == Qt.RightEdge){
                    if(deltaX < 0){
                        animMargin.from = hiddenItem.addToMargin;
                        animMargin.to = 0;
                        animMargin.start();

                    }
                    else {
                        animMargin.from = hiddenItem.addToMargin;
                        animMargin.to = drawer.mainStep;
                        animMargin.start();
                    }

                }
                else if(drawer.edge == Qt.TopEdge){
                    if(deltaY < 0){
                        animMargin.from = hiddenItem.addToMargin;
                        animMargin.to = drawer.mainStep;
                        animMargin.start();
                    }
                    else {
                        animMargin.from = hiddenItem.addToMargin;
                        animMargin.to = 0;
                        animMargin.start();
                    }
                }
                else if(drawer.edge == Qt.BottomEdge){
                    if(deltaY < 0){
                        animMargin.from = hiddenItem.addToMargin;
                        animMargin.to = 0;
                        animMargin.start();

                    }
                    else {
                        animMargin.from = hiddenItem.addToMargin;
                        animMargin.to = drawer.mainStep;
                        animMargin.start();
                    }
                }

            }
            onPositionChanged: {
                let mousePos = mapToItem(drawer.parent, mouse.x, mouse.y)

                if(drawer.edge == Qt.LeftEdge){
                    let delta = startPoint.x - mousePos.x;
                    //console.log(delta); //влево +, вправо -
                    if(delta >= 0){
                        let ok  = hiddenItem.addToMargin - delta >= 0;
                        if(ok){
                            hiddenItem.addToMargin -= delta;
                        }
                    }
                    else {
                        let ok  = hiddenItem.addToMargin -delta <= drawer.mainStep;
                        if(ok){
                            hiddenItem.addToMargin -= delta;
                        }

                    }
                }//LeftEdge

                else if(drawer.edge == Qt.RightEdge){
                    let delta = startPoint.x - mousePos.x;
                    //console.log(delta); //влево +, вправо -
                    if(delta >= 0){
                        let ok  = hiddenItem.addToMargin +delta <= drawer.mainStep;
                        if(ok){
                            hiddenItem.addToMargin += delta;
                        }
                    }
                    else {
                        let ok  = hiddenItem.addToMargin + delta >= 0;
                        if(ok){
                            hiddenItem.addToMargin += delta;
                        }

                    }
                }//RightEdge

                else if(drawer.edge == Qt.TopEdge){
                    let delta = startPoint.y - mousePos.y;
                    //console.log(delta);
                    if(delta >= 0){
                        let ok  = hiddenItem.addToMargin - delta >= 0;
                        if(ok){
                            hiddenItem.addToMargin -= delta;
                        }
                    }
                    else {
                        let ok  = hiddenItem.addToMargin -delta <= drawer.mainStep;
                        if(ok){
                            hiddenItem.addToMargin -= delta;
                        }

                    }
                }//TopEdge

                else if(drawer.edge == Qt.BottomEdge){
                    let delta = startPoint.y - mousePos.y;
                    //console.log(delta);
                    if(delta >= 0){
                        let ok  = hiddenItem.addToMargin +delta <= drawer.mainStep;
                        if(ok){
                            hiddenItem.addToMargin += delta;
                        }
                    }
                    else {
                        let ok  = hiddenItem.addToMargin + delta >= 0;
                        if(ok){
                            hiddenItem.addToMargin += delta;
                        }

                    }
                }//BottomEdge

                lastDeltaX = startPoint.x - mousePos.x;
                lastDeltaY = startPoint.y - mousePos.y;
                startPoint = mousePos;
            }
        }

    }


    NumberAnimation {
        id: animMargin;

        target: hiddenItem;
        property: "addToMargin";
        duration: 200;
    }

    
}

