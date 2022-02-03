import QtQuick 2.0
import Acf 1.0;

Item {
    id: container;
    height: 40;

    signal accepted();

    property string focusColor: Style.textSelected;
    property bool enabled:  true;

    property bool correctData: true;

    property bool isFocus: textField.focus;

    property alias text: textField.text;
    property alias textField: textField;
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
        border.color: textField.focus ? container.borderColor : Style.hover;
        border.width: 1;
    }

    Timer {
        id: timer;
        interval: 3000;
        onTriggered: {
            console.log("onTriggered",container.isTextChanged);
            if (container.isTextChanged === true){
                container.accepted();
                container.isTextChanged = false;
            }
        }
    }

    TextInput {
        id: textField;
        anchors.fill: parent;
        anchors.leftMargin: 5;
        anchors.rightMargin: 5;

        color: Style.textColor;
        font.pixelSize: container.textSize;
        text: "";

        verticalAlignment: TextInput.AlignVCenter;
//        selectionColor: Style.textSelected;
        selectByMouse: true;
        clip: true;

//        onFocusChanged: {
//            container.focusChanged();
//            container.wasFocus = true;
//        }

        onTextChanged: {
            console.log("TextFieldCustom onTextChanged");

            if (container.wasFocus) {
               container.inputTextChanged();
            }

            container.isTextChanged = true;
            timer.restart();
        }

        onAccepted: {
            console.log("TextFieldCustom onAccepted");
            container.accepted();
            container.isTextChanged = false;
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
