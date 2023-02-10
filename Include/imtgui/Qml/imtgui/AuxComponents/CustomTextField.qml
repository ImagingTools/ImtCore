import QtQuick 2.15
import Acf 1.0;

FocusScope {
    id: containerTextField;

    height: 40;

    property alias text: textField.text;
    property alias acceptableInput: textField.acceptableInput;
    property alias textInputFocus: textField.focus;
    property alias textInputMask: textField.inputMask;
    property alias textInputValidator: textField.validator;
    property alias readOnly: textField.readOnly;
    property alias horizontalAlignment: textField.horizontalAlignment;
    property alias borderWidth: mainRect.border.width;
    property alias color: mainRect.color;

    property alias maximumLength: textField.maximumLength;

    property string placeHolderText;
    property string focusColor: Style.textSelected;

    property string fontColor: Style.textColor;
    property string placeHolderFontColor: "gray";

    property color borderColor: textField.acceptableInput ? Style.iconColorOnSelected : Style.errorTextColor;
    property string borderColorConst: "";

    property int radius: 0;
    property int textSize: 15;
    property int placeHolderTextSize: textSize;
    property int echoMode: TextInput.Normal;
    property int margin: 5;

    property bool fontBold: false;

    property real placeHolderOpacity: 1;

    signal accepted();
    signal cancelled();
    signal textEdited();
    signal editingFinished();

    Keys.onPressed: {
        if (containerTextField.activeFocus){
            console.log("CustomTextField onPressed");

            if (event.key == Qt.Key_Escape) {
                console.log("Key_Escape");
                containerTextField.cancelled();
            }
        }
    }

    function setFocus(value) {
        textField.focus = value;
    }

    onFocusChanged: {
        console.log("CustomTextField onFocusChanged", textField.text, containerTextField.focus);

        if (containerTextField.focus){
            if (!containerTextField.readOnly){
                textField.selectAll();
            }

            textField.forceActiveFocus();
        }
    }

    Rectangle {
        id: mainRect;

        anchors.fill: parent;

        color: containerTextField.readOnly ? Style.alternateBaseColor : Style.baseColor;

        radius: containerTextField.radius;

        border.color: containerTextField.borderColorConst !== "" ? containerTextField.borderColorConst: textField.activeFocus ? containerTextField.borderColor : Style.hover;
        border.width: 1;
    }

    MouseArea {
        id: mouseArea;

        anchors.fill: textField;
        cursorShape: containerTextField.readOnly ? Qt.ArrowCursor : Qt.IBeamCursor;
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

        onEditingFinished: {
            containerTextField.editingFinished();
        }

        Text {
            id: placeHolder;

            anchors.left: parent.left;
            anchors.verticalCenter: parent.verticalCenter;

            font.pixelSize: containerTextField.placeHolderTextSize;
            font.bold: containerTextField.fontBold;
            font.family: Style.fontFamily;

            //TODO -> Style
            color: containerTextField.placeHolderFontColor;
            opacity: containerTextField.placeHolderOpacity;

            visible: textField.text == "";

            text: containerTextField.placeHolderText;
        }
    }
}
