import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

FocusScope {
    id: container;

    height: bodyColumn.height + 40;

    property Item rootItem: null;

    onFocusChanged: {
        console.log("InputBody onFocusChanged", container.focus);

        if (container.focus){
            inputName.focus = container.focus;
        }
    }

    Column {
        id: bodyColumn;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: parent.right;
        anchors.left: parent.left;
        anchors.rightMargin: 10;
        anchors.leftMargin: 10;

        width: container.width;

        spacing: 5;

        Text {
            id: titleName;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            text: qsTr("Group Name");
        }

        CustomTextField {
            id: inputName;

            width: bodyColumn.width;
            height: 30;

            text: container.rootItem.groupName;

            KeyNavigation.tab: descriptionInput;

            onTextChanged: {
                container.rootItem.groupName = inputName.text;
            }
        }

        Text {
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;

            text: qsTr("Group Description");
        }

        CustomTextField {
            id: descriptionInput;

            width: bodyColumn.width;
            height: 30;

            text: container.rootItem.groupDescription;

            onTextChanged: {
                container.rootItem.groupDescription = descriptionInput.text;
            }

            KeyNavigation.tab: inputName;
        }
    }
}
