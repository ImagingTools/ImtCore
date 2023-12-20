import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0

Item{
    id: drawer;

    anchors.left: drawer.edge == Qt.LeftEdge ? parent.left : undefined;
    anchors.right: drawer.edge == Qt.RightEdge ? parent.right : undefined;
    anchors.top: drawer.edge == Qt.TopEdge ? parent.top : undefined;
    anchors.bottom: drawer.edge == Qt.BottomEdge ? parent.bottom : undefined;

    clip: true;

    property int edge: Qt.LeftEdge;//Qt.TopEdge, Qt.BottomEdge, Qt.RightEdge
    property Component content : emptyComp;
    property var steps: [];
    property int stepsLength: steps.length;
    property int mainStep: 0;
    property int currentStep: 0;
    property int currentStepIndex: 0;
    property bool isOpen: hiddenItem.addToMargin > 0;

    property int edgeSize: 12;
    property string controlColor: "#ffe4c4";

    Component.onCompleted: {
    }

    onStepsLengthChanged: {
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

    MouseArea{
        id: controlMA;

        anchors.left: drawer.edge == Qt.RightEdge ? controlRec.left : undefined;
        anchors.right: drawer.edge == Qt.LeftEdge ? controlRec.right : undefined;
        anchors.top: drawer.edge == Qt.BottomEdge ? controlRec.top : undefined;
        anchors.bottom: drawer.edge == Qt.TopEdge ? controlRec.bottom : undefined;

        width: (drawer.edge == Qt.LeftEdge || drawer.edge == Qt.RightEdge) ? (controlRec.width + hiddenItem.width) : parent.width;
        height: (drawer.edge == Qt.LeftEdge || drawer.edge == Qt.RightEdge) ? parent.height : (controlRec.height + hiddenItem.height);

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
            let isClick = position.x.toFixed(1) == startPointConst.x.toFixed(1) && position.y.toFixed(1) == startPointConst.y.toFixed(1)
            let deltaX;
            let deltaY;

            let minVal = 0;
            let maxVal = drawer.mainStep;

            let positionInDrawer = mapToItem(drawer, mouse.x, mouse.y)
            let clickVal;
            if(drawer.edge == Qt.LeftEdge){
                clickVal = positionInDrawer.x;
            }
            else if(drawer.edge == Qt.RightEdge){
                clickVal = drawer.width - positionInDrawer.x;
            }
            else if(drawer.edge == Qt.TopEdge){
                clickVal = positionInDrawer.y;
            }
            else if(drawer.edge == Qt.BottomEdge){
                clickVal = drawer.height - positionInDrawer.y;
            }
            //console.log("clickVal - hiddenItem.addToMargin:: ", clickVal - hiddenItem.addToMargin)


            if(isClick && (clickVal - hiddenItem.addToMargin > 0) && (hiddenItem.addToMargin.toFixed() == drawer.mainStep.toFixed())){
                animMargin.from = hiddenItem.addToMargin;
                animMargin.to = 0;
                animMargin.start();
                return;
            }

            //steps
            if(drawer.steps.length){
                //console.log(positionInDrawer.x, positionInDrawer.y)
                let releasedVal;

                if(drawer.edge == Qt.LeftEdge){
                    releasedVal = hiddenItem.addToMargin + lastDeltaX;
                }
                else if(drawer.edge == Qt.RightEdge){
                    releasedVal = hiddenItem.addToMargin + lastDeltaX;
                }
                else if(drawer.edge == Qt.TopEdge){
                    releasedVal = hiddenItem.addToMargin + lastDeltaY;
                }
                else if(drawer.edge == Qt.BottomEdge){
                    releasedVal = hiddenItem.addToMargin + lastDeltaY;
                }
                if(releasedVal < 0){
                    releasedVal = 0;
                }
                if(releasedVal > drawer.mainStep){
                    releasedVal = drawer.mainStep;
                }
                //console.log("releasedVal:: ", releasedVal)

                if(isClick){
                    //console.log("isClick", isClick)
                    if(clickVal - hiddenItem.addToMargin > 0){
                        if(hiddenItem.addToMargin.toFixed() == 0){
                            minVal = 0;
                            maxVal = drawer.steps[0];
                        }
                        else if(hiddenItem.addToMargin.toFixed() == drawer.mainStep.toFixed()){
                            minVal = drawer.steps[drawer.steps.length - 1];
                            maxVal = minVal;
                        }
                        else {
                            let index_= 0
                            for(let i = 0; i < (drawer.steps.length); i++){
                                let currVal = drawer.steps[i];
                                if(hiddenItem.addToMargin.toFixed() == currVal.toFixed()){
                                    index_ = i;
                                    break;
                                }
                            }
                            minVal = drawer.steps[index_];
                            maxVal = drawer.steps[index_ + 1];
                        }
                    }
                    else {//click into body field
                        return;
                    }


                }

                //not click
                else if(releasedVal >= 0 && releasedVal <= drawer.mainStep){
                    for(let i = 0; i < (drawer.steps.length); i++){
                        let ok;
                        if(i == 0){
                            ok = releasedVal >= 0 &&
                                releasedVal < drawer.steps[i];
                            if(ok){
                                minVal = 0;
                                maxVal = drawer.steps[i];
                                break
                            }
                        }
                        else {
                            ok = releasedVal >= drawer.steps[i-1] &&
                                releasedVal < drawer.steps[i];
                            if(ok){
                                minVal = drawer.steps[i-1];
                                maxVal = drawer.steps[i];
                                break;
                            }
                        }

                    }//for
                }
                //console.log("VALUES:: ", minVal, maxVal);
            }//steps

            //on click
            if(isClick){
                deltaX = drawer.edge == Qt.LeftEdge ? -1 : 1;
                deltaY = drawer.edge == Qt.TopEdge ? -1 : 1;
            }
            //not on click
            else {
                deltaX = lastDeltaX;
                deltaY = lastDeltaY;
            }

            if(drawer.edge == Qt.LeftEdge){
                if(deltaX < 0){
                    animMargin.from = hiddenItem.addToMargin;
                    animMargin.to = maxVal;
                    animMargin.start();
                }
                else {
                    animMargin.from = hiddenItem.addToMargin;
                    animMargin.to = minVal;
                    animMargin.start();
                }
            }
            else if(drawer.edge == Qt.RightEdge){
                if(deltaX < 0){
                    animMargin.from = hiddenItem.addToMargin;
                    animMargin.to = minVal;
                    animMargin.start();

                }
                else {
                    animMargin.from = hiddenItem.addToMargin;
                    animMargin.to = maxVal;
                    animMargin.start();
                }

            }
            else if(drawer.edge == Qt.TopEdge){
                if(deltaY < 0){
                    animMargin.from = hiddenItem.addToMargin;
                    animMargin.to = maxVal;
                    animMargin.start();
                }
                else {
                    animMargin.from = hiddenItem.addToMargin;
                    animMargin.to = minVal;
                    animMargin.start();
                }
            }
            else if(drawer.edge == Qt.BottomEdge){
                if(deltaY < 0){
                    animMargin.from = hiddenItem.addToMargin;
                    animMargin.to = minVal;
                    animMargin.start();

                }
                else {
                    animMargin.from = hiddenItem.addToMargin;
                    animMargin.to = maxVal;
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
    }//MouseArea

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

            property string imageName: hiddenItem.addToMargin.toFixed() == drawer.mainStep ? "Icons/Close" : drawer.isOpen ? "Icons/Delete" : "Icons/Up";
            source: "../../../" + Style.getIconPath(imageName, Icon.State.On, Icon.Mode.Normal);

            rotation: drawer.isOpen &&  (drawer.edge == Qt.LeftEdge || drawer.edge == Qt.RightEdge) ? 90 :
                                      drawer.isOpen ? 0 :  drawer.edge == Qt.LeftEdge ? 90 :  drawer.edge == Qt.RightEdge ? 270 :
                                                                                 drawer.edge == Qt.TopEdge ? 180 : 0;


        }



    }//controlRec



    NumberAnimation {
        id: animMargin;

        target: hiddenItem;
        property: "addToMargin";
        duration: 200;
    }

    
}

