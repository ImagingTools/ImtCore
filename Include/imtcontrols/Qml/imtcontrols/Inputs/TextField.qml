import QtQuick 2.12
import Acf 1.0;

FocusScope {
    id: containerTextField;

    width: decorator ? decorator.width : 0
    height: decorator ? decorator.height : 0

    property Component decoratorComponent;
    property var decorator : null;

    property alias text: textField.text;
    property alias acceptableInput: textField.acceptableInput;
    property alias textInputFocus: textField.focus;
    property alias textInputMask: textField.inputMask;
    property alias textInputValidator: textField.validator;
    property alias readOnly: textField.readOnly;
    property alias horizontalAlignment: textField.horizontalAlignment;
    property alias maximumLength: textField.maximumLength;

    property string placeHolderText;
    property string focusColor: Style.textSelected;

    property string fontColor: Style.textColor;
    property string placeHolderFontColor: "gray";

    property color borderColor: textField.acceptableInput ? Style.iconColorOnSelected : Style.errorTextColor;

    property string borderColorConst: "";

    property int radius: 3;
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

        //textField.focus = value;
        focus = value;
        if(decorator.z < 0){
            textField.focus = value;
            if(value){
                textField.forceActiveFocus()
            }
        }
        else {
            decorator.focus = value;
            if(value){
                decorator.forceActiveFocus()
            }
        }
    }

    function ensureVisible(pos) {
        textField.ensureVisible(pos);
    }

    function openInputAction(pointX, pointY){
        inputActions.open(pointX, pointY);
    }

    onFocusChanged: {
        console.log("CustomTextField onFocusChanged", textField.text, containerTextField.focus);

        if (containerTextField.focus){
            if (!textField.readOnly){
                textField.selectAll();
            }

            textField.focus = true;
            textField.forceActiveFocus();
        }
    }


    onDecoratorComponentChanged: {
        if (decoratorComponent){

            if(decorator){
                decorator.destroy()
            }
            decorator = decoratorComponent.createObject(containerTextField)
            decorator.baseElement = containerTextField
            bindWidth.target = decorator
            bindHeight.target = decorator
            bindText.target = decorator
        }

//        if(decorator.z < 0){
//            textField.focus = true;
//            textField.forceActiveFocus()
//        }
//        else {
//            decorator.focus = true;
//            decorator.forceActiveFocus()
//        }

    }

    Binding {
        id: bindWidth
        property: "width"
        value: containerTextField.width;
    }

    Binding {
        id: bindHeight
        property: "height"
        value: containerTextField.height;
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
        visible: containerTextField.decorator.z < 0;

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
