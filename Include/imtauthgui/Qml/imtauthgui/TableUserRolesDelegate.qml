import QtQuick 2.0
import imtgui 1.0
import Acf 1.0

TableViewItemDelegateBase {
    id: productRolesDelegate;

    height: root.rowItemHeight + footerItem.height;

    root: rolesTable;

    highlightDelegate: Rectangle {
        id: highlight;

        width: parent.width;

        color: Style.alternateBaseColor;

        border.width: 1;
        border.color: Style.imagingToolsGradient2;
    }

    rowBodyDelegate: Component { Row {
        height: root.rowItemHeight;
        width: parent.width;

        Item {
            height: root.rowItemHeight;
            width: parent.width;

            Text {
                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 10;

                text: model.Name;

                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamilyBold;
                font.bold: true;

                color: Style.textColor;
                wrapMode: Text.WordWrap;
                elide: Text.ElideRight;
            }
        }
    }
    }

    footerDelegate: Component { Column {
        id: rolesColumn;

        width: parent.width;

        property var rolesModel: model.Roles ? model.Roles : null;

        onRolesModelChanged: {
            console.log("onRolesModelChanged", rolesModel)
            if (rolesModel != null){
                rolesRepeater.model = rolesModel;
            }
        }

//        Rectangle {
//            anchors.fill: rolesRepeater;

//            border.width: 1;
//            border.color: Style.imagingToolsGradient2;
//        }

        Repeater {
            id: rolesRepeater;

            delegate: Rectangle {
                width: productRolesDelegate.width;
                height: root.rowItemHeight;

                border.width: 1;
                border.color: "transparent";

                CheckBox {
                    id: checkBoxRole;

                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.left: parent.left;
                    anchors.leftMargin: 10;

                    checkState: model.CheckState;

                    onClicked: {
                        console.log("checkBoxRole onClicked", model.CheckState)
                        model.CheckState = Qt.Checked - model.CheckState;

                        root.rowModelDataChanged(productRolesDelegate, "CheckState");
                    }
                }

                Text {
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.left: checkBoxRole.right;
                    anchors.leftMargin: 10;

                    text: model.Name;

                    font.family: Style.fontFamily;
                    font.pixelSize: Style.fontSize_common;
                    color: Style.textColor;
                    wrapMode: Text.WordWrap;
                    elide: Text.ElideRight;
                }
            }
        }
    }//Column
    }
}
