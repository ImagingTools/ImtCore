import QtQuick 2.0
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

TreeViewItemDelegateBase {
    id: packageTreeItemDelegate;

    rowBodyDelegate: Component{ Row {
            id: row;

            height: packageTreeItemDelegate.root ? packageTreeItemDelegate.root.rowItemHeight : 0;

            property alias descriptionInput: inputDescription;
            property alias nameInput: inputName;
            property alias idInput: inputId;

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
                    elide: Text.ElideRight;

                    visible: !inputName.visible;

                    text: model.FeatureName ? model.FeatureName : "";
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

                    text: model.FeatureName ? model.FeatureName : "";

                    visible: false;

                    onCancelled: {
                        inputName.visible = false;
                    }

                    onEditingFinished: {
                        inputName.visible = false;

                        if (model.FeatureName !== inputName.text){
                            let id = inputName.text.replace(/\s+/g, '');

                            model.FeatureName = inputName.text;

                            if (model.FeatureId === ""){
                                let id = model.FeatureName.replace(/\s+/g, '');
                                let alreadyExists = packageTreeItemDelegate.root.featureIdExists(id);
                                if (!alreadyExists){
                                    model.FeatureId = id;
                                }
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
                    visible: !inputId.visible;
                    elide: Text.ElideRight;

                    text: model.FeatureId ? model.FeatureId : "";
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

                    text: model.FeatureId ? model.FeatureId : "";

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

                        if (model.FeatureId !== inputId.text){
                            let canRename = packageTreeItemDelegate.root.canRename(model.FeatureId);
                            if (canRename){
                                let alreadyExists = packageTreeItemDelegate.root.featureIdExists(inputId.text);
                                if (!alreadyExists){
                                    model.FeatureId = inputId.text;

                                    return;
                                }
                                else{
                                    packageTreeItemDelegate.root.openFeatureErrorDialog(qsTr("Feature-ID already exists (In this or another package)"));
                                }
                            }
                            else{
                                packageTreeItemDelegate.root.openFeatureErrorDialog(qsTr("It is impossible to rename the feature, please remove all dependencies"));
                            }
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
                    visible: !inputDescription.visible;
                    elide: Text.ElideRight;

                    text: model.FeatureDescription ? model.FeatureDescription : "";
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

                    text: model.FeatureDescription ? model.FeatureDescription : "";

                    visible: false;

                    onCancelled: {
                        inputDescription.visible = false;
                    }

                    onEditingFinished: {
                        inputDescription.visible = false;

                        if (model.FeatureDescription !== inputDescription.text){
                            model.FeatureDescription = inputDescription.text;
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

                    isActive: packageTreeItemDelegate.root ? !packageTreeItemDelegate.root.readOnly : true;

                    visible: packageTreeItemDelegate.level !== 0 && !packageTreeItemDelegate.hasChild;

                    onClicked: {
                        model.Optional = !model.Optional;
                    }
                }
            }
        }
    }
}
