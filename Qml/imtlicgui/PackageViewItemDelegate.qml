import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0
import imtlicFeaturesSdl 1.0

TreeViewItemDelegateBase {
    id: packageTreeItemDelegate;

    property FeatureData featureData: model.item ? model.item : null;
    itemData: featureData;

    signal error(string message);

    function featureIsValid(featureId, featureName){
        return true;
    }

    function canRename(featureId){
        return true;
    }

    rowBodyDelegate: Component{ Row {
            id: row;

            height: packageTreeItemDelegate.root ? packageTreeItemDelegate.root.rowItemHeight : 0;

            property alias descriptionInput: inputDescription;
            property alias nameInput: inputName;
            property alias idInput: inputId;

            Item {
                id: nameItem;
                width: packageTreeItemDelegate.root && packageTreeItemDelegate.root.columnCount !== 0 ? packageTreeItemDelegate.root.width / packageTreeItemDelegate.root.columnCount - 20 * packageTreeItemDelegate.level : 0;
                height: packageTreeItemDelegate.root ? packageTreeItemDelegate.root.rowItemHeight : 0;

                Text {
                    anchors.verticalCenter: nameItem.verticalCenter;
                    width: parent.width;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                    color: Style.textColor;
                    elide: Text.ElideRight;
                    visible: !inputName.visible;
                    text: packageTreeItemDelegate.featureData ? packageTreeItemDelegate.featureData.m_featureName : "";
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
                    text: packageTreeItemDelegate.featureData ? packageTreeItemDelegate.featureData.m_featureName : "";
                    visible: false;
                    autoEditingFinished: false;
                    onCancelled: {
                        inputName.visible = false;
                    }
                    onEditingFinished: {
                        inputName.visible = false;
                        let featureName = packageTreeItemDelegate.featureData.m_featureName;
                        let featureId = packageTreeItemDelegate.featureData.m_featureId;

                        if (featureName !== inputName.text){
                            let id = inputName.text.replace(/\s+/g, '');
                            if (featureId === ""){
                                let id = inputName.text.replace(/\s+/g, '');
                                let isValid = packageTreeItemDelegate.featureIsValid(id, inputName.text);
                                if (!isValid){
                                    return;
                                }

                                packageTreeItemDelegate.featureData.m_featureId = id;
                            }

                            packageTreeItemDelegate.featureData.m_featureName = inputName.text;
                        }
                    }
                }
            }

            Item {
                id: idItem;

                width: packageTreeItemDelegate.root && packageTreeItemDelegate.root.columnCount !== 0 ? packageTreeItemDelegate.root.width / packageTreeItemDelegate.root.columnCount : 0;
                height: packageTreeItemDelegate.root ? packageTreeItemDelegate.root.rowItemHeight : 0;

                Text {
                    anchors.verticalCenter: idItem.verticalCenter;
                    width: parent.width;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                    color: Style.textColor;
                    visible: !inputId.visible;
                    elide: Text.ElideRight;
                    text: packageTreeItemDelegate.featureData ? packageTreeItemDelegate.featureData.m_featureId : "";
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
                    text: packageTreeItemDelegate.featureData ? packageTreeItemDelegate.featureData.m_featureId : "";
                    visible: false;
                    autoEditingFinished: false;

                    onCancelled: {
                        inputId.visible = false;
                    }

                    property bool block: false

                    onEditingFinished: {
                        inputId.visible = false;

                        let featureId = packageTreeItemDelegate.featureData.m_featureId;
                        if (featureId !== inputId.text){
                            let canRename = packageTreeItemDelegate.canRename(featureId);
                            if (canRename){
                                let isValid = packageTreeItemDelegate.featureIsValid(inputId.text, inputName.text);
                                if (isValid){
                                    packageTreeItemDelegate.featureData.m_featureId = inputId.text;

                                    return;
                                }
                                else{
                                    packageTreeItemDelegate.error(qsTr("Feature-ID already exists (In this or another package)"));
                                }
                            }
                            else{
                                packageTreeItemDelegate.error(qsTr("It is impossible to rename the feature, please remove all dependencies"));
                            }
                        }
                    }
                }
            }

            Item {
                id: descriptionItem;

                width: packageTreeItemDelegate.root && packageTreeItemDelegate.root.columnCount !== 0 ? packageTreeItemDelegate.root.width / packageTreeItemDelegate.root.columnCount : 0;
                height: packageTreeItemDelegate.root ? packageTreeItemDelegate.root.rowItemHeight : 0;

                Text {
                    anchors.verticalCenter: descriptionItem.verticalCenter;
                    width: parent.width;
                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                    color: Style.textColor;
                    visible: !inputDescription.visible;
                    elide: Text.ElideRight;
                    text: packageTreeItemDelegate.featureData ? packageTreeItemDelegate.featureData.m_description : "";
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
                    text: packageTreeItemDelegate.featureData ? packageTreeItemDelegate.featureData.m_description : "";
                    visible: false;
                    autoEditingFinished: false;

                    onCancelled: {
                        inputDescription.visible = false;
                    }

                    onEditingFinished: {
                        inputDescription.visible = false;

                        let description = packageTreeItemDelegate.featureData.m_description;
                        if (description !== inputDescription.text){
                            packageTreeItemDelegate.featureData.m_description = inputDescription.text;
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
                    checkState: packageTreeItemDelegate.featureData.m_optional ? Qt.Checked : Qt.Unchecked;
                    isActive: packageTreeItemDelegate.root ? !packageTreeItemDelegate.root.readOnly : true;
                    visible: !packageTreeItemDelegate.hasChild;

                    onClicked: {
                        packageTreeItemDelegate.featureData.m_optional = !packageTreeItemDelegate.featureData.m_optional;
                    }
                }
            }

            Item {
                id: permissionItem;

                width: packageTreeItemDelegate.root ? packageTreeItemDelegate.root.width / packageTreeItemDelegate.root.columnCount : 0;
                height: packageTreeItemDelegate.root ? packageTreeItemDelegate.root.rowItemHeight : 0;

                CheckBox {
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.horizontalCenter: parent.horizontalCenter;

                    z: 10;
                    checkState: packageTreeItemDelegate.featureData.m_isPermission ? Qt.Checked : Qt.Unchecked;
                    isActive: packageTreeItemDelegate.root ? !packageTreeItemDelegate.root.readOnly : true;
                    visible: !packageTreeItemDelegate.hasChild;

                    onClicked: {
                        packageTreeItemDelegate.featureData.m_isPermission = !packageTreeItemDelegate.featureData.m_isPermission;
                    }
                }
            }
        }
    }

    footerDelegate: Component { Column {
            id: childrenColumn;

            visible: packageTreeItemDelegate.isOpened;

            Repeater {
                id: childModelRepeater;
                model: packageTreeItemDelegate.itemData.m_subFeatures ? packageTreeItemDelegate.itemData.m_subFeatures: 0;
                delegate: packageTreeItemDelegate.root ? packageTreeItemDelegate.root.rowDelegate : null;
                onCountChanged: {
                    packageTreeItemDelegate.hasChild = count > 0;
                }

                onItemAdded: {
                    item.parentDelegate = packageTreeItemDelegate;
                    packageTreeItemDelegate.childrenDelegates.push(item);
                    item.modelIndex.parentIndex = packageTreeItemDelegate.modelIndex;
                    packageTreeItemDelegate.modelIndex.childModel.push(item.modelIndex);
                    item.modelIndex.treeModel = childModelRepeater.model;
                }

                onItemRemoved: {
                    let index = packageTreeItemDelegate.childrenDelegates.indexOf(item);
                    if (index > -1) {
                        packageTreeItemDelegate.childrenDelegates.splice(index, 1);
                    }

                    index = packageTreeItemDelegate.modelIndex.childModel.indexOf(item.modelIndex);
                    if (index > -1) {
                        packageTreeItemDelegate.modelIndex.childModel.splice(index, 1);
                    }
                }
            }
        }
    }
}
