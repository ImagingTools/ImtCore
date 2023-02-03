import QtQuick 2.0
import Acf 1.0
import imtgui  1.0

TreeViewItemDelegateBase {
    id: packageTreeItemDelegate;

    //root: tableView;

    rowBodyDelegate: Component{ Row {
            id: row;

            height: packageTreeItemDelegate.root ? packageTreeItemDelegate.root.rowItemHeight : 0;

            Item {
                id: nameItem;

                width: packageTreeItemDelegate.root ? packageTreeItemDelegate.root.width / packageTreeItemDelegate.root.columnCount - 20 * model.Level : 0;
                height: packageTreeItemDelegate.root ? packageTreeItemDelegate.root.rowItemHeight : 0;

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

                    visible: packageTreeItemDelegate.root ? !packageTreeItemDelegate.root.readOnly : false;

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
                        inputName.visible = false;
                    }

                    onEditingFinished: {
                        console.log("inputName onEditingFinished");

                        inputName.visible = false;

                        if (model.Name != inputName.text){
                            model.Name = inputName.text;

                            if (model.Id == ""){
                                let id = model.Name.replace(/\s+/g, '');
                                model.Id = id;
                            }

                            packageTreeItemDelegate.root.rowModelDataChanged(packageTreeItemDelegate, "Name");
                        }
                    }
                }
            }

            Item {
                id: idItem;

                width: packageTreeItemDelegate.root ? packageTreeItemDelegate.root.width / packageTreeItemDelegate.root.columnCount : 0;
                height: packageTreeItemDelegate.root ? packageTreeItemDelegate.root.rowItemHeight : 0;

                Text {
                    anchors.verticalCenter: idItem.verticalCenter;


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

                    visible: packageTreeItemDelegate.root ? !packageTreeItemDelegate.root.readOnly : 0;

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
                        inputId.visible = false;
                    }

                    onEditingFinished: {
                        console.log("inputName onEditingFinished");

                        inputId.visible = false;

                        if (model.Id != inputId.text){
                            model.Id = inputId.text;


                            packageTreeItemDelegate.root.rowModelDataChanged(packageTreeItemDelegate, "Id");
                        }
                    }
                }
            }

            Item {
                id: descriptionItem;

                width: packageTreeItemDelegate.root ? packageTreeItemDelegate.root.width / packageTreeItemDelegate.root.columnCount : 0;
                height: packageTreeItemDelegate.root ? packageTreeItemDelegate.root.rowItemHeight : 0;

                Text {
                    anchors.verticalCenter: descriptionItem.verticalCenter;

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

                    visible: packageTreeItemDelegate.root ? !packageTreeItemDelegate.root.readOnly : false;

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
                        inputDescription.visible = false;
                    }

                    onEditingFinished: {
                        console.log("inputName onEditingFinished");
                        inputDescription.visible = false;

                        if (model.Description != inputDescription.text){
                            model.Description = inputDescription.text;

                            packageTreeItemDelegate.root.rowModelDataChanged(packageTreeItemDelegate, "Description");
                        }
                    }
                }
            }

            Item {
                id: optionalItem;

                width: packageTreeItemDelegate.root ? packageTreeItemDelegate.root.width / packageTreeItemDelegate.root.columnCount : 0;
                height: packageTreeItemDelegate.root ? packageTreeItemDelegate.root.rowItemHeight : 0;

                CheckBox {
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.horizontalCenter: parent.horizontalCenter;

                    z: 10;

                    checkState: model.Optional ? Qt.Checked : Qt.Unchecked;

                    visible: model.ChildModel ? model.ChildModel.count === 0: true;

                    onClicked: {
                        model.Optional = !model.Optional;

                        packageTreeItemDelegate.root.rowModelDataChanged(packageTreeItemDelegate, "Optional");
                    }
                }
            }
        }
    }
}
