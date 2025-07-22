import QtQuick 2.12

import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0


Rectangle {
    id: tableTreeViewPage;

    anchors.fill: parent;
    clip: true;
	color: Style.baseColor

    property int size_: 100;

    TreeItemModel{
        id: testInsertModelLevel0;

        property bool compl: false;
        Component.onCompleted: {//TEST

            let date = new Date();
            let val = date.valueOf();
            let index;

            for(let i = 0; i < 10; i++){
                index = testInsertModelLevel0.insertNewItem();
                testInsertModelLevel0.setData("id", String(val + index), index);
                testInsertModelLevel0.setData("name","ул. Ленина", index);
                testInsertModelLevel0.setData("TypeId__", "Node", index);
                testInsertModelLevel0.setData("HasChildren__", true, index);

                testInsertModelLevel0.setData("Date", "26.12.23", index);
                testInsertModelLevel0.setData("Size",  String(tableTreeViewPage.size_ + index +1), index);

                tableTreeViewPage.size_++


                index = testInsertModelLevel0.insertNewItem();
                testInsertModelLevel0.setData("id", String(val + index), index);
                testInsertModelLevel0.setData("name","ул. Маркса", index);
                testInsertModelLevel0.setData("TypeId__", "Node", index);
                testInsertModelLevel0.setData("HasChildren__", true, index);

                testInsertModelLevel0.setData("Date", "26.12.23", index);
                testInsertModelLevel0.setData("Size",  String(tableTreeViewPage.size_ + index +1), index);

                tableTreeViewPage.size_++


                index = testInsertModelLevel0.insertNewItem();
                testInsertModelLevel0.setData("id", String(val + index), index);
                testInsertModelLevel0.setData("name","ул. Менделеева", index);
                testInsertModelLevel0.setData("TypeId__", "Node", index);
                testInsertModelLevel0.setData("HasChildren__", true, index);

                testInsertModelLevel0.setData("Date", "26.12.23", index);
                testInsertModelLevel0.setData("Size",  String(tableTreeViewPage.size_ + index +1), index);

                tableTreeViewPage.size_++


                index = testInsertModelLevel0.insertNewItem();
                testInsertModelLevel0.setData("id", String(val + index), index);
                testInsertModelLevel0.setData("name","ул. Королёва", index);
                testInsertModelLevel0.setData("TypeId__", "Node", index);
                testInsertModelLevel0.setData("HasChildren__", true, index);

                testInsertModelLevel0.setData("Date", "26.12.23", index);
                testInsertModelLevel0.setData("Size",  String(tableTreeViewPage.size_ + index +1), index);

                tableTreeViewPage.size_++


                index = testInsertModelLevel0.insertNewItem();
                testInsertModelLevel0.setData("id", String(val + index), index);
                testInsertModelLevel0.setData("name","ул. Гагарина", index);
                testInsertModelLevel0.setData("TypeId__", "Node", index);
                testInsertModelLevel0.setData("HasChildren__", false, index);

                testInsertModelLevel0.setData("Date", "26.12.23", index);
                testInsertModelLevel0.setData("Size",  String(tableTreeViewPage.size_ + index +1), index);

                tableTreeViewPage.size_++


                index = testInsertModelLevel0.insertNewItem();
                testInsertModelLevel0.setData("id", String(val + index), index);
                testInsertModelLevel0.setData("name","ул. Мира", index);
                testInsertModelLevel0.setData("TypeId__", "Doc", index);
                testInsertModelLevel0.setData("HasChildren__", false, index);

                testInsertModelLevel0.setData("Date", "26.12.23", index);
                testInsertModelLevel0.setData("Size",  String(tableTreeViewPage.size_ + index +1), index);

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

            index = testInsertModel.insertNewItem();
            testInsertModel.setData("name", "дом 1" , index);
            testInsertModel.setData("TypeId__", "Node", index);
            testInsertModel.setData("HasChildren__", true, index);

            testInsertModel.setData("Date", "26.12.23", index);
            testInsertModel.setData("Size",  String(tableTreeViewPage.size_ + index +1), index);

            tableTreeViewPage.size_++


            index = testInsertModel.insertNewItem();
            testInsertModel.setData("name","дом 2" , index);
            testInsertModel.setData("TypeId__", "Node", index);
            testInsertModel.setData("HasChildren__", true, index);

            testInsertModel.setData("Date", "26.12.23", index);
            testInsertModel.setData("Size",  String(tableTreeViewPage.size_ + index +1), index);

            tableTreeViewPage.size_++


            index = testInsertModel.insertNewItem();
            testInsertModel.setData("name", "дом 3" , index);
            testInsertModel.setData("TypeId__", "Doc", index);
            testInsertModel.setData("HasChildren__", false, index);

            testInsertModel.setData("Date", "26.12.23", index);
            testInsertModel.setData("Size",  String(tableTreeViewPage.size_ + index +1), index);

            tableTreeViewPage.size_++
        }
    }



    TreeItemModel{
        id: tableHeaderModel;

        property bool compl: false;
        Component.onCompleted: {
            let index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "name", index);
            tableHeaderModel.setData("name", qsTr("name"), index);

            index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "Date", index);
            tableHeaderModel.setData("name", qsTr("Date"), index);

            index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "Size", index);
            tableHeaderModel.setData("name", qsTr("Size"), index);


            tableTreeView.headers = tableHeaderModel;
            tableHeaderModel.compl = true;
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
                var headers = tableDec.getTreeItemModel("Headers");
                //headers.setData("CellRadius",2);
                tableDec.setDefault(decorModel,tableHeaderModel.getItemsCount());

                headers = decorModel.getTreeItemModel("Headers");//
                var cells = decorModel.getTreeItemModel("Cells");//
                var cellWidth = decorModel.getTreeItemModel("CellWidth");//


                //cellWidth.setData("Width",200,1);
                //cellWidth.setData("Width",200,2);
                cellWidth.setData("MinWidth",200,0);
                //cellWidth.setData("MinWidth",100,1);
                //cellWidth.setData("MinWidth",100,2);


                //cells.setData("TextPosition",Text.AlignLeft,0);
                //cells.setData("TextPosition",Text.AlignLeft,1);
                //cells.setData("TextPosition",Text.AlignLeft,2);

                //headers.setData("LeftTopRound",true,0)
                //headers.setData("RightTopRound",true,tableHeaderModel.getItemsCount()-1)

                tableTreeView.tableDecorator = decorModel;

            }
        }
    }



}
