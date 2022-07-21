import QtQuick 2.0
import Acf 1.0;

Item {
    id: containerTextField;

    height: 40;

    property alias text: textField.text;

    property string placeHolderText;
    property string focusColor: Style.textSelected;

    property bool canReduce: false;
    property bool reduced: false;
    property bool isTextChanged: false;
    property bool wasFocus: false;
    property bool enabled:  true;
    property bool correctData: true;

    property color borderColor: Style.iconColorOnSelected;

    property int radius: 0;
    property int textSize: 15;
    property int echoMode: TextInput.Normal;

//    property string regexMask: "/([0-9A-F]{2}[:-]){5}([0-9A-F]{2})/";

    signal inputTextChanged();

    signal textInputFocusChanged();
    signal clicked();


    onFocusChanged: {
        console.log('TextFieldCustom onFocusChanged', containerTextField.focus);
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

        echoMode: containerTextField.echoMode;

        verticalAlignment: TextInput.AlignVCenter;
        selectionColor: Style.textSelected;
        selectByMouse: true;
        clip: true;

        onFocusChanged: {
            console.log("TextInput onFocusChanged", textField.focus);
            containerTextField.wasFocus = true;

            if (textField.focus){
                containerTextField.textInputFocusChanged();
            }
        }

        onTextChanged: {
            console.log("TextFieldCustom onTextChanged", textField.text);
            containerTextField.inputTextChanged();
            containerTextField.isTextChanged = true;
        }

        onAccepted: {
            console.log("TextFieldCustom onAccepted");
            containerTextField.isTextChanged = false;
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
