import QtQuick 2.0
import Acf 1.0
import imtgui  1.0

TreeViewItemDelegateBase {
    id: productTreeItemDelegate;

    root: tableView;

    signal addButtonClicked();
    signal removeButtonClicked();

    rowBodyDelegate: Component { Row {
        id: row;

        height: root.rowItemHeight;

        Item {
            width: checkBox.visible ? 20 : 0;
            height: root.rowItemHeight;

            CheckBox {
                id: checkBox;

                anchors.verticalCenter: parent.verticalCenter;

                visible: model.Optional ? model.Optional : false;

                checkState: model.State;

                onClicked: {
                    model.State = Qt.Checked - model.State;
                    root.rowModelDataChanged(productTreeItemDelegate, "State");
                }
            }
        }

        Item {
            id: nameItem;

            width: root.width / root.columnCount - 20 * model.Level;
            height: root.rowItemHeight;

            Text {
                anchors.verticalCenter: nameItem.verticalCenter;

                text: model.Name;

                width: parent.width;

                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
                color: Style.textColor;
                wrapMode: Text.WordWrap;
                elide: Text.ElideRight;
            }

            MouseArea {
                anchors.fill: parent;

                cursorShape: Qt.IBeamCursor;

                visible: !root.readOnly && model.Level == 0;

                onClicked: {
                    mouseArea.clicked(null);
                }

                onDoubleClicked: {
                    inputName.visible = true;
                    inputName.focus = true;
                }
            }

            CustomTextField {
                id: inputName;

                anchors.verticalCenter: nameItem.verticalCenter;

                width: parent.width - 50;
                height: parent.height - 5;

                text: model.Name ? model.Name : "";

                visible: false;

                onCancelled: {
                    visible = false;
                }

                onEditingFinished: {
                    visible = false;

                    if (model.Name != inputName.text){
                        model.Name = inputName.text;

                        if (model.Id == ""){
                            let id = model.Name.replace(/\s+/g, '');
                            model.Id = id;
                        }

                        root.rowModelDataChanged(productTreeItemDelegate, "Name");
                    }
                }
            }
        }

        Item {
            id: idItem;

            width: root.width / (root.columnCount + 1);
            height: root.rowItemHeight;

            Item {
                width: parent.width;
                height: parent.height;

                visible: model.Level == 0;

                Text {
                    anchors.verticalCenter: parent.verticalCenter;

                    text: model.Id;

                    width: parent.width;

                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                    color: Style.textColor;
                    wrapMode: Text.WordWrap;
                    elide: Text.ElideRight;
                }

                MouseArea {
                    anchors.fill: parent;

                    cursorShape: Qt.IBeamCursor;

                    visible: !root.readOnly && model.Level == 0;

                    onClicked: {
                        mouseArea.clicked(null);
                    }

                    onDoubleClicked: {
                        inputId.visible = true;
                        inputId.focus = true;
                    }
                }

                CustomTextField {
                    id: inputId;

                    anchors.verticalCenter: parent.verticalCenter;

                    width: parent.width - 50;
                    height: parent.height - 5;

                    text: model.Id ? model.Id : "";

                    visible: false;

                    onCancelled: {
                        visible = false;
                    }

                    onEditingFinished: {
                        visible = false;

                        if (model.Id != inputId.text){
                            model.Id = inputId.text;

                            root.rowModelDataChanged(productTreeItemDelegate, "Id");
                        }
                    }
                }
            }
        }

        Item {
            id: descriptionItem;

            width: root.width / (root.columnCount + 1);
            height: root.rowItemHeight;

            Item {
                width: parent.width;
                height: parent.height;

                visible: model.Level == 0;

                Text {
                    anchors.verticalCenter: parent.verticalCenter;

                    text: model.Description;

                    width: parent.width;

                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                    color: Style.textColor;
                    wrapMode: Text.WordWrap;
                    elide: Text.ElideRight;
                }

                MouseArea {
                    anchors.fill: parent;

                    cursorShape: Qt.IBeamCursor;

                    visible: !root.readOnly && model.Level == 0;

                    onClicked: {
                        mouseArea.clicked(null);
                    }

                    onDoubleClicked: {
                        inputDescription.visible = true;
                        inputDescription.focus = true;
                    }
                }

                CustomTextField {
                    id: inputDescription;

                    anchors.verticalCenter: parent.verticalCenter;

                    width: parent.width - 50;
                    height: parent.height - 5;

                    text: model.Description ? model.Description : "";

                    visible: false;

                    onCancelled: {
                        visible = false;
                    }

                    onEditingFinished: {
                        visible = false;

                        if (model.Description != inputDescription.text){
                            model.Description = inputDescription.text;

                            root.rowModelDataChanged(productTreeItemDelegate, "Description");
                        }
                    }
                }
            }
        }

        Item {
            id: addButtonRect;

            width: 18;
            height: root.rowItemHeight;

            visible: model.Level == 0;

            AuxButton {
                anchors.verticalCenter: addButtonRect.verticalCenter;
                anchors.horizontalCenter: addButtonRect.horizontalCenter;

                width: 18;
                height: width;

                iconSource: "../../../Icons/" + Style.theme + "/Add_On_Normal.svg";

                onClicked: {
                    console.log("onClicked");

                    productTreeItemDelegate.addButtonClicked();
                }
            }
        }

        Item {
            id: removeButtonRect;

            width: 18;
            height: root.rowItemHeight;

            visible: model.Level == 1;

            AuxButton {
                anchors.verticalCenter: removeButtonRect.verticalCenter;
                anchors.horizontalCenter: removeButtonRect.horizontalCenter;

                width: 18;
                height: width;

                iconSource: "../../../Icons/" + Style.theme + "/Delete_On_Normal.svg";

                onClicked: {
                    console.log("onClicked");

                    productTreeItemDelegate.removeButtonClicked();
                }
            }
        }
    }
    }
}
