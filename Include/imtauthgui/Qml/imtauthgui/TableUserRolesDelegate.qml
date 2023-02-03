import QtQuick 2.0
import imtgui 1.0
import Acf 1.0

TableViewItemDelegateBase {
    id: productRolesDelegate;

    height: root.rowItemHeight + footerItem.height;

    //root: rolesTable;

    highlightDelegate: Rectangle {
        id: highlight;

        width: parent.width;

        color: Style.alternateBaseColor;

        border.width: 1;
        border.color: Style.imagingToolsGradient2;
    }

    rowBodyDelegate: Component { Row {
            height: productRolesDelegate.root ? productRolesDelegate.root.rowItemHeight : 0;
            width: parent.width;

            Item {
                height: productRolesDelegate.root ? productRolesDelegate.root.rowItemHeight : 0;
                width: parent.width;

                Text {
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.left: parent.left;
                    anchors.leftMargin: 10;

                    font.pixelSize: Style.fontSize_common;
                    font.family: Style.fontFamilyBold;
                    font.bold: true;

                    color: Style.textColor;
                    wrapMode: Text.WordWrap;
                    elide: Text.ElideRight;

                    text: model.Name;
                }
            }
        }
    }

    footerDelegate: Component { Column {
            id: rolesColumn;

            width: parent.width;

            property var rolesModel: model.Roles ? model.Roles : null;

            onRolesModelChanged: {
                console.log("onRolesModelChanged", rolesColumn.rolesModel)
                if (rolesModel != null){
                    rolesRepeater.model = rolesColumn.rolesModel;
                }
            }

            //        Rectangle {
            //            anchors.fill: rolesRepeater;

            //            border.width: 1;
            //            border.color: Style.imagingToolsGradient2;
            //        }

            ListView {
                id: rolesRepeater;

                width: parent.width;
                height: productRolesDelegate.root ? lesRepeater.count * productRolesDelegate.root.rowItemHeight : 0;

                spacing: -1;

                delegate: Rectangle {
                    width: productRolesDelegate.width;
                    height: productRolesDelegate.root ? productRolesDelegate.root.rowItemHeight : 0;

                    border.width: 1;
                    border.color: Style.borderColor;

                    color: Style.baseColor;

                    CheckBox {
                        id: checkBoxRole;

                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.left: parent.left;
                        anchors.leftMargin: 10;

                        checkState: model.CheckState;

                        onClicked: {
                            console.log("checkBoxRole onClicked", model.CheckState)
                            model.CheckState = Qt.Checked - model.CheckState;

                            productRolesDelegate.root.rowModelDataChanged(productRolesDelegate, "CheckState");
                        }
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.left: checkBoxRole.right;
                        anchors.leftMargin: 10;

                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSize_common;
                        color: Style.textColor;
                        wrapMode: Text.WordWrap;
                        elide: Text.ElideRight;

                        text: model.Name;
                    }
                }
            }

//            Repeater {
//                id: rolesRepeater;

//                delegate: Rectangle {
//                    width: productRolesDelegate.width;
//                    height: productRolesDelegate.root ? productRolesDelegateroot.rowItemHeight : 0;

//                    border.width: 1;
//                    border.color: Style.borderColor;

//                    color: Style.baseColor;

//                    CheckBox {
//                        id: checkBoxRole;

//                        anchors.verticalCenter: parent.verticalCenter;
//                        anchors.left: parent.left;
//                        anchors.leftMargin: 10;

//                        checkState: model.CheckState;

//                        onClicked: {
//                            console.log("checkBoxRole onClicked", model.CheckState)
//                            model.CheckState = Qt.Checked - model.CheckState;

//                            productRolesDelegate.root.rowModelDataChanged(productRolesDelegate, "CheckState");
//                        }
//                    }

//                    Text {
//                        anchors.verticalCenter: parent.verticalCenter;
//                        anchors.left: checkBoxRole.right;
//                        anchors.leftMargin: 10;

//                        font.family: Style.fontFamily;
//                        font.pixelSize: Style.fontSize_common;
//                        color: Style.textColor;
//                        wrapMode: Text.WordWrap;
//                        elide: Text.ElideRight;

//                        text: model.Name;
//                    }
//                }
//            }
        }//Column
    }
}
