import QtQuick 2.12
import Acf 1.0;
import imtqml 1.0
import imtcontrols 1.0

ControlBase {
    id: containerTextEdit;

    decoratorComponent: Style.isQtStyle ? DecoratorsQt.textEditDecorator: Decorators.textEditDecorator;

    property bool isDinamicHeight: true;

    property alias text: textEdit.text;
    property alias textInputFocus: textEdit.focus;
    property alias readOnly: textEdit.readOnly;
    property alias horizontalAlignment: textEdit.horizontalAlignment;
    //property alias maximumLength: textEdit.maximumLength;

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

        textEdit.setHeight();

        if(decorator.z < 0){
            if(focus){
                textEdit.forceActiveFocus()
            }
        }
        else {
            decorator.focus = focus;
        }
    }

    Keys.onPressed: {
        if (containerTextEdit.activeFocus){
            console.log("CustomTextEdit onPressed");

            if (event.key == Qt.Key_Escape) {
                console.log("Key_Escape");
                containerTextEdit.cancelled();
            }
        }
    }

    function setFocus(value) {
        //textEdit.focus = value;
        focus = value;
        if(decorator.z < 0){
            textEdit.focus = value;
            if(value){
                textEdit.forceActiveFocus()
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
        console.log("CustomTextEdit onFocusChanged", textEdit.text, containerTextEdit.focus);

        if (containerTextEdit.focus){
            if (!containerTextEdit.readOnly){
                textEdit.selectAll();
            }

            textEdit.forceActiveFocus();
        }

        if(decorator && decorator.z >= 0){
            decorator.focus = focus;
        }
    }



    Binding {
        id: bindText
        property: "text"
        value: containerTextEdit.text;
    }


    Timer {
        id: editingFinishedTimer;
        interval: 1000;

        onTriggered: {
            containerTextEdit.editingFinished();
        }
    }

    InputActions {
        id: inputActions;

        onCopyClicked: {
            textEdit.copy();
        }

        onCutClicked: {
            textEdit.cut();

            containerTextEdit.editingFinished();
        }

        onPasteClicked: {
            let oldText = textEdit.text;
            textEdit.paste();
            let newText = textEdit.text;
            if (oldText !== newText){
                containerTextEdit.editingFinished();
            }
        }

        onRemoveClicked: {
            if (textEdit.selectedText !== ""){
                textEdit.remove(textEdit.selectionStart, textEdit.selectionEnd);

                containerTextEdit.editingFinished();
            }
        }

        onSelectAllClicked: {
            textEdit.selectAll();
        }
    }


    Flickable{
        id: flick;

        anchors.fill: parent;

        contentWidth: width;
        contentHeight: textEdit.paintedHeight + 2*containerTextEdit.margin;
        clip: true;
        boundsBehavior: Flickable.StopAtBounds;

        function ensureVisible(r)
        {
            if (flick.contentY >= r.y)
                flick.contentY = r.y;
            else if (flick.contentY + flick.height  <= r.y + r.height + containerTextEdit.margin)
                flick.contentY = r.y + r.height - flick.height + 2*containerTextEdit.margin;
        }

        TextEdit {
            id: textEdit;

            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.top: parent.top;

            anchors.bottom: parent.bottom;
            anchors.bottomMargin: containerTextEdit.margin;

            anchors.topMargin: containerTextEdit.margin;
            anchors.leftMargin: containerTextEdit.margin;
            anchors.rightMargin: containerTextEdit.margin;

            color: containerTextEdit.fontColor;
            font.pixelSize: containerTextEdit.textSize;
            font.bold: containerTextEdit.fontBold;
            font.family: Style.fontFamily;


            verticalAlignment: TextInput.AlignVCenter;
            selectionColor: Style.textSelected;
            selectByMouse: true;
            wrapMode: TextEdit.WordWrap;
            clip: true;
            visible: containerTextEdit.decorator.z < 0;


            //for web, do not remove!!!
            onCursorRectangleChanged: {
                if(!containerTextEdit.isDinamicHeight){
                    flick.ensureVisible(textEdit.cursorRectangle);
                }
            }

            onHeightChanged: {
                setHeight();

            }

            function setHeight () {
                if(containerTextEdit.isDinamicHeight  && containerTextEdit.decorator && containerTextEdit.decorator.z < 0){
                    var newHeigth = textEdit.height + 2*containerTextEdit.margin;
                    if(newHeigth !== containerTextEdit.height){
                        containerTextEdit.height = newHeigth;
                    }

                }
                if(containerTextEdit.decorator){
                    containerTextEdit.decorator.setHeight();
                }

            }

            onEditingFinished: {
                containerTextEdit.editingFinished();
            }


            onTextChanged:  {
                containerTextEdit.textEdited();

                editingFinishedTimer.restart();
                console.log("CustomTextEdit onTextEdited");
            }
        }
    }
}
