import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Item {
    id: inputDialogBodyContainer;

    height: columnBody.height + 40;

    property string message;
    property string inputValue: "";
    property Item rootItem: null;
    property alias placeHolderText: inputField.placeHolderText;

    Component.onCompleted: {
        inputField.forceActiveFocus();
    }

    Column {
        id: columnBody;

        anchors.verticalCenter: parent.verticalCenter;
        anchors.right: parent.right;
        anchors.rightMargin: Style.sizeMainMargin;
        anchors.left: parent.left;
        anchors.leftMargin: Style.sizeMainMargin;

        spacing: 5;

        Text {
            id: message;

            width: columnBody.width;

            text: inputDialogBodyContainer.message;
            color: Style.textColor;
            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSizeNormal;
            elide: Text.ElideRight;

            wrapMode: Text.WordWrap;
        }

        CustomTextField {
            id: inputField;

            anchors.horizontalCenter: parent.horizontalCenter;

            width: columnBody.width;
            height: 30;

            text: inputDialogBodyContainer.inputValue;

            onTextChanged: {
                inputDialogBodyContainer.inputValue = inputField.text;
            }

            onAccepted: {
                inputDialogBodyContainer.rootItem.buttons.buttonClicked(Enums.ok);
            }
        }
    }
}

