import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

Item {
    id: container;

    height: bodyColumn.height + 40;

    property string titleId;
    property string titleName;

    property string valueId;
    property string valueName;

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

            text: container.titleName;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        CustomTextField {
            id: inputName;

            width: bodyColumn.width;
            height: 30;

            text: container.valueName;

            onTextChanged: {
                container.valueName = inputName.text;
            }
        }


        Text {
            id: titleFeatureId;

            anchors.topMargin: 10;

            text: container.titleId;

            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
        }

        CustomTextField {
            id: inputId;

            width: bodyColumn.width;
            height: 30;

            text: container.valueId;

            onTextChanged: {
                container.valueId = inputId.text;
            }
        }
    }
}
