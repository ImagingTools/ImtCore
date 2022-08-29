import QtQuick 2.0
import Acf 1.0;

Item {
    id: containerTextField;

    height: 40;

    property alias text: textField.text;
    property alias acceptableInput: textField.acceptableInput;
    property alias textInputFocus: textField.focus;
    property alias textInputMask: textField.inputMask;
    property alias textInputValidator: textField.validator;

    property string placeHolderText;
    property string focusColor: Style.textSelected;

    property color borderColor: Style.iconColorOnSelected;

    property int radius: 0;
    property int textSize: 15;
    property int placeHolderTextSize: Style.fontSize_common;
    property int echoMode: TextInput.Normal;

    property alias borderWidth: mainRect.border.width;

    signal accepted();

    function setFocus(value) {
        textField.focus = value
    }

    onFocusChanged: {
        console.log("CustomTextField onFocusChanged", textField.text, containerTextField.focus)
    }

    Rectangle {
        id: mainRect;

        anchors.fill: parent;

        color: Style.baseColor;
        radius: containerTextField.radius;

        border.color: textField.focus ? containerTextField.borderColor : Style.hover;
        border.width: 1;
    }

    MouseArea {
        anchors.fill: textField;
        cursorShape: Qt.IBeamCursor;
    }

    TextInput {
        id: textField;

        anchors.fill: parent;
        anchors.leftMargin: 5;
        anchors.rightMargin: 5;

        color: Style.textColor;
        font.pixelSize: containerTextField.textSize;
        font.family: Style.fontFamily;

        echoMode: containerTextField.echoMode;

        verticalAlignment: TextInput.AlignVCenter;
        selectionColor: Style.textSelected;
        selectByMouse: true;
        clip: true;

        onFocusChanged: {
            console.log("CustomTextField onFocusChanged", textField.text, textField.focus)
        }

        onAccepted: {
            console.log("CustomTextField onAccepted");
            containerTextField.accepted();
        }

        Text {
            id: placeHolder;

            anchors.left: parent.left;
            anchors.verticalCenter: parent.verticalCenter;

            text: containerTextField.placeHolderText;

            font.pixelSize: containerTextField.placeHolderTextSize;
            font.family: Style.fontFamily;

            //TODO -> Style
            color: "gray";

            visible: textField.text == "";
        }
    }
}
