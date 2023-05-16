import QtQuick 2.0
import Acf 1.0
import imtgui  1.0

TreeViewItemDelegateBase {
    id: packageTreeItemDelegate;

    onRootChanged: {
        if (packageTreeItemDelegate.root != null){
            packageTreeItemDelegate.root.tableSelection.singleSelect(packageTreeItemDelegate.modelIndex);
        }
    }

    function idIsValid(id){
        let delegateItems = packageTreeItemDelegate.root.getItemsDataAsList();
        for (let item of delegateItems){
            let itemData = item.getItemData();
            if (itemData.Id === id){
                return false;
            }
        }

        return true;
    }

    rowBodyDelegate: Component{ Row {
            id: row;

            height: packageTreeItemDelegate.root ? packageTreeItemDelegate.root.rowItemHeight : 0;

            Item {
                id: nameItem;

                width: packageTreeItemDelegate.root ? packageTreeItemDelegate.root.width / packageTreeItemDelegate.root.columnCount - 20 * packageTreeItemDelegate.level : 0;
                height: packageTreeItemDelegate.root ? packageTreeItemDelegate.root.rowItemHeight : 0;

                Text {
                    anchors.verticalCenter: nameItem.verticalCenter;

                    width: parent.width;

                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                    color: Style.textColor;
                    wrapMode: Text.WordWrap;
                    elide: Text.ElideRight;

                    text: model.Name ? model.Name : "";
                }

                MouseArea {
                    anchors.fill: parent;

                    visible: packageTreeItemDelegate.root ? !packageTreeItemDelegate.root.readOnly : false;

                    onClicked: {
                        packageTreeItemDelegate.mouseArea.clicked(null);
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
                        inputName.visible = false;

                        if (model.Name !== inputName.text){
                            let id = inputName.text.replace(/\s+/g, '');

                            model.Name = inputName.text;

                            if (model.Id === ""){
                                let id = model.Name.replace(/\s+/g, '');
                                model.Id = id;
                            }
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

                    text: model.Id ? model.Id : "";
                }

                MouseArea {
                    anchors.fill: parent;

                    visible: packageTreeItemDelegate.root ? !packageTreeItemDelegate.root.readOnly : 0;

                    onClicked: {
                        packageTreeItemDelegate.mouseArea.clicked(null);
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

                    property bool block: false

                    onEditingFinished: {
                        if (modalDialogManager.count > 0){
                            return;
                        }

                        inputId.visible = false;

                        if (model.Id !== inputId.text){
                            let ok = packageTreeItemDelegate.root.canRename(model.Id);
                            if (ok){
                                model.Id = inputId.text;
                            }
                            else{
                                packageTreeItemDelegate.root.openFeatureErrorDialog(qsTr("It is impossible to rename the feature, please remove all dependencies"));
                            }
                        }

//                        let ok = packageTreeItemDelegate.idIsValid(inputId.text);
//                        if (ok){
//                            if (model.Id !== inputId.text){
//                                model.Id = inputId.text;
//                            }
//                        }
//                        else{
//                            packageTreeItemDelegate.root.openFeatureErrorDialog();
//                        }
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

                    text: model.Description ? model.Description : "";
                }

                MouseArea {
                    anchors.fill: parent;

                    visible: packageTreeItemDelegate.root ? !packageTreeItemDelegate.root.readOnly : false;

                    onClicked: {
                        packageTreeItemDelegate.mouseArea.clicked(null);
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
                        inputDescription.visible = false;

                        if (model.Description !== inputDescription.text){
                            model.Description = inputDescription.text;
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

                    visible: packageTreeItemDelegate.level !== 0 && !packageTreeItemDelegate.hasChild;

                    onClicked: {
                        model.Optional = !model.Optional;
                    }
                }
            }
        }
    }
}
