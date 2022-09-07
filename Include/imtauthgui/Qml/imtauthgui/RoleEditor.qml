import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Item {
    id: roleEditorContainer;

    function updateGui(){
        console.log("RoleEditor updateGui");
        roleIdInput.text = documentModel.GetData("RoleId");
        roleNameInput.text = documentModel.GetData("RoleName");
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

                text: qsTr("Id");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextField {
                id: roleIdInput;

                width: parent.width;
                height: 30;

                placeHolderText: qsTr("Enter the role Id");

                onTextChanged: {
                    documentModel.SetData("RoleId", usernameInput.text);
                }
            }
            Text {
                id: titleRoleName;

                text: qsTr("Name");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            CustomTextField {
                id: roleNameInput;

                width: parent.width;
                height: 30;

                placeHolderText: qsTr("Enter the role name");

                onTextChanged: {
                    documentModel.SetData("RoleName", nameInput.text);
                }
            }

        }//Column bodyColumn
    }//Flickable
}//Container
