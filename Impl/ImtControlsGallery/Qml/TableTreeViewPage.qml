import QtQuick 2.12

import Acf 1.0
import imtcontrols 1.0


Rectangle {
    id: tableTreeViewPage;

    anchors.fill: parent;

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
                testInsertModelLevel0.SetData("Size",  String(tableTreeViewPage.size_ + index +1), index);

                tableTreeViewPage.size_++


                index = testInsertModelLevel0.InsertNewItem();
                testInsertModelLevel0.SetData("Id", String(val + index), index);
                testInsertModelLevel0.SetData("Name","ул. Маркса", index);
                testInsertModelLevel0.SetData("TypeId", "Node", index);
                testInsertModelLevel0.SetData("HasChildren", true, index);

                testInsertModelLevel0.SetData("Date", "26.12.23", index);
                testInsertModelLevel0.SetData("Size",  String(tableTreeViewPage.size_ + index +1), index);

                tableTreeViewPage.size_++


                index = testInsertModelLevel0.InsertNewItem();
                testInsertModelLevel0.SetData("Id", String(val + index), index);
                testInsertModelLevel0.SetData("Name","ул. Менделеева", index);
                testInsertModelLevel0.SetData("TypeId", "Node", index);
                testInsertModelLevel0.SetData("HasChildren", true, index);

                testInsertModelLevel0.SetData("Date", "26.12.23", index);
                testInsertModelLevel0.SetData("Size",  String(tableTreeViewPage.size_ + index +1), index);

                tableTreeViewPage.size_++


                index = testInsertModelLevel0.InsertNewItem();
                testInsertModelLevel0.SetData("Id", String(val + index), index);
                testInsertModelLevel0.SetData("Name","ул. Королёва", index);
                testInsertModelLevel0.SetData("TypeId", "Node", index);
                testInsertModelLevel0.SetData("HasChildren", true, index);

                testInsertModelLevel0.SetData("Date", "26.12.23", index);
                testInsertModelLevel0.SetData("Size",  String(tableTreeViewPage.size_ + index +1), index);

                tableTreeViewPage.size_++


                index = testInsertModelLevel0.InsertNewItem();
                testInsertModelLevel0.SetData("Id", String(val + index), index);
                testInsertModelLevel0.SetData("Name","ул. Гагарина", index);
                testInsertModelLevel0.SetData("TypeId", "Node", index);
                testInsertModelLevel0.SetData("HasChildren", false, index);

                testInsertModelLevel0.SetData("Date", "26.12.23", index);
                testInsertModelLevel0.SetData("Size",  String(tableTreeViewPage.size_ + index +1), index);

                tableTreeViewPage.size_++


                index = testInsertModelLevel0.InsertNewItem();
                testInsertModelLevel0.SetData("Id", String(val + index), index);
                testInsertModelLevel0.SetData("Name","ул. Мира", index);
                testInsertModelLevel0.SetData("TypeId", "Doc", index);
                testInsertModelLevel0.SetData("HasChildren", false, index);

                testInsertModelLevel0.SetData("Date", "26.12.23", index);
                testInsertModelLevel0.SetData("Size",  String(tableTreeViewPage.size_ + index +1), index);

                tableTreeViewPage.size_++

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
            testInsertModel.SetData("Size",  String(tableTreeViewPage.size_ + index +1), index);

            tableTreeViewPage.size_++


            index = testInsertModel.InsertNewItem();
            testInsertModel.SetData("Name","дом 2" , index);
            testInsertModel.SetData("TypeId", "Node", index);
            testInsertModel.SetData("HasChildren", true, index);

            testInsertModel.SetData("Date", "26.12.23", index);
            testInsertModel.SetData("Size",  String(tableTreeViewPage.size_ + index +1), index);

            tableTreeViewPage.size_++


            index = testInsertModel.InsertNewItem();
            testInsertModel.SetData("Name", "дом 3" , index);
            testInsertModel.SetData("TypeId", "Doc", index);
            testInsertModel.SetData("HasChildren", false, index);

            testInsertModel.SetData("Date", "26.12.23", index);
            testInsertModel.SetData("Size",  String(tableTreeViewPage.size_ + index +1), index);

            tableTreeViewPage.size_++
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
