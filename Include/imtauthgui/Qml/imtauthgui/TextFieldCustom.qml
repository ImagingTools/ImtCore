import QtQuick 2.0
import Acf 1.0;

Item {
    id: container;
    height: 40;

    signal accepted();

    property string focusColor: Style.textSelected;
    property bool enabled:  true;

    property alias text: textField.text;
    property bool isTextChanged: false;
    property string placeHolderText: "Наименование";

    property bool canReduce: false;
    property bool reduced: false;

    property color borderColor: "transparent";

    signal focusChanged();
//    signal textChanged();

    Rectangle {
        id: mainRect;
        anchors.fill: parent;
        color: Style.baseColor;
        border.color: textField.focus ? "#00BFFF" : Style.hover;
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

        color: Style.textColor;
        font.pixelSize: 12;
        text: "";

        verticalAlignment: TextInput.AlignVCenter;
        selectionColor: Style.textSelected;
        selectByMouse: true;
        clip: true;

        onFocusChanged: {
            container.focusChanged();
        }

        onTextChanged: {
            console.log("TextFieldCustom onTextChanged");
            container.isTextChanged = true;
            container.textChanged();
            timer.restart();
        }

        onAccepted: {
            console.log("TextFieldCustom onAccepted");
            container.accepted();
            container.isTextChanged = false;
        }
    }

    Text {
        id: placeHolder;
        anchors.left: parent.left;
        anchors.leftMargin: 5;
        anchors.bottom: parent.bottom;
    }

//    Rectangle{
//        id:bottomBlue;
//        anchors.horizontalCenter: parent.horizontalCenter;
//        anchors.bottom: parent.bottom;
//        height: 3;
//        Behavior on width {
//            NumberAnimation { duration: container.animDuration;}
//        }
//        color: container.focusColor;
//    }

}
