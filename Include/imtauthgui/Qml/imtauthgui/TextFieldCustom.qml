import QtQuick 2.0
import Acf 1.0;

Item {
    id: containerTextField;

    height: 40;

    //signal accepted();

    property string focusColor: Style.textSelected;
    property bool enabled:  true;

    property bool correctData: true;

//    property bool isFocus: textField.focus;
   // property string text;
    property alias text: textField.text;
//    property alias textField: textField;

    property bool isTextChanged: false;
    property string placeHolderText: "Наименование";

    property bool canReduce: false;
    property bool reduced: false;

    property bool wasFocus: false;

    property color borderColor: Style.iconColorOnSelected;

    property int textSize: 15;

    signal focusChanged();
    signal inputTextChanged();

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
        //text: containerTextField.text;
        text: "";

        verticalAlignment: TextInput.AlignVCenter;
        selectionColor: Style.textSelected;
        selectByMouse: true;
        clip: true;

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
