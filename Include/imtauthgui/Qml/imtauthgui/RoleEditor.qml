import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: roleEditorContainer;

    function updateGui(){
        console.log("RoleEditor updateGui");
        roleIdInput.text = documentModel.GetData("Id");
        roleNameInput.text = documentModel.GetData("Name");
    }

    Flickable {
        anchors.fill: parent;
        anchors.leftMargin: 20;

        contentWidth: bodyColumn.width;
        contentHeight: bodyColumn.height + 50;

        boundsBehavior: Flickable.StopAtBounds;

        Column {
            id: bodyColumn;

            width: 500;

            spacing: 7;

            Text {
                id: titleRoleId;

                text: qsTr("Role-ID");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextField {
                id: roleIdInput;

                width: parent.width;
                height: 30;

                placeHolderText: qsTr("Enter the Role-ID");

                onTextChanged: {
                    documentModel.SetData("Id", roleIdInput.text);
                }
            }

            Text {
                id: titleRoleName;

                text: qsTr("Role Name");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextField {
                id: roleNameInput;

                width: parent.width;
                height: 30;

                placeHolderText: qsTr("Enter the Role name");

                onTextChanged: {
                    documentModel.SetData("Name", roleNameInput.text);
                }
            }
        }//Column bodyColumn
    }//Flickable
}//Container
