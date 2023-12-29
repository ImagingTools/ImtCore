import QtQuick 2.12

import Acf 1.0
//import imtgui 1.0
import imtqml 1.0
import imtcontrols 1.0


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



    /********************************TableTreeView TEST************************/

    property int size_: 100;

    TreeItemModel{
        id: testInsertModelLevel0;

        property bool compl: false;
        Component.onCompleted: {//TEST

            let date = new Date();
            let val = date.valueOf();
            let index;

            for(let i = 0; i < 10; i++){
                index = testInsertModelLevel0.InsertNewItem();
                testInsertModelLevel0.SetData("Id", String(val + index), index);
                testInsertModelLevel0.SetData("Name","ул. Ленина", index);
                testInsertModelLevel0.SetData("TypeId", "Node", index);
                testInsertModelLevel0.SetData("HasChildren", true, index);

                testInsertModelLevel0.SetData("Date", "26.12.23", index);
                testInsertModelLevel0.SetData("Size",  String(testPage.size_ + index +1), index);

                testPage.size_++


                index = testInsertModelLevel0.InsertNewItem();
                testInsertModelLevel0.SetData("Id", String(val + index), index);
                testInsertModelLevel0.SetData("Name","ул. Маркса", index);
                testInsertModelLevel0.SetData("TypeId", "Node", index);
                testInsertModelLevel0.SetData("HasChildren", true, index);

                testInsertModelLevel0.SetData("Date", "26.12.23", index);
                testInsertModelLevel0.SetData("Size",  String(testPage.size_ + index +1), index);

                testPage.size_++


                index = testInsertModelLevel0.InsertNewItem();
                testInsertModelLevel0.SetData("Id", String(val + index), index);
                testInsertModelLevel0.SetData("Name","ул. Менделеева", index);
                testInsertModelLevel0.SetData("TypeId", "Node", index);
                testInsertModelLevel0.SetData("HasChildren", true, index);

                testInsertModelLevel0.SetData("Date", "26.12.23", index);
                testInsertModelLevel0.SetData("Size",  String(testPage.size_ + index +1), index);

                testPage.size_++


                index = testInsertModelLevel0.InsertNewItem();
                testInsertModelLevel0.SetData("Id", String(val + index), index);
                testInsertModelLevel0.SetData("Name","ул. Королёва", index);
                testInsertModelLevel0.SetData("TypeId", "Node", index);
                testInsertModelLevel0.SetData("HasChildren", true, index);

                testInsertModelLevel0.SetData("Date", "26.12.23", index);
                testInsertModelLevel0.SetData("Size",  String(testPage.size_ + index +1), index);

                testPage.size_++


                index = testInsertModelLevel0.InsertNewItem();
                testInsertModelLevel0.SetData("Id", String(val + index), index);
                testInsertModelLevel0.SetData("Name","ул. Гагарина", index);
                testInsertModelLevel0.SetData("TypeId", "Node", index);
                testInsertModelLevel0.SetData("HasChildren", false, index);

                testInsertModelLevel0.SetData("Date", "26.12.23", index);
                testInsertModelLevel0.SetData("Size",  String(testPage.size_ + index +1), index);

                testPage.size_++


                index = testInsertModelLevel0.InsertNewItem();
                testInsertModelLevel0.SetData("Id", String(val + index), index);
                testInsertModelLevel0.SetData("Name","ул. Мира", index);
                testInsertModelLevel0.SetData("TypeId", "Doc", index);
                testInsertModelLevel0.SetData("HasChildren", false, index);

                testInsertModelLevel0.SetData("Date", "26.12.23", index);
                testInsertModelLevel0.SetData("Size",  String(testPage.size_ + index +1), index);

                testPage.size_++

            }

            testInsertModelLevel0.compl = true;
        }
    }

    TreeItemModel{
        id: testInsertModel;

        property bool compl: false;
        Component.onCompleted: {//TEST

            let date = new Date();
            let val = date.valueOf();
            let index;

            index = testInsertModel.InsertNewItem();
            testInsertModel.SetData("Name", "дом 1" , index);
            testInsertModel.SetData("TypeId", "Node", index);
            testInsertModel.SetData("HasChildren", true, index);

            testInsertModel.SetData("Date", "26.12.23", index);
            testInsertModel.SetData("Size",  String(testPage.size_ + index +1), index);

            testPage.size_++


            index = testInsertModel.InsertNewItem();
            testInsertModel.SetData("Name","дом 2" , index);
            testInsertModel.SetData("TypeId", "Node", index);
            testInsertModel.SetData("HasChildren", true, index);

            testInsertModel.SetData("Date", "26.12.23", index);
            testInsertModel.SetData("Size",  String(testPage.size_ + index +1), index);

            testPage.size_++


            index = testInsertModel.InsertNewItem();
            testInsertModel.SetData("Name", "дом 3" , index);
            testInsertModel.SetData("TypeId", "Doc", index);
            testInsertModel.SetData("HasChildren", false, index);

            testInsertModel.SetData("Date", "26.12.23", index);
            testInsertModel.SetData("Size",  String(testPage.size_ + index +1), index);

            testPage.size_++
        }
    }



    TreeItemModel{
        id: tableHeaderModel;

        property bool compl: false;
        Component.onCompleted: {
            let index = tableHeaderModel.InsertNewItem();
            tableHeaderModel.SetData("Id", "Name", index);
            tableHeaderModel.SetData("Name", qsTr("Name"), index);

            index = tableHeaderModel.InsertNewItem();
            tableHeaderModel.SetData("Id", "Date", index);
            tableHeaderModel.SetData("Name", qsTr("Date"), index);

            index = tableHeaderModel.InsertNewItem();
            tableHeaderModel.SetData("Id", "Size", index);
            tableHeaderModel.SetData("Name", qsTr("Size"), index);


            tableHeaderModel.compl = true;
            tableTreeView.headers = tableHeaderModel;
        }

    }

    Rectangle{
        id: tableTreeViewContainer;

        anchors.top: parent.top;
        anchors.topMargin: 50;
        anchors.horizontalCenter: parent.horizontalCenter;

        width: parent.width - 40;
        height: parent.height - 2* anchors.topMargin;

        TableTreeView{
            id: tableTreeView;

            width: parent.width;
            height: parent.height;

            property bool ready : tableTreeView.compl && tableHeaderModel.compl && testInsertModelLevel0.compl;
            onReadyChanged: {
                if(ready){
                    tableTreeView.insertTree(-1, testInsertModelLevel0);//!!!
                }
            }

            onRequestSignal: {
                tableTreeView.insertTree(index, testInsertModel);
            }

        }//tableTreeView
    }



    TableTreeDecorator{
        id: tableDec;
    }

    TreeItemModel{
        id: decorModel;

        property bool compl: false;
        property bool complCompl: compl && tableDec.compl && tableHeaderModel.compl;

        Component.onCompleted: {decorModel.compl = true;}

        onComplComplChanged: {
            if(decorModel.complCompl){
                var headers = tableDec.GetTreeItemModel("Headers");
                //headers.SetData("CellRadius",2);
                tableDec.setDefault(decorModel,tableHeaderModel.GetItemsCount());

                headers = decorModel.GetTreeItemModel("Headers");//
                var cells = decorModel.GetTreeItemModel("Cells");//
                var cellWidth = decorModel.GetTreeItemModel("CellWidth");//


                //cellWidth.SetData("Width",200,1);
                //cellWidth.SetData("Width",200,2);
                cellWidth.SetData("MinWidth",200,0);
                //cellWidth.SetData("MinWidth",100,1);
                //cellWidth.SetData("MinWidth",100,2);


                //cells.SetData("TextPosition",Text.AlignLeft,0);
                //cells.SetData("TextPosition",Text.AlignLeft,1);
                //cells.SetData("TextPosition",Text.AlignLeft,2);

                //headers.SetData("LeftTopRound",true,0)
                //headers.SetData("RightTopRound",true,tableHeaderModel.GetItemsCount()-1)

                tableTreeView.tableDecorator = decorModel;

            }
        }
    }



}
