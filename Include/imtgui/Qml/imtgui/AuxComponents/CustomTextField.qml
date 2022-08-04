import QtQuick 2.0
import Acf 1.0;

Item {
    id: containerTextField;

    height: 40;

    property alias text: textField.text;

    property alias acceptableInput: textField.acceptableInput;

    property string placeHolderText;
    property string focusColor: Style.textSelected;

    property color borderColor: Style.iconColorOnSelected;

    property int radius: 0;
    property int textSize: 15;
    property int echoMode: TextInput.Normal;

    signal accepted();

    onFocusChanged: {
        console.log('CustomTextField onFocusChanged', containerTextField.focus);

//        if (focus){
//            textField.focus = focus;
//        }
    }

    function setMask(mask){
        textField.inputMask = mask;
    }

    function setValidator(validator){
        textField.validator = validator;
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

//        focus: true;

        onFocusChanged: {
            console.log("TextInput onFocusChanged", textField.focus)

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

            font.pixelSize: Style.fontSize_common;
            font.family: Style.fontFamily;

            //TODO -> Style
            color: "gray";

            visible: textField.text == "";
        }
    }
}
