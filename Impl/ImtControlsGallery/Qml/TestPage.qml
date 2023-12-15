import QtQuick 2.12

import Acf 1.0
import imtgui 1.0
import imtqml 1.0


Rectangle {
    id: testPage;

    anchors.fill: parent;

//    Rectangle{
//        id: testRec;

//        anchors.horizontalCenter: parent.horizontalCenter;
//        anchors.top: parent.top;
//        anchors.topMargin: 50;

//        width: 150;
//        height: 150;
//        color: "red";

//        Rectangle{
//            anchors.left: parent.left;
//            //anchors.top: parent.top;

//            width: 50;
//            height: 50;
//            color: "green";
//        }

//        Rectangle{
//            anchors.right: parent.right;
//            anchors.top: parent.top;

//            width: 50;
//            height: 50;
//            color: "yellow";
//        }

//    }

//    Rectangle{
//        id: rowContainer;

//        anchors.horizontalCenter: parent.horizontalCenter;
//        anchors.top: testRec.bottom;
//        anchors.topMargin: 50;

//        width: row.width + 8;
//        height: 100;

//        border.color: "lightgreen";
//        Row{
//            id:row;

//            anchors.centerIn: parent;
//            height: parent.height - 8;

//            spacing: 40;

//            Rectangle{

//                anchors.verticalCenter: parent.verticalCenter;
//                width: 50;
//                height: 50;
//                color: "orange";
//            }

//            Rectangle{

//                anchors.verticalCenter: parent.verticalCenter;
//                width: 50;
//                height: 50;
//                color: "violet";
//            }
//        }
//    }

//    ListModel{
//        id: listModel;
//        ListElement {text: "delegate 1"; color: "red";}
//        ListElement {text: "delegate 2"; color: "green";}
//        ListElement {text: "delegate 3"; color: "blue";}

//    }

//    ListView{
//        id: listView;

//        anchors.verticalCenter: parent.verticalCenter;
//        anchors.right: parent.right;
//        anchors.rightMargin: 100;

//        width: 150;
//        height: 100;
//        clip: true;
//        boundsBehavior: Flickable.StopAtBounds;
//        spacing: 30;
//        model: listModel;
//        delegate:
//            Rectangle{
//                width: listView.width;
//                height: 40;
//                property alias text: name.text;
//                property string modelColor: model.color;
//                color: model.color;
//                Text {
//                    id: name;

//                    anchors.centerIn: parent;
//                    text: model.text;
//                }
//            }
//        }


//    ////////////////////Button//////////////////////////
//    Rectangle{
//        id: button;

//        anchors.horizontalCenter: parent.horizontalCenter;
//        anchors.bottom: parent.bottom;
//        anchors.bottomMargin: 50;

//        width: 100;
//        height: 40;
//        radius: 8;
//        color: "lightgreen";
//        border.color: "lightgrey";
//        Text {
//            anchors.centerIn: parent;
//            font.pixelSize: 18;
//            text: qsTr("Button");
//        }
//        MouseArea{
//            anchors.fill: parent;
//            hoverEnabled: true;
//            cursorShape: Qt.PointingHandCursor;
//            onClicked: {
//                //
////                testRec.children[0].width = 100;
////                testRec.children[0].y = 50;
//                //

//                //
////                testRec.children = [];
////                recComp.createObject(testRec);
//                //

//                //
////                var element = rowComp.createObject();
////                row.children.push(element);
////                console.log(row, element.parent)
////                row.children.push(brown);
//                //

//                //
////                var element = rowComp.createObject();
////                row.children.splice(1,0,element);
//                //

//                //
////                var element = rowComp.createObject();
////                var list = []
////                list.push(element);
////                list.push(brown);
////                row.children = list;
//                //

//                //
////                var first = row.children[0];
////                row.children[0] = brown;
////                console.log(first.parent)
//                //

//                //
////                row.children = []
//                //

//                //
////                row.children[0].width = 200;
//                //

//                //
//                let item = listView.children[0].children[1];
//                console.log(item.text /*alias*/,
//                            item.modelColor /*string*/,
//                            item.color /*color*/);
//                //

//            }
//            onPressed: {
//                parent.scale = 0.95;
//            }
//            onReleased: {
//                parent.scale = 1;
//            }
//        }
//    }
//    ////////////////////Button//////////////////////////



//    Component{
//        id: recComp;

//        Rectangle{
//            anchors.right: parent.right;
//            anchors.bottom: parent.bottom;

//            width: 50;
//            height: 50;
//            color: "blue";
//        }

//    }

//    Component{
//        id: rowComp;

//        Rectangle{
//            anchors.verticalCenter: parent.verticalCenter;

//            width: 50;
//            height: 50;
//            color: "blue";
//        }

//    }

//    Rectangle{
//        id: brown;

//        anchors.verticalCenter: parent.verticalCenter;

//        width: 50;
//        height: 50;
//        color: "brown";
//    }

//    //background change test
//    Rectangle{
//        id: backgroundTest;

//        anchors.centerIn: parent;

//        width: 100;
//        height: 100;
//        border.color: "brown";

//        property Item background: Rectangle{
//            anchors.fill: parent;
//            color: "orange";
//        };

//        function setBackgroud(){
//            if(!backgroundItem.children.length){
//                backgroundItem.children = [];
//                backgroundItem.children.push(background);
//            }
//        }
//        Component.onCompleted: {
//            setBackgroud();
//        }

//        onBackgroundChanged: {
//            setBackgroud();
//        }

//        Item{
//            id: backgroundItem;

//            anchors.centerIn: parent;

//            width: parent.width - 8;
//            height: parent.height - 8;
//        }
//    }

//TreeViewGql{
//    anchors.centerIn: parent;

//    width: parent.width - 200;
//    height: parent.height - 200;
//}























}
