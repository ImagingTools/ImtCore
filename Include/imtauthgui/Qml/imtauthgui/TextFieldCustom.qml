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

    signal focusChanged();
    signal inputTextChanged();

    function setFocus(){
//        textField.activeFocusOnPress = true;

        textField.forceActiveFocus();
//        textField.focus = true;
//        textField.selectAll();

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
        //text: containerTextField.text;
       // text: "";

        verticalAlignment: TextInput.AlignVCenter;
        selectionColor: Style.textSelected;
        selectByMouse: true;
        clip: true;

//        focus: false;

        onFocusChanged: {
            containerTextField.focusChanged();
            containerTextField.wasFocus = true;
        }

        onTextChanged: {
            console.log("TextFieldCustom onTextChanged");

            if (containerTextField.wasFocus) {
               containerTextField.inputTextChanged();
            }

            containerTextField.isTextChanged = true;
            //timer.restart();
        }

        onAccepted: {
            console.log("TextFieldCustom onAccepted");
            //containerTextField.accepted();
            containerTextField.isTextChanged = false;
        }

        onCursorVisibleChanged: {
            if (!cursorVisible) {
                cursorVisible = true;
            }
        }
    }

    Text {
        id: placeHolder;
        anchors.left: parent.left;
        anchors.leftMargin: 5;
        anchors.bottom: parent.bottom;
    }
}
