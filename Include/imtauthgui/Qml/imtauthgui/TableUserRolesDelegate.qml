import QtQuick 2.0
import imtgui 1.0
import Acf 1.0

Item {
    id: productRolesDelegate;

    height: body.height + rolesColumn.height;

    property int index: model.index;

    property int selectedIndex: -1;

    Component.onCompleted: {
        console.log("productRolesDelegate onCompleted", model.Roles);
        if (model.Roles){
            rolesRepeater.model = model.Roles;
        }
    }

    onSelectedIndexChanged: {
        if (selectedIndex != index){
            rolesRepeater.selectedIndex = -1;
        }
    }

    Rectangle {
        id: body;

        width: productRolesDelegate.width;
        height: 30;

        color: Style.alternateBaseColor;

        border.width: 1;
        border.color: Style.imagingToolsGradient2;

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

    Rectangle {
        id: rolesBg;

        anchors.top: body.bottom;

        color: "transparent";

        width: productRolesDelegate.width;
        height: rolesColumn.height;

        border.color: Style.imagingToolsGradient2;
        border.width: 1;

        Column {
            id: rolesColumn;

            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.verticalCenter: parent.verticalCenter;

            width: rolesBg.width - 2;

            Repeater {
                id: rolesRepeater;

                property int selectedIndex: -1;

                onSelectedIndexChanged: {
                    if (productRolesDelegate.selectedIndex == index){
                        baseCollectionView.selectedIndexChanged(selectedIndex);
                    }
                }

                delegate: Rectangle {
                    width: productRolesDelegate.width;
                    height: body.height;

                    color: rolesRepeater.selectedIndex == model.index ? Style.selectedColor : "transparent";

                    CheckBox {
                        id: checkBoxRole;

                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.left: parent.left;
                        anchors.leftMargin: 10;

                        checkState: model.State;

                        MouseArea {
                            anchors.fill: parent;

                            onClicked: {
                                model.State = 2 - checkBoxRole.checkState;
                            }
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

