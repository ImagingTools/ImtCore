import QtQuick 2.0
import Acf 1.0;

Item {
    id: containerTextField;

    height: 40;

    property alias text: textField.text;

    property string placeHolderText: "Наименование";
    property string focusColor: Style.textSelected;

    property bool canReduce: false;
    property bool reduced: false;
    property bool isTextChanged: false;
    property bool wasFocus: false;
    property bool enabled:  true;
    property bool correctData: true;

    property color borderColor: Style.iconColorOnSelected;

    property int textSize: 15;
    property int echoMode: TextInput.Normal;

    signal inputTextChanged();

    signal textInputFocusChanged();
    signal clicked();


    onFocusChanged: {
        console.log('TextFieldCustom onFocusChanged', containerTextField.focus);
        if (!containerTextField.focus){
            containerTextField.setFocus(false);
        }
//        else{
//            containerTextField.setFocus(true);
//        }
    }

    function setFocus(state){
        if (state === true){
           textField.selectAll();
        }
        textField.focus = state;
    }

    Rectangle {
        id: mainRect;

        anchors.fill: parent;

        color: Style.baseColor;

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
            console.log("TextFieldCustom onTextChanged");
            if (containerTextField.wasFocus) {
                containerTextField.inputTextChanged();
            }

//            containerTextField.customTextChanged();
            containerTextField.isTextChanged = true;
        }

        onAccepted: {
            console.log("TextFieldCustom onAccepted");
            containerTextField.isTextChanged = false;
        }
    }

    Text {
        id: placeHolder;
        anchors.left: parent.left;
        anchors.leftMargin: 5;
        anchors.bottom: parent.bottom;
    }
}
