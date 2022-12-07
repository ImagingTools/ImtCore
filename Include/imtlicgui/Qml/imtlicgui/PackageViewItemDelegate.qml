import QtQuick 2.0
import Acf 1.0
import imtgui  1.0

TreeViewItemDelegateBase {
    id: packageTreeItemDelegate;

    root: tableView;

    onClicked: {
        console.log("packageTreeItemDelegate onCompleted", model.Name);
        rowBodyItem.forceActiveFocus();
    }

    rowBodyDelegate: Row {
        id: row;

        height: root.rowItemHeight;

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

                visible: !root.readOnly;

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

                width: parent.width - 10;
                height: parent.height - 7;

                text: model.Name ? model.Name : "";

                visible: false;

                onCancelled: {
                    visible = false;
                }

                onEditingFinished: {
                    console.log("inputName onEditingFinished");

                    visible = false;

                    if (model.Name != inputName.text){
                        model.Name = inputName.text;

                        if (model.Id == ""){
                            let id = model.Name.replace(/\s+/g, '');
                            model.Id = id;
                        }

                        root.rowModelDataChanged(packageTreeItemDelegate, "Name");
                    }
                }
            }
        }

        Item {
            id: idItem;

            width: root.width / root.columnCount;
            height: root.rowItemHeight;

            Text {
                anchors.verticalCenter: idItem.verticalCenter;

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

                visible: !root.readOnly;

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

                anchors.verticalCenter: idItem.verticalCenter;

                width: parent.width - 10;
                height: parent.height - 7;

                text: model.Id ? model.Id : "";

                visible: false;

                onCancelled: {
                    visible = false;
                }

                onEditingFinished: {
                    console.log("inputName onEditingFinished");

                    visible = false;

                    if (model.Id != inputId.text){
                        model.Id = inputId.text;


                        root.rowModelDataChanged(packageTreeItemDelegate, "Id");
                    }
                }
            }
        }

        Item {
            id: descriptionItem;

            width: root.width / root.columnCount;
            height: root.rowItemHeight;

            Text {
                anchors.verticalCenter: descriptionItem.verticalCenter;

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

                visible: !root.readOnly;

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

                anchors.verticalCenter: descriptionItem.verticalCenter;

                width: parent.width - 10;
                height: parent.height - 7;

                text: model.Description ? model.Description : "";

                visible: false;

                onCancelled: {
                    visible = false;
                }

                onEditingFinished: {
                    console.log("inputName onEditingFinished");
                    visible = false;

                    if (model.Description != inputDescription.text){
                        model.Description = inputDescription.text;

                        root.rowModelDataChanged(packageTreeItemDelegate, "Description");
                    }
                }
            }
        }

        Item {
            id: optionalItem;

            width: root.width / root.columnCount;
            height: root.rowItemHeight;

            CheckBox {
                anchors.verticalCenter: parent.verticalCenter;
                anchors.horizontalCenter: parent.horizontalCenter;

                z: 10;

                checkState: model.Optional ? Qt.Checked : Qt.Unchecked;

                onClicked: {
                    model.Optional = !model.Optional;

                    root.rowModelDataChanged(packageTreeItemDelegate, "Optional");
                }
            }
        }
    }
}
