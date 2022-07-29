import QtQuick 2.0
import Acf 1.0;

Item {
    id: containerTextField;

    height: 40;

//    property string text: textField.text;
    property alias text: textField.text;

    property string placeHolderText;
    property string focusColor: Style.textSelected;

    property color borderColor: Style.iconColorOnSelected;

    property int radius: 0;
    property int textSize: 15;
    property int echoMode: TextInput.Normal;

//    property string regexMask: "/([0-9A-F]{2}[:-]){5}([0-9A-F]{2})/";
//    signal textChanged();
    signal accepted();

    onFocusChanged: {
        console.log('CustomTextField onFocusChanged', containerTextField.focus);
        containerTextField.setFocus(containerTextField.focus);
    }

    function setFocus(state){
        if (state === true){
           textField.selectAll();
        }
        textField.focus = state;
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

    TextInput {
        id: textField;

        anchors.fill: parent;
        anchors.leftMargin: 5;
        anchors.rightMargin: 5;

        color: Style.textColor;
        font.pixelSize: containerTextField.textSize;
        font.family: Style.fontFamily;

//        text: containerTextField.text;


        echoMode: containerTextField.echoMode;

        verticalAlignment: TextInput.AlignVCenter;
        selectionColor: Style.textSelected;
        selectByMouse: true;
        clip: true;

        onFocusChanged: {
            console.log("TextInput onFocusChanged", textField.focus)

        }

        onTextChanged: {
            console.log("CustomTextField onTextChanged", textField.text);
//            containerTextField.text = textField.text;
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

            color: "gray";

            visible: textField.text == "";
        }
    }
}
