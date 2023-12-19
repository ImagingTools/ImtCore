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
    property int edgeSize: 20;
    property Component content : emptyComp;
    property var steps: [];
    property int mainStep: 0;

    Component.onCompleted: {
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

        anchors.left: parent.left;
        anchors.leftMargin: - width + addToMargin;

        property real addToMargin: 0;

        Loader{
            id: loader;

            sourceComponent: drawer.content;
            onLoaded: {
                let width_ = loader.item.width;
                let height_ = loader.item.height;
                hiddenItem.width = width_;
                hiddenItem.height = height_;

                drawer.width = width_ + drawer.edgeSize;
                drawer.height = height_;
                drawer.mainStep = width_;

            }
        }

    }

    Rectangle{
        id: controlRec;

        anchors.left: edge == Qt.LeftEdge ? hiddenItem.right : undefined;
        anchors.right: edge == Qt.RightEdge ? hiddenItem.left : undefined;
        anchors.top: edge == Qt.TopEdge ? hiddenItem.bottom : undefined;
        anchors.bottom: edge == Qt.BottomEdge ? hiddenItem.top : undefined;

        height: parent.height;
        width: drawer.edgeSize;
        color: "green";

        MouseArea{
            anchors.fill: parent;

            property point startPoint;
            property real lastDeltaX;
            onPressed: {
                startPoint = mapToItem(drawer.parent, mouse.x, 0);
            }
            onReleased: {
                let position = mapToItem(drawer, mouse.x, 0)
                //               if(position.x.toFixed(1) == startPoint.x.toFixed(1)){
                //                   list.selectedIndex = model.index;
                //                   list.positionViewAtIndex(model.index, ListView.Center);
                //               }

                //               else if(lastDeltaX > 0){//after moving
                //                   if(listPreview.addToMargin > listPreview.selectedIndex * listPreview.delegateWidth){
                //                       list.selectedIndex++;
                //                       list.positionViewAtIndex(list.selectedIndex, ListView.Center);
                //                   }
                //               }
                //               else {
                //                   if(listPreview.addToMargin < listPreview.selectedIndex * listPreview.delegateWidth){
                //                       list.selectedIndex--;
                //                       list.positionViewAtIndex(list.selectedIndex, ListView.Center);
                //                   }
                //               }
            }
            onPositionChanged: {
                let mousePos = mapToItem(drawer.parent, mouse.x, 0)
                let delta = startPoint.x - mousePos.x;
                lastDeltaX = delta;
                console.log(delta); //влево +, вправо -
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
                startPoint = mousePos;
            }
        }

    }


    
}

