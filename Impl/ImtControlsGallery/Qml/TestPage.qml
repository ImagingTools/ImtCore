import QtQuick 2.12

import Acf 1.0
import imtcontrols 1.0
import QtQuick.Window 2.2
import imtgui 1.0
import imtdocgui 1.0

Rectangle {
    id: testPage;

    anchors.fill: parent;
    clip: true;

    Button {
        id: button;

        width: 70;
        height: 30;

        text: "Open";

        onClicked: {
            ModalDialogManager.openDialog(dialogComp);
        }
    }

    Component {
        id: dialogComp;
        Dialog {
            id: dialog;
            width: 300;

            function fillButtons(){
               buttonsModel.clear();
               buttonsModel.append({"Id": Enums.ok, "Name": qsTr("Apply"), "Enabled": false});
               buttonsModel.append({"Id": Enums.cancel, "Name": qsTr("Close"), "Enabled": true});
            }

            contentComp: Component {
                id: productPairEditor;

                Table {
                    id: table;
                    width: dialog.width;
                    height: contentHeight;

                    TreeItemModel {
                        id: headers;

                        Component.onCompleted: {
                            let index = headers.insertNewItem();
                            headers.setData("Id", "Id", index);
                            headers.setData("Name", "Name", index);
                        }
                    }

                    TreeItemModel {
                        id: elements;

                        Component.onCompleted: {
                            let index = elements.insertNewItem();
                            elements.setData("Id", "Test1", index);
                            elements.setData("Name", "Test1", index);

                            index = elements.insertNewItem();
                            elements.setData("Id", "Test2", index);
                            elements.setData("Name", "Test2", index);

                            table.headers = headers;
                            table.elements = elements;
                        }
                    }

                    onDoubleClicked: {
                        console.log("Table onDoubleClicked");
                    }
                }
            }
        }
    }

    TreeItemModel {
        id: rootModel;
    }

}
