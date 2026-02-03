import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0;
import imtcontrols 1.0

ControlBase {
    id: containerTextField;

    decorator: Style.textFieldDecorator

    property alias text: textField.text;
    property alias acceptableInput: textField.acceptableInput;
    property alias textInputFocus: textField.focus;
    property alias textInputMask: textField.inputMask;
    property alias textInputValidator: textField.validator;
    property alias readOnly: textField.readOnly;
    property alias horizontalAlignment: textField.horizontalAlignment;
    property alias maximumLength: textField.maximumLength;

    property string placeHolderText;
    property string focusColor: Style.textSelectedColor;

    property string fontColor: Style.textColor;
    property string placeHolderFontColor: "gray";

    property color borderColor: textField.acceptableInput ? Style.iconColorOnSelected : Style.errorTextColor;

    property string borderColorConst: "";

	property int radius: Style.radiusS;
	property int textSize: Style.fontSizeL;
    property int placeHolderTextSize: textSize;
    property int echoMode: TextInput.Normal;
	property int margin: Style.marginXS;

    property bool fontBold: false;

    property real placeHolderOpacity: 1;

    signal accepted();
    signal cancelled();
    signal textEdited();
    signal editingFinished();

    onDecoratorChanged: {
        bindText.target = decorator_

        if(decorator_.z < 0){
            if(focus){
                textField.forceActiveFocus()
            }
        }
        else {
            decorator_.focus = focus;
        }
    }

    Keys.onPressed: {
        if (containerTextField.activeFocus){
            if (event.key == Qt.Key_Escape){
                console.log("Key_Escape");
                containerTextField.cancelled();
            }
        }
    }

    function setFocus(value){

        //textField.focus = value;
        focus = value;
        if(decorator_.z < 0){
            textField.focus = value;
            if(value){
                textField.forceActiveFocus()
            }
        }
        else {
            decorator_.focus = value;
            if(value){
                decorator_.forceActiveFocus()
            }
        }
    }

    function ensureVisible(pos){
        textField.ensureVisible(pos);
    }

    function openInputAction(pointX, pointY){
        inputActions.open(pointX, pointY);
    }

    onFocusChanged: {
        if (containerTextField.focus){
            if (!textField.readOnly){
                textField.selectAll();
            }

            textField.focus = true;
            textField.forceActiveFocus();
        }

        if(decorator_ && decorator_.z >= 0){
            decorator_.focus = focus;
        }

    }

    Binding {
        id: bindText
        property: "text"
        value: containerTextField.text;
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
        objectName: "TextInput"

        anchors.fill: parent;
        anchors.leftMargin: containerTextField.margin;
        anchors.rightMargin: containerTextField.margin;

        color: containerTextField.fontColor;
        font.pixelSize: containerTextField.textSize;
        font.bold: containerTextField.fontBold;
        font.family: Style.fontFamily;

        echoMode: containerTextField.echoMode;

        verticalAlignment: TextInput.AlignVCenter;
        selectionColor: Style.textSelectedColor;
        selectByMouse: true;
        clip: true;
        visible: containerTextField.decorator_.z < 0;

        onAccepted: {
            containerTextField.accepted();
        }

        onTextEdited: {
            containerTextField.textEdited();
        }

        onAcceptableInputChanged: {
            if (textField.acceptableInput){
                containerTextField.borderColor = Style.iconColorOnSelected;
            }
            else{
                containerTextField.borderColor = Style.errorTextColor;
            }
        }

        onEditingFinished: {
            containerTextField.editingFinished();
        }

    }
}
