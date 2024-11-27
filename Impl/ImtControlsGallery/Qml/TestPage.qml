import QtQuick 2.12

import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0
import imtdocgui 1.0

Rectangle {
    id: testPage;

    anchors.fill: parent;
    clip: true;

    property int columnCount: 20
    property int rowCount: 100

    Table {
        id: table;
        width: parent.width;
        height: contentHeight;
        hasFilter: true;
        hasSort: true;
        canMoveColumns: true;

        TreeItemModel {
            id: elementsModel;

            Component.onCompleted: {

                for (let i = 0; i < testPage.rowCount; i++){
                    elementsModel.insertNewItem();

                    for (let j = 1; j <= testPage.columnCount; j++){
                        elementsModel.setData("Test" + j, "Test Data" + j, i);
                    }
                }

                table.headers = headersModel;
                table.elements = elementsModel;
            }
        }

        onHeaderRightMouseClicked: {
            ModalDialogManager.openDialog(tableHeaderParamComp, {});
        }

        onHeadersChanged: {
            resetViewParams()
        }

        Component {
            id: tableHeaderParamComp;

            TableHeaderParamComp{
                tableItem: table;
                onFinished: {
                }
            }
        }

        TreeItemModel {
            id: headersModel;

            Component.onCompleted: {
                for (let i = 1; i <= testPage.columnCount; i++){
                    let index = headersModel.insertNewItem();
                    headersModel.setData("Id", "Test" + i, index);
                    headersModel.setData("Name", "Test" + i, index);
                }
            }
        }
    }
}
