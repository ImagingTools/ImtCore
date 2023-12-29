import QtQuick 2.12
import Acf 1.0;
import imtqml 1.0
import imtcontrols 1.0

ControlBase {
    id: containerTextArea;

    decoratorComponent: Style.isQtStyle ? DecoratorsQt.textAreaDecorator: Decorators.textAreaDecorator;

    property bool isDinamicHeight: true;

    property alias text: textArea.text;
    property alias textInputFocus: textArea.focus;
    property alias readOnly: textArea.readOnly;
    property alias horizontalAlignment: textArea.horizontalAlignment;
    //property alias maximumLength: textArea.maximumLength;

    //property alias borderWidth: mainRect.border.width;
    //property alias color: mainRect.color;

    property string placeHolderText;
    property string focusColor: Style.textSelected;

    property string fontColor: Style.textColor;
    property string placeHolderFontColor: "gray";

    property color borderColor:  Style.iconColorOnSelected;
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

    onDecoratorComponentChanged: {
        bindText.target = decorator

        textArea.setHeight();

        if(decorator.z < 0){
            if(focus){
                textArea.forceActiveFocus()
            }
        }
        else {
            decorator.focus = focus;
        }
    }

    Keys.onPressed: {
        if (containerTextArea.activeFocus){
            console.log("TextArea onPressed");

            if (event.key == Qt.Key_Escape) {
                console.log("Key_Escape");
                containerTextArea.cancelled();
            }
        }
    }

    function setFocus(value) {
        //textArea.focus = value;
        focus = value;
        if(decorator.z < 0){
            textArea.focus = value;
            if(value){
                textArea.forceActiveFocus()
            }
        }
        else {
            decorator.focus = value;
            if(value){
                decorator.forceActiveFocus()
            }
        }
    }

    function openInputAction(pointX, pointY){
        inputActions.open(pointX, pointY);
    }

    onFocusChanged: {
        console.log("TextArea onFocusChanged", textArea.text, containerTextArea.focus);

        if (containerTextArea.focus){
            if (!containerTextArea.readOnly){
                textArea.selectAll();
            }

            textArea.forceActiveFocus();
        }

        if(decorator && decorator.z >= 0){
            decorator.focus = focus;
        }
    }



    Binding {
        id: bindText
        property: "text"
        value: containerTextArea.text;
    }


    Timer {
        id: editingFinishedTimer;
        interval: 1000;

        onTriggered: {
            containerTextArea.editingFinished();
        }
    }

    InputActions {
        id: inputActions;

        onCopyClicked: {
            textArea.copy();
        }

        onCutClicked: {
            textArea.cut();

            containerTextArea.editingFinished();
        }

        onPasteClicked: {
            let oldText = textArea.text;
            textArea.paste();
            let newText = textArea.text;
            if (oldText !== newText){
                containerTextArea.editingFinished();
            }
        }

        onRemoveClicked: {
            if (textArea.selectedText !== ""){
                textArea.remove(textArea.selectionStart, textArea.selectionEnd);

                containerTextArea.editingFinished();
            }
        }

        onSelectAllClicked: {
            textArea.selectAll();
        }
    }


    Flickable{
        id: flick;

        anchors.fill: parent;

        contentWidth: width;
        contentHeight: textArea.paintedHeight + 2*containerTextArea.margin;
        clip: true;
        boundsBehavior: Flickable.StopAtBounds;

        function ensureVisible(r)
        {
            if (flick.contentY >= r.y)
                flick.contentY = r.y;
            else if (flick.contentY + flick.height  <= r.y + r.height + containerTextArea.margin)
                flick.contentY = r.y + r.height - flick.height + 2*containerTextArea.margin;
        }

        TextEdit {
            id: textArea;

            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.top: parent.top;

            anchors.bottom: parent.bottom;
            anchors.bottomMargin: containerTextArea.margin;

            anchors.topMargin: containerTextArea.margin;
            anchors.leftMargin: containerTextArea.margin;
            anchors.rightMargin: containerTextArea.margin;

            color: containerTextArea.fontColor;
            font.pixelSize: containerTextArea.textSize;
            font.bold: containerTextArea.fontBold;
            font.family: Style.fontFamily;


            verticalAlignment: TextInput.AlignVCenter;
            selectionColor: Style.textSelected;
            selectByMouse: true;
            wrapMode: TextEdit.WordWrap;
            clip: true;
            visible: containerTextArea.decorator.z < 0;


            //for web, do not remove!!!
            onCursorRectangleChanged: {
                if(!containerTextArea.isDinamicHeight){
                    flick.ensureVisible(textArea.cursorRectangle);
                }
            }

            onHeightChanged: {
                setHeight();

            }

            function setHeight () {
                if(containerTextArea.isDinamicHeight  && containerTextArea.decorator && containerTextArea.decorator.z < 0){
                    var newHeigth = textArea.height + 2*containerTextArea.margin;
                    if(newHeigth !== containerTextArea.height){
                        containerTextArea.height = newHeigth;
                    }

                }
                if(containerTextArea.decorator && containerTextArea.decorator.setHeight !== undefined){
                    containerTextArea.decorator.setHeight();
                }

            }

            onEditingFinished: {
                containerTextArea.editingFinished();
            }


            onTextChanged:  {
                containerTextArea.textEdited();

                editingFinishedTimer.restart();
                console.log("TextArea onTextEdited");
            }
        }
    }
}
