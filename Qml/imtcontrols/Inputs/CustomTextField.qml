import QtQuick 2.12
import Acf 1.0;

FocusScope {
    id: containerTextField;

    height: 40;

//    property alias text: textField.text;
    property string text: textField.text; // for web TEMP!!!
    property alias acceptableInput: textField.acceptableInput;
    property alias textInputFocus: textField.focus;
    property alias textInputMask: textField.inputMask;
    property alias textInputValidator: textField.validator;
    property alias readOnly: textField.readOnly;
    property alias horizontalAlignment: textField.horizontalAlignment;
    property alias borderWidth: mainRect.border.width;
    property alias color: mainRect.color;
    property alias leftPadding: textField.leftPadding;

    property alias maximumLength: textField.maximumLength;
    property alias selectionStart: textField.selectionStart;
    property alias selectionEnd: textField.selectionEnd;

    property string placeHolderText;
    property string focusColor: Style.textSelected;

    property string fontColor: Style.textColor;
	property alias fontFamily: textField.font.family;
    property string placeHolderFontColor: "#808094";

    property color borderColor: containerTextField.acceptableInput ? Style.iconColorOnSelected : Style.errorTextColor;

    property string borderColorConst: "";

    property int radius: Style.textFieldRadius;
    property int textSize: Style.fontSize_common;
    property int placeHolderTextSize: textSize;
    property int echoMode: TextInput.Normal;
    property int margin: Style.size_mainMargin;

    property int textFieldLeftMargin: margin;
    property int textFieldRightMargin: margin;

    property bool fontBold: false;
    property bool hasActionMenu: true;
    property bool autoEditingFinished: true;

    property real placeHolderOpacity: 1;

    signal accepted();
    signal cancelled();
    signal textEdited();
    signal editingFinished();

    Keys.onPressed: {
        if (containerTextField.activeFocus){

            if (event.key === Qt.Key_Escape) {
                containerTextField.cancelled();
            }
        }
    }

    onTextChanged: {
        textField.text = text;// for web TEMP!!!
    }

    function select(from, to) {
        textField.select(from, to)
    }

    function setFocus(value) {
        textField.focus = value;
    }

    function ensureVisible(pos) {
        textField.ensureVisible(pos);
    }

    onFocusChanged: {
        if (containerTextField.focus){
            if (!containerTextField.readOnly){
                textField.selectAll();
            }

            textField.focus = true;
            textField.forceActiveFocus();
        }
        else {
            textField.focus = false;
        }
    }

    Rectangle {
        id: mainRect;

        anchors.fill: parent;

        color: containerTextField.readOnly ? Style.alternateBaseColor : Style.baseColor;

        radius: containerTextField.radius;

        border.color: containerTextField.borderColorConst !== "" ?
                          containerTextField.borderColorConst: textField.activeFocus ?
                              containerTextField.borderColor : Style.borderColor;

        border.width: 1;
    }

    MouseArea {
        id: mouseArea;

        anchors.fill: textField;
        cursorShape: containerTextField.readOnly ? Qt.ArrowCursor : Qt.IBeamCursor;

        visible: !containerTextField.readOnly;

        acceptedButtons: Qt.RightButton;

        onClicked: {
            if (containerTextField.hasActionMenu){
                let point = mapToItem(null, mouse.x, mouse.y);

                inputActions.open(point.x, point.y);
            }
        }
    }

    InputActions {
        id: inputActions;

        onCopyClicked: {
            textField.copy();
        }

        onCutClicked: {
            textField.cut();

            containerTextField.editingFinished();
        }

        onPasteClicked: {
            let oldText = textField.text;
            textField.paste();
            let newText = textField.text;
            if (oldText !== newText){
                containerTextField.editingFinished();
            }
        }

        onRemoveClicked: {
            if (textField.selectedText !== ""){
                textField.remove(textField.selectionStart, textField.selectionEnd);

                containerTextField.editingFinished();
            }
        }

        onSelectAllClicked: {
            textField.selectAll();
        }
    }

    TextInput {
        id: textField;

        z: parent.z + 1;

        anchors.fill: parent;
        anchors.leftMargin: containerTextField.textFieldLeftMargin;
        anchors.rightMargin: containerTextField.textFieldRightMargin;

        color: containerTextField.fontColor;
        font.pixelSize: containerTextField.textSize;
        font.bold: containerTextField.fontBold;
        font.family: Style.fontFamily;

        echoMode: containerTextField.echoMode;

        verticalAlignment: TextInput.AlignVCenter;
        selectionColor: Style.textSelected;
        selectByMouse: true;
        clip: true;

        activeFocusOnPress: !containerTextField.readOnly;

        onAccepted: {
            containerTextField.accepted();
        }

        onTextChanged: {
            if(containerTextField.text !== text){// for web TEMP!!!
                containerTextField.text = text
            }
        }

        onTextEdited: {
            containerTextField.textEdited();

            editingFinishedTimer.restart();
        }

        onEditingFinished: {
            containerTextField.editingFinished();
        }

        Text {
            id: placeHolder;

            anchors.verticalCenter: textField.verticalCenter;
            anchors.left: textField.left;

            font.pixelSize: containerTextField.placeHolderTextSize;
            font.bold: containerTextField.fontBold;
            font.family: Style.fontFamily;

            //TODO -> Style
            color: containerTextField.placeHolderFontColor;
            opacity: containerTextField.placeHolderOpacity;

            visible: textField.text === "" && !containerTextField.readOnly;

            text: containerTextField.placeHolderText;
        }
    }

    Timer {
        id: editingFinishedTimer;
        interval: 700;

        onTriggered: {
            if (containerTextField.autoEditingFinished){
                containerTextField.editingFinished();
            }
        }
    }
}
