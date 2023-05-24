import QtQuick 2.0
import Acf 1.0
import imtgui  1.0

TreeViewItemDelegateBase {
    id: productTreeItemDelegate;

    signal addButtonClicked();
    signal removeButtonClicked();

    signal emptyIdError();

    signal dataChanged();

    checkState: model.CheckState ? model.CheckState : Qt.Unchecked;

    rowBodyDelegate: Component { Row {
            id: row;

            height: productTreeItemDelegate.root ? productTreeItemDelegate.root.rowItemHeight : 0;

            Item {
                width: checkBox.visible ? 20 : 0;
                height: productTreeItemDelegate.root ? productTreeItemDelegate.root.rowItemHeight : 0;

                CheckBox {
                    id: checkBox;

                    anchors.verticalCenter: parent.verticalCenter;

                    visible: model.Optional ? model.Optional : false;

                    checkState: productTreeItemDelegate.checkState;

                    onClicked: {
                        console.log("checkBox onClicked", model.CheckState);

                        model.CheckState = Qt.Checked - model.CheckState;
                    }
                }
            }

            Item {
                id: nameItem;

                width: productTreeItemDelegate.root ? productTreeItemDelegate.root.width / productTreeItemDelegate.root.columnCount - 20 * productTreeItemDelegate.level : 0;
                height: productTreeItemDelegate.root ? productTreeItemDelegate.root.rowItemHeight : 0;

                Text {
                    anchors.verticalCenter: nameItem.verticalCenter;

                    width: parent.width;

                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                    color: Style.textColor;
//                    wrapMode: Text.WordWrap;
                    elide: Text.ElideRight;

                    text: model.Name ? model.Name : "";
                }

                MouseArea {
                    anchors.fill: parent;

                    visible: productTreeItemDelegate.root ? !productTreeItemDelegate.root.readOnly && productTreeItemDelegate.level == 0 : false;
                    //                    visible: productTreeItemDelegate.root ? !productTreeItemDelegate.root.readOnly && model.Level == 0 : false;

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

                        if (model.Name !== inputName.text){
                            model.Name = inputName.text;

                            if (model.Id === ""){
                                let id = model.Name.replace(/\s+/g, '');

                                let alreadyExists = productTreeItemDelegate.root.licenseIdExists(id);
                                if (!alreadyExists){
                                    model.Id = id;
                                }
                            }

                            productTreeItemDelegate.dataChanged();

                            //                            productTreeItemDelegate.root.rowModelDataChanged(productTreeItemDelegate, "Name");
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

                    visible: productTreeItemDelegate.level == 0;

                    Text {
                        anchors.verticalCenter: parent.verticalCenter;

                        width: parent.width;

                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                        color: Style.textColor;
//                        wrapMode: Text.WordWrap;
                        elide: Text.ElideRight;

                        text: model.Id ? model.Id : "";
                    }

                    MouseArea {
                        anchors.fill: parent;

                        visible: productTreeItemDelegate.root ? !productTreeItemDelegate.root.readOnly && productTreeItemDelegate.level == 0 : false;
                        //                        visible: productTreeItemDelegate.root ? !productTreeItemDelegate.root.readOnly && model.Level == 0 : false;

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
                            if (modalDialogManager.count > 0){
                                return;
                            }

                            inputId.visible = false;

                            if (model.Id !== inputId.text){

                                let alreadyExists = productTreeItemDelegate.root.licenseIdExists(inputId.text);
                                if (!alreadyExists){
                                    model.Id = inputId.text;

                                    productTreeItemDelegate.dataChanged();
                                }
                                else{
                                    productTreeItemDelegate.root.openLicenseErrorDialog(qsTr("License-ID already exists"));
                                }
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

                    visible: productTreeItemDelegate.level == 0;

                    Text {
                        anchors.verticalCenter: parent.verticalCenter;

                        width: parent.width;

                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                        color: Style.textColor;
//                        wrapMode: Text.WordWrap;
                        elide: Text.ElideRight;

                        text: model.Description ? model.Description : "";
                    }

                    MouseArea {
                        anchors.fill: parent;
                        //                        visible: productTreeItemDelegate.root ? !productTreeItemDelegate.root.readOnly && model.Level == 0 : false;
                        visible: productTreeItemDelegate.root ? !productTreeItemDelegate.root.readOnly && productTreeItemDelegate.level == 0 : false;

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

                            if (model.Description !== inputDescription.text){
                                model.Description = inputDescription.text;

                                productTreeItemDelegate.dataChanged();
                            }
                        }
                    }
                }
            }

            Item {
                id: addButtonRect;

                width: 18;
                height: productTreeItemDelegate.root ? productTreeItemDelegate.root.rowItemHeight : 0;

                visible: productTreeItemDelegate.level == 0 && productTreeItemDelegate.selected && !productTreeItemDelegate.root.readOnly;

                AuxButton {
                    anchors.verticalCenter: addButtonRect.verticalCenter;
                    anchors.horizontalCenter: addButtonRect.horizontalCenter;

                    width: 18;
                    height: width;

                    iconSource: "../../../Icons/" + Style.theme + "/Add_On_Normal.svg";

                    onClicked: {
                        console.log("onClicked");

                        if (model.Id === ""){
                            productTreeItemDelegate.emptyIdError();
                        }
                        else{
                            productTreeItemDelegate.addButtonClicked();
                        }
                    }
                }
            }

            Item {
                id: removeButtonRect;

                width: 18;
                height: productTreeItemDelegate.root ? productTreeItemDelegate.root.rowItemHeight : 0;

                visible: productTreeItemDelegate.level == 1 && productTreeItemDelegate.selected && !productTreeItemDelegate.root.readOnly;

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
