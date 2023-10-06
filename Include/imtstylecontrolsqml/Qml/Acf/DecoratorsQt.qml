pragma Singleton
import QtQuick 2.12
import Acf 1.0
import QtQuick.Controls 2.15

Item {
    property Component buttonDecorator: buttonDecoratorComp
    property Component roundButtonDecorator: roundButtonDecoratorComp

    property Component textFieldDecorator: textFieldDecoratorComp
    property Component textEditDecorator: textEditDecoratorComp

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

        }
    }

    Component{
        id: textEditDecoratorComp;

        Flickable {
            id: flick;

            width: 200;
            height: 40;

            contentWidth: edit.paintedWidth;
            contentHeight: edit.paintedHeight;
            clip: true;
            z: baseElement ? 1 : -1;

            property var baseElement;
            property alias text: edit.text;


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
                console.log("setHeight")
                height = 40;
            }
            onHeightChanged: {
                if(flick.baseElement){
                    flick.baseElement.height = height;
                }
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


        //        TextEdit{
        //            width: 200;
        //            height: 40;

        //            z: baseElement ? 1 : -1;
        //            clip: true;

        //            property var baseElement;
        //            property string text: "";

        //            onTextChanged: {
        //                if(baseElement){
        //                    baseElement.text = text;
        //                }
        //            }

        //            onEditingFinished: {
        //                if(baseElement){
        //                    baseElement.editingFinished();
        //                }
        //            }

        //        }
    }

}
