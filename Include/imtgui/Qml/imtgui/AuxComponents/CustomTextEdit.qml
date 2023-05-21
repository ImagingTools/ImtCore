import QtQuick 2.15
import Acf 1.0;

FocusScope {
    id: containerTextEdit;

    height: 40;


    property bool isDinamicHeight: true;

    property alias text: textEdit.text;
    property alias textInputFocus: textEdit.focus;
    property alias readOnly: textEdit.readOnly;
    property alias horizontalAlignment: textEdit.horizontalAlignment;

    property alias borderWidth: mainRect.border.width;
    property alias color: mainRect.color;

    //    property alias maximumLength: textEdit.maximumLength;

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
    property int margin: 8;

    property bool fontBold: false;

    property real placeHolderOpacity: 1;

    signal accepted();
    signal cancelled();
    signal textEdited();
    signal editingFinished();

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
        textEdit.focus = value;
    }

    onFocusChanged: {
        console.log("CustomTextEdit onFocusChanged", textEdit.text, containerTextEdit.focus);

        if (containerTextEdit.focus){
            if (!containerTextEdit.readOnly){
                textEdit.selectAll();
            }

            textEdit.forceActiveFocus();
        }
    }

    Rectangle {
        id: mainRect;

        anchors.fill: parent;

        color: containerTextEdit.readOnly ? Style.alternateBaseColor : Style.baseColor;

        radius: containerTextEdit.radius;

        border.color: containerTextEdit.borderColorConst !== "" ? containerTextEdit.borderColorConst: textEdit.activeFocus ? containerTextEdit.borderColor : Style.hover;
        border.width: 1;
    }

    Flickable{
        id: flick;

        anchors.fill: mainRect;

        contentWidth: width;
        contentHeight: textEdit.height + 2*containerTextEdit.margin;
        clip: true;
        boundsBehavior: Flickable.StopAtBounds;

        function ensureVisible(r)
        {
            if (flick.contentY >= r.y)
                flick.contentY = r.y;
            else if (flick.contentY + flick.height  <= r.y + r.height + containerTextEdit.margin)
                flick.contentY = r.y + r.height - flick.height + 2*containerTextEdit.margin;
        }

        MouseArea {
            id: mouseArea;

            anchors.fill: textEdit;
//            cursorShape: containerTextEdit.readOnly ? Qt.ArrowCursor : Qt.IBeamCursor;
            acceptedButtons: Qt.RightButton;

            visible: !containerTextEdit.readOnly;

            onClicked: {
                let point = mapToItem(null, mouse.x, mouse.y);

                inputActions.open(point.x, point.y);
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

        TextEdit {
            id: textEdit;

            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.top: parent.top;
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

            //for web, do not remove!!!
//            onCursorRectangleChanged: {
//                if(!containerTextEdit.isDinamicHeight){
//                    flick.ensureVisible(textEdit.cursorRectangle);
//                }
//            }

            onHeightChanged: {
                if(containerTextEdit.isDinamicHeight){
                    var newHeigth = textEdit.height + 2*containerTextEdit.margin;
                    if(newHeigth > containerTextEdit.height){
                        containerTextEdit.height = newHeigth;
                    }

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

            Text {
                id: placeHolder;

                anchors.left: parent.left;
                anchors.verticalCenter: parent.verticalCenter;

                font.pixelSize: containerTextEdit.placeHolderTextSize;
                font.bold: containerTextEdit.fontBold;
                font.family: Style.fontFamily;

                //TODO -> Style
                color: containerTextEdit.placeHolderFontColor;
                opacity: containerTextEdit.placeHolderOpacity;

                visible: textEdit.text == "";

                text: containerTextEdit.placeHolderText;
            }
        }
    }

    Timer {
        id: editingFinishedTimer;
        interval: 1000;

        onTriggered: {
            containerTextEdit.editingFinished();
        }
    }
}
