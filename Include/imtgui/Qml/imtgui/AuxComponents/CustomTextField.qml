import QtQuick 2.15
import Acf 1.0;

Item {
    id: containerTextField;

    height: 40;

    focus: textField.activeFocus;

    property alias text: textField.text;
    property alias acceptableInput: textField.acceptableInput;
    property alias textInputFocus: textField.focus;
    property alias textInputMask: textField.inputMask;
    property alias textInputValidator: textField.validator;
    property alias readOnly: textField.readOnly;
    property alias horizontalAlignment: textField.horizontalAlignment;
    property alias borderWidth: mainRect.border.width;
    property alias color: mainRect.color;

    property string placeHolderText;
    property string focusColor: Style.textSelected;

    property string fontColor: Style.textColor;
    property string placeHolderFontColor: "gray";

    property color borderColor: Style.iconColorOnSelected;

    property int radius: 0;
    property int textSize: 15;
    property int placeHolderTextSize: textSize;
    property int echoMode: TextInput.Normal;
    property int margin: 5;

    property bool fontBold: false;

    property real placeHolderOpacity: 1;


    signal accepted();
    signal textEdited();

    function setFocus(value) {
        textField.focus = value
    }

    onFocusChanged: {
        console.log("CustomTextField onFocusChanged", textField.text, containerTextField.focus)

        if (focus){
            textField.selectAll();
            textField.forceActiveFocus();
        }
    }

    Rectangle {
        id: mainRect;

        anchors.fill: parent;

        color: Style.baseColor;
        radius: containerTextField.radius;

        border.color: textField.activeFocus ? containerTextField.borderColor : Style.hover;
        border.width: 1;
    }

    MouseArea {
        id: mouseArea;

        anchors.fill: textField;
        cursorShape: Qt.IBeamCursor;
    }

    TextInput {
        id: textField;

        anchors.fill: parent;
        anchors.leftMargin: containerTextField.margin;
        anchors.rightMargin: containerTextField.margin;

        color: containerTextField.fontColor;
        font.pixelSize: containerTextField.textSize;
        font.bold: containerTextField.fontBold;
        font.family: Style.fontFamily;

        echoMode: containerTextField.echoMode;

        verticalAlignment: TextInput.AlignVCenter;
        selectionColor: Style.textSelected;
        selectByMouse: true;
        clip: true;

        onAccepted: {
            console.log("CustomTextField onAccepted");
            containerTextField.accepted();
        }

        onTextEdited: {
            containerTextField.textEdited();
            console.log("CustomTextField onTextEdited");
        }

        Text {
            id: placeHolder;

            anchors.left: parent.left;
            anchors.verticalCenter: parent.verticalCenter;

            text: containerTextField.placeHolderText;

            font.pixelSize: containerTextField.placeHolderTextSize;
            font.bold: containerTextField.fontBold;
            font.family: Style.fontFamily;

            //TODO -> Style
            color: containerTextField.placeHolderFontColor;
            opacity: containerTextField.placeHolderOpacity;

            visible: textField.text == "";
        }
    }
}
