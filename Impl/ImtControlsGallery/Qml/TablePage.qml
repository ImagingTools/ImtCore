import QtQuick 2.12

import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0


Rectangle {
    id: tablePage;

    anchors.fill: parent;
    clip: true;
	color: Style.baseColor

    property TreeItemModel items: TreeItemModel{};

    TreeItemModel{
        id: tableDataModel;
        property bool compl: false;
        Component.onCompleted: {
            for(let i = 0; i < 200; i++){
                let index = tableDataModel.insertNewItem();
                tableDataModel.setData("Account", 10000 + index, index);
                tableDataModel.setData("FullName", "Иванов Иван Иванович", index);
                tableDataModel.setData("Address", "Проспект Мира " + String(index +1), index);
                tableDataModel.setData("Residents", String(index +1), index);
                tableDataModel.setData("Branch", String(index +1), index);
                tableDataModel.setData("Type", String(index +1), index);
                tableDataModel.setData("Gauges", String(index +1), index);

                tableDataModel.setData("Account2", 10000 + index, index);
                tableDataModel.setData("FullName2", "Иванов Иван Иванович", index);
                tableDataModel.setData("Address2", "Проспект Мира " + String(index +1), index);
                tableDataModel.setData("Residents2", String(index +1), index);
                tableDataModel.setData("Branch2", String(index +1), index);
                tableDataModel.setData("Type2", String(index +1), index);
                tableDataModel.setData("Gauges2", String(index +1), index);

                tableDataModel.setData("Account3", 10000 + index, index);
                tableDataModel.setData("FullName3", "Иванов Иван Иванович", index);
                tableDataModel.setData("Address3", "Проспект Мира " + String(index +1), index);
                tableDataModel.setData("Residents3", String(index +1), index);
                tableDataModel.setData("Branch3", String(index +1), index);
                tableDataModel.setData("Type3", String(index +1), index);
                tableDataModel.setData("Gauges3", String(index +1), index);
            }

            tableDataModel.compl = true;
            // tablePage.items = tableDataModel;
        }

    }


    TreeItemModel{
        id: tableHeaderModel;
        property bool compl: false;
        Component.onCompleted: {
            let index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "Account", index);
            tableHeaderModel.setData("name", qsTr("ЛС"), index);

            index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "FullName", index);
            tableHeaderModel.setData("name", qsTr("ФИО"), index);

            index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "Address", index);
            tableHeaderModel.setData("name", qsTr("Адрес"), index);

            index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "Residents", index);
            tableHeaderModel.setData("name", qsTr("Проживает"), index);

            index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "Branch", index);
            tableHeaderModel.setData("name", qsTr("Филиал"), index);

            index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "Type", index);
            tableHeaderModel.setData("name", qsTr("Тип"), index);

            index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "Gauges", index);
            tableHeaderModel.setData("name", qsTr("Счетчики"), index);


            index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "Account2", index);
            tableHeaderModel.setData("name", qsTr("ЛС"), index);

            index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "FullName2", index);
            tableHeaderModel.setData("name", qsTr("ФИО"), index);

            index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "Address2", index);
            tableHeaderModel.setData("name", qsTr("Адрес"), index);

            index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "Residents2", index);
            tableHeaderModel.setData("name", qsTr("Проживает"), index);

            index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "Branch2", index);
            tableHeaderModel.setData("name", qsTr("Филиал"), index);

            index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "Type2", index);
            tableHeaderModel.setData("name", qsTr("Тип"), index);

            index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "Gauges2", index);
            tableHeaderModel.setData("name", qsTr("Счетчики"), index);


            index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "Account3", index);
            tableHeaderModel.setData("name", qsTr("ЛС"), index);

            index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "FullName3", index);
            tableHeaderModel.setData("name", qsTr("ФИО"), index);

            index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "Address3", index);
            tableHeaderModel.setData("name", qsTr("Адрес"), index);

            index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "Residents3", index);
            tableHeaderModel.setData("name", qsTr("Проживает"), index);

            index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "Branch3", index);
            tableHeaderModel.setData("name", qsTr("Филиал"), index);

            index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "Type3", index);
            tableHeaderModel.setData("name", qsTr("Тип"), index);

            index = tableHeaderModel.insertNewItem();
            tableHeaderModel.setData("id", "Gauges3", index);
            tableHeaderModel.setData("name", qsTr("Счетчики"), index);

            table.headers = tableHeaderModel;
            tableDecor.headers = tableHeaderModel;
            tableHeaderModel.compl = true;
            tableDecor.tableDecorator = decorModel;
            tablePage.items = tableDataModel;
        }

    }



    Table{
        id: table;

        anchors.top: parent.top;
        anchors.topMargin: Style.marginM;
        anchors.left: parent.left;
        anchors.leftMargin: Style.marginM;
        anchors.right: parent.right;
        anchors.rightMargin: Style.marginM;
        height: 500;
        itemHeight: 50;
        headerHeight: 50;
        visibleLeftBorderFirst: false;
        hasFilter: false;
        hasSort: false;
        canSetBorderParams: true;
        elements: tablePage.items;

        onSelectedIndexChanged: {
        }

        onTextFilterChanged: {
        }

        onElementsChanged: {
        }

        onFilterClicked: {
        }

    }

    Table{
        id: tableDecor;

        anchors.top: table.bottom;
        anchors.topMargin: 70;
        anchors.left: parent.left;
        anchors.leftMargin: Style.marginM;
        anchors.right: parent.right;
        anchors.rightMargin: Style.marginM;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 50;

        itemHeight: 50;
        headerHeight: 50;
        visibleLeftBorderFirst: false;
        hasFilter: false;
        hasSort: false;
        canSetBorderParams: true;
        enableAlternating : false;
        canMoveColumns: true;
        elements: tablePage.items;

        // delegate: TableRowDelegateBase{

        // }

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
                var cells = tableDec.getTreeItemModel("Cells");
                cells.setData("FontSize",18);
                tableDec.setDefault(decorModel,tableHeaderModel.getItemsCount());

                var headers = decorModel.getTreeItemModel("Headers");//
                cells = decorModel.getTreeItemModel("Cells");//
                var cellWidth = decorModel.getTreeItemModel("CellWidth");//


                cellWidth.setData("Width",100,0);
                cellWidth.setData("Width",300,1);
                cellWidth.setData("Width",400,2);

                cells.setData("TextPosition",Text.AlignLeft,1);
                cells.setData("TextPosition",Text.AlignLeft,2);

                headers.setData("LeftTopRound",true,0)
                headers.setData("RightTopRound",true,tableHeaderModel.getItemsCount()-1)
            }
        }
    }


   
}
