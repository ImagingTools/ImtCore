import QtQuick 2.0
import Acf 1.0
import imtgui  1.0

TreeViewItemDelegateBase {
    id: productTreeItemDelegate;

    //root: tableView;

    signal addButtonClicked();
    signal removeButtonClicked();

    rowBodyDelegate: Component { Row {
            id: row;

            height: productTreeItemDelegate.root ? root.rowItemHeight : 0;

            Item {
                width: checkBox.visible ? 20 : 0;
                height: productTreeItemDelegate.root ? productTreeItemDelegate.root.rowItemHeight : 0;

                CheckBox {
                    id: checkBox;

                    anchors.verticalCenter: parent.verticalCenter;

                    visible: model.Optional ? model.Optional && model.Level != 1 : false;

                    checkState: model.State;

                    onClicked: {
                        model.State = Qt.Checked - model.State;
                        productTreeItemDelegate.root.rowModelDataChanged(productTreeItemDelegate, "State");
                    }
                }
            }

            Item {
                id: nameItem;

                width: productTreeItemDelegate.root ? productTreeItemDelegate.root.width / productTreeItemDelegate.root.columnCount - 20 * model.Level : 0;
                height: productTreeItemDelegate.root ? productTreeItemDelegate.root.rowItemHeight : 0;

                Text {
                    anchors.verticalCenter: nameItem.verticalCenter;

                    width: parent.width;

                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                    color: Style.textColor;
                    wrapMode: Text.WordWrap;
                    elide: Text.ElideRight;

                    text: model.Name;
                }

                MouseArea {
                    anchors.fill: parent;

                    cursorShape: Qt.IBeamCursor;

                    visible: productTreeItemDelegate.root ? !productTreeItemDelegate.root.readOnly && model.Level == 0 : false;

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
                        inputName.visible = false;
                    }

                    onEditingFinished: {
                        inputName.visible = false;

                        if (model.Name != inputName.text){
                            model.Name = inputName.text;

                            if (model.Id == ""){
                                let id = model.Name.replace(/\s+/g, '');
                                model.Id = id;
                            }

                            productTreeItemDelegate.root.rowModelDataChanged(productTreeItemDelegate, "Name");
                        }
                    }
                }
            }

            Item {
                id: idItem;

                width: productTreeItemDelegate.root ? productTreeItemDelegate.root.width / (productTreeItemDelegate.root.columnCount + 1) : 0;
                height: productTreeItemDelegate.root ? productTreeItemDelegate.root.rowItemHeight : 0;

                Item {
                    width: parent.width;
                    height: parent.height;

                    visible: model.Level == 0;

                    Text {
                        anchors.verticalCenter: parent.verticalCenter;

                        width: parent.width;

                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                        color: Style.textColor;
                        wrapMode: Text.WordWrap;
                        elide: Text.ElideRight;

                        text: model.Id;
                    }

                    MouseArea {
                        anchors.fill: parent;

                        cursorShape: Qt.IBeamCursor;

                        visible: productTreeItemDelegate.root ? !productTreeItemDelegate.root.readOnly && model.Level == 0 : false;

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
                            inputId.visible = false;
                        }

                        onEditingFinished: {
                            inputId.visible = false;

                            if (model.Id != inputId.text){
                                model.Id = inputId.text;

                                productTreeItemDelegate.root.rowModelDataChanged(productTreeItemDelegate, "Id");
                            }
                        }
                    }
                }
            }

            Item {
                id: descriptionItem;

                width: productTreeItemDelegate.root ? productTreeItemDelegate.root.width / (productTreeItemDelegate.root.columnCount + 1) : 0;
                height: productTreeItemDelegate.root ? productTreeItemDelegate.root.rowItemHeight : 0;

                Item {
                    width: parent.width;
                    height: parent.height;

                    visible: model.Level == 0;

                    Text {
                        anchors.verticalCenter: parent.verticalCenter;

                        width: parent.width;

                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                        color: Style.textColor;
                        wrapMode: Text.WordWrap;
                        elide: Text.ElideRight;

                        text: model.Description;
                    }

                    MouseArea {
                        anchors.fill: parent;

                        cursorShape: Qt.IBeamCursor;

                        visible: productTreeItemDelegate.root ? !productTreeItemDelegate.root.readOnly && model.Level == 0 : false;

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
                            inputDescription.visible = false;
                        }

                        onEditingFinished: {
                            inputDescription.visible = false;

                            if (model.Description != inputDescription.text){
                                model.Description = inputDescription.text;

                                productTreeItemDelegate.root.rowModelDataChanged(productTreeItemDelegate, "Description");
                            }
                        }
                    }
                }
            }

            Item {
                id: addButtonRect;

                width: 18;
                height: productTreeItemDelegate.root ? productTreeItemDelegate.root.rowItemHeight : 0;

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
                height: productTreeItemDelegate.root ? productTreeItemDelegate.root.rowItemHeight : 0;

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
