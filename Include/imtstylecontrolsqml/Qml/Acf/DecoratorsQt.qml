pragma Singleton
import QtQuick 2.12
import Acf 1.0
import QtQuick.Controls 2.15

Item {
    property Component buttonDecorator: buttonDecoratorComp
    property Component roundButtonDecorator: roundButtonDecoratorComp

    property Component textFieldDecorator: textFieldDecoratorComp
    property Component textEditDecorator: textEditDecoratorComp

    property Component comboBoxDecorator: comboBoxDecoratorComp
    property Component checkBoxDecorator: checkBoxDecoratorComp
    property Component switchDecorator: switchDecoratorComp
    property Component sliderDecorator: sliderDecoratorComp



    Component {
        id: buttonDecoratorComp

        Button{
            width: 160;
            height: 40;
            text: !baseElement ? "" : baseElement.text;
            icon.source: !baseElement ? "" : baseElement.iconSource;
            checkable: !baseElement ? false : baseElement.checkable;
            checked: !baseElement ? false : !baseElement.checkable ? false : baseElement.checked;
            highlighted: !baseElement ? false : baseElement.focus;
            property var baseElement;
            onClicked: {
                //console.log("Qt Button clicked!!!")
                if(baseElement){
                    baseElement.clicked()
                }
            }
            onToggled: {
                if(checked){
                    console.log("Qt Button toggled!!!")
                    if(baseElement){
                        baseElement.toggled()
                    }
                }
            }

        }
    }

    Component {
        id: roundButtonDecoratorComp

        RoundButton{
            width: 40;
            height: 40;
            radius: width;
            text: !baseElement ? "" : baseElement.text;
            icon.source: !baseElement ? "" : baseElement.iconSource;
            highlighted: !baseElement ? false : baseElement.focus;
            property var baseElement;
            onClicked: {
                //console.log("Qt Button clicked!!!")
                if(baseElement){
                    baseElement.clicked()
                }
            }

        }
    }

    Component {
        id: textFieldDecoratorComp


        TextField {
            width: 200;
            height: 40;

            z: baseElement ? 1 : -1;

            placeholderText: baseElement ? baseElement.placeHolderText : "";

            property var baseElement;

            onTextChanged: {
                if(baseElement){
                    baseElement.text = text;
                }
            }
            onAccepted: {
                if(baseElement){
                    baseElement.accepted();
                }
            }
            onEditingFinished: {
                if(baseElement){
                    baseElement.editingFinished();
                }
            }
            onTextEdited: {
                if(baseElement){
                    baseElement.textEdited();
                }
            }

            onFocusChanged: {
                console.log("focus", focus)
                if(focus){
                    forceActiveFocus();
                }
            }

        }
    }

    Component{
        id: textEditDecoratorComp;

        Flickable {
            id: flick;

            width: 200;
            height: 30;

            contentWidth: edit.paintedWidth;
            contentHeight: edit.paintedHeight;
            clip: true;
            z: baseElement ? 1 : -1;

            property var baseElement;
            property alias text: edit.text;

            onHeightChanged: {
                if(flick.baseElement){
                    flick.baseElement.height = height;
                }
            }

            onFocusChanged: {
                if(focus){
                    edit.forceActiveFocus();
                }
            }

            function ensureVisible(r)
            {
                if (contentX >= r.x)
                    contentX = r.x;
                else if (contentX+width <= r.x+r.width)
                    contentX = r.x+r.width-width;
                if (contentY >= r.y)
                    contentY = r.y;
                else if (contentY+height <= r.y+r.height)
                    contentY = r.y+r.height-height;
            }

            function setHeight(){
                //console.log("setHeight")
                height = 30;
            }

            TextEdit {
                id: edit

                width: flick.width;
                focus: true;
                wrapMode: TextEdit.Wrap;
                font.pixelSize: 15;
                clip: true;

                onCursorRectangleChanged: {
                    flick.ensureVisible(cursorRectangle);
                }

                onTextChanged: {
                    if(flick.baseElement){
                        flick.baseElement.text = text;
                    }
                }

                onEditingFinished: {
                    if(flick.baseElement){
                        flick.baseElement.editingFinished();
                    }
                }

            }
        }

    }

    Component{
        id: comboBoxDecoratorComp;

        ComboBox{
            width: 200;
            height: 30;

            topInset: 0;
            bottomInset: 0;

            textRole: "Name"
            displayText: currentIndex >= 0 ? currentText : "ComboBox"


            model: !baseElement ? 0 : baseElement.model;

            property var baseElement;

            onModelChanged: {
                if(model){
                    currentIndex = -1;
                }
            }

            onActivated: {
                var id = model.GetData("Id", currentIndex);
                console.log("Activated", currentIndex, id)
                if(baseElement){
                    baseElement.finished(id, currentIndex);
                }
            }

        }
    }

    Component{
        id: checkBoxDecoratorComp;

        CheckBox{
            height: 30;

            indicator.width: height - 10;
            indicator.height: height - 10;

            text: !baseElement ? "" : baseElement.text;
            checkState: !baseElement ? 0 : baseElement.checkState;

            property var baseElement;

            onCheckStateChanged: {
                if(baseElement){
                    baseElement.checkState = checkState;
                }
            }

        }

    }

    Component{
        id: switchDecoratorComp;

        Switch{

            height: 30;


            text: !baseElement ? "" : baseElement.text;
            checked: !baseElement ? false : baseElement.checked;

            property real backgroundWidth: contentItem.width;
            property var baseElement;

            onCheckedChanged: {
                if(baseElement){
                    baseElement.checked = checked;
                }
            }

        }

    }

    Component{
        id: sliderDecoratorComp;

        Slider{
            property var baseElement;

            onPositionChanged: {
                if(baseElement && baseElement.from !== undefined && baseElement.to !== undefined){
                    baseElement.position = position;
                }
            }
        }
    }
}
