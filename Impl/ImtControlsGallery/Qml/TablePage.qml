import QtQuick 2.12

import Acf 1.0
import imtcontrols 1.0


Rectangle {
    id: tablePage;

    anchors.fill: parent;

    property TreeItemModel items: TreeItemModel{};

    TreeItemModel{
        id: tableDataModel;
        property bool compl: false;
        Component.onCompleted: {
            for(let i = 0; i < 20; i++){
                let index = tableDataModel.InsertNewItem();
                tableDataModel.SetData("Account", 10000 + index, index);
                tableDataModel.SetData("FullName", "Иванов Иван Иванович", index);
                tableDataModel.SetData("Address", "Проспект Мира " + String(index +1), index);
                tableDataModel.SetData("Residents", String(index +1), index);
                tableDataModel.SetData("Branch", String(index +1), index);
                tableDataModel.SetData("Type", String(index +1), index);
                tableDataModel.SetData("Gauges", String(index +1), index);
            }

            tableDataModel.compl = true;
            tablePage.items = tableDataModel;
        }

    }


    TreeItemModel{
        id: tableHeaderModel;
        property bool compl: false;
        Component.onCompleted: {
            let index = tableHeaderModel.InsertNewItem();
            tableHeaderModel.SetData("Id", "Account", index);
            tableHeaderModel.SetData("Name", qsTr("ЛС"), index);

            index = tableHeaderModel.InsertNewItem();
            tableHeaderModel.SetData("Id", "FullName", index);
            tableHeaderModel.SetData("Name", qsTr("ФИО"), index);

            index = tableHeaderModel.InsertNewItem();
            tableHeaderModel.SetData("Id", "Address", index);
            tableHeaderModel.SetData("Name", qsTr("Адрес"), index);

            index = tableHeaderModel.InsertNewItem();
            tableHeaderModel.SetData("Id", "Residents", index);
            tableHeaderModel.SetData("Name", qsTr("Проживает"), index);

            index = tableHeaderModel.InsertNewItem();
            tableHeaderModel.SetData("Id", "Branch", index);
            tableHeaderModel.SetData("Name", qsTr("Филиал"), index);

            index = tableHeaderModel.InsertNewItem();
            tableHeaderModel.SetData("Id", "Type", index);
            tableHeaderModel.SetData("Name", qsTr("Тип"), index);

            index = tableHeaderModel.InsertNewItem();
            tableHeaderModel.SetData("Id", "Gauges", index);
            tableHeaderModel.SetData("Name", qsTr("Счетчики"), index);

            tableHeaderModel.compl = true;
            table.headers = tableHeaderModel;
            tableDecor.headers = tableHeaderModel;
        }

    }



    AuxTable{
        id: table;

        anchors.top: parent.top;
        anchors.topMargin: Style.size_mainMargin;
        anchors.left: parent.left;
        anchors.leftMargin: Style.size_mainMargin;
        anchors.right: parent.right;
        anchors.rightMargin: Style.size_mainMargin;
        height: 400;
        itemHeight: 50;
        headerHeight: 50;
        visibleLeftBorderFirst: false;
        hasFilter: false;
        hasSort: false;
        cacheBuffer: 0;
        canSetBorderParams: true;
        elements: tablePage.items;

        onSelectItem: {
        }

        onSelectedIndexChanged: {
        }

        onTextFilterChanged: {
        }

        onElementsChanged: {
        }

        onFilterClicked: {
        }

    }

    AuxTable{
        id: tableDecor;

        anchors.top: table.bottom;
        anchors.topMargin: 70;
        anchors.left: parent.left;
        anchors.leftMargin: Style.size_mainMargin;
        anchors.right: parent.right;
        anchors.rightMargin: Style.size_mainMargin;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 50;

        itemHeight: 50;
        headerHeight: 50;
        visibleLeftBorderFirst: false;
        hasFilter: false;
        hasSort: false;
        cacheBuffer: 0;
        canSetBorderParams: true;
        enableAlternating : false;
        canMoveColumns: true;
        elements: tablePage.items;

        delegate:
            TableDelegate {
                id: tableRow;

                minHeight: tableDecor.itemHeight;
                width: tableDecor.width;

                cellDecorator: tableDecor.cellDecorator;
                widthDecorator: tableDecor.widthDecorator;
                canSetBorderParams: true;
                wrapMode: Text.NoWrap;
                headers: tableHeaderModel;
                tableItem: tableDecor;
        }

        onSelectItem: {
        }

        onSelectedIndexChanged: {
        }

        onTextFilterChanged: {
        }

        onElementsChanged: {
        }

        onFilterClicked: {
        }

    }

    TableDecorator{
        id: tableDec;
    }

    TreeItemModel{
        id: decorModel;

        property bool compl: false;
        property bool complCompl: compl && tableDec.compl && tableHeaderModel.compl;

        Component.onCompleted: {decorModel.compl = true;}

        onComplComplChanged: {
            if(decorModel.complCompl){
                console.log("onTableDecoratorChanged decorModel")
                var cells = tableDec.GetTreeItemModel("Cells");
                cells.SetData("FontSize",18);
                tableDec.setDefault(decorModel,tableHeaderModel.GetItemsCount());

                var headers = decorModel.GetTreeItemModel("Headers");//
                cells = decorModel.GetTreeItemModel("Cells");//
                var cellWidth = decorModel.GetTreeItemModel("CellWidth");//


                cellWidth.SetData("Width",100,0);
                cellWidth.SetData("Width",300,1);
                cellWidth.SetData("Width",400,2);

                cells.SetData("TextPosition",Text.AlignLeft,1);
                cells.SetData("TextPosition",Text.AlignLeft,2);

                headers.SetData("LeftTopRound",true,0)
                headers.SetData("RightTopRound",true,tableHeaderModel.GetItemsCount()-1)

                tableDecor.tableDecorator = decorModel;

            }
        }
    }


   
}
