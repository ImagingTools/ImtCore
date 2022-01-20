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

    signal focusChanged();

    onVisibleChanged: {
        if(!visible)
        {
           // textField.focus = false;
        }
    }

    Rectangle {
        id: mainRect;
        anchors.fill: parent;
        color: Style.baseColor;
        border.color: textField.focus ? "#00BFFF" : "transparent";
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
        width: parent.width - 20;
        color: Style.textColor;
        font.pixelSize: 12;
        focus: false;
        text: "";
        verticalAlignment: TextInput.AlignVCenter;
        selectByMouse: true;
        selectionColor: Style.textSelected;
        onFocusChanged: {
            container.focusChanged();
        }

        onTextChanged: {
            console.log("TextFieldCustom onTextChanged");
            container.isTextChanged = true;
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

    Rectangle{
        id:bottomBlue;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.bottom: parent.bottom;
        height: 3;
        Behavior on width {
            NumberAnimation { duration: container.animDuration;}
        }
        color: container.focusColor;
    }

}
