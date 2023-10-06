pragma Singleton
import QtQuick 2.0
import Acf 1.0
import imtcontrols 1.0

Item {
    property Component topButtonDecorator: topButtonDecoratorComp
    property Component topPanelDecorator: topPanelDecoratorComp

    property Component buttonDecorator: buttonDecoratorComp
    property Component roundButtonDecorator: roundButtonDecoratorComp

    property Component textFieldDecorator: textFieldDecoratorComp
    property Component textEditDecorator: textEditDecoratorComp

    Component {
        id: topButtonDecoratorComp

        TopButtonDecoratorNew{

        }
    }

    Component{
        id: topPanelDecoratorComp;

        TopPanelDecorator{

        }

    }

    Component{
        id: buttonDecoratorComp;

        ButtonDecorator{
            width: 160;
            height: 30;
            radius: 4;

        }
    }

    Component{
        id: roundButtonDecoratorComp;

        ButtonDecorator{
            width: 30;
            height: width;
            radius: width;
        }
    }

    Component{
        id: textFieldDecoratorComp;

        Item{
            id: textFieldItem;

            width: 200;
            height: 30;
            z: -1;

            property var baseElement;
            property string text: "";

            Rectangle {
                id: mainRect;

                anchors.fill: parent;

                color: !textFieldItem.baseElement ? "transparent" : textFieldItem.baseElement.readOnly ? Style.alternateBaseColor : Style.baseColor;

                radius: !textFieldItem.baseElement ? 0 : textFieldItem.baseElement.radius;

                border.color: !textFieldItem.baseElement ? "transparent" : textFieldItem.baseElement.borderColorConst !== "" ? textFieldItem.baseElement.borderColorConst: textFieldItem.baseElement.activeFocus ? textFieldItem.baseElement.borderColor : Style.hover;

                border.width: 1;
            }

            Text {
                id: placeHolder;

                anchors.left: parent.left;
                anchors.leftMargin: !textFieldItem.baseElement ? 0 : textFieldItem.baseElement.margin;
                anchors.verticalCenter: parent.verticalCenter;


                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;

                //TODO -> Style
                color: !textFieldItem.baseElement ? "transparent" : textFieldItem.baseElement.placeHolderFontColor;
                opacity: !textFieldItem.baseElement ? 0 : textFieldItem.baseElement.placeHolderOpacity;

                visible: !textFieldItem.baseElement ? false : textFieldItem.baseElement.text == "";

                text: !textFieldItem.baseElement ? "" : textFieldItem.baseElement.placeHolderText;
            }

            MouseArea {
                id: mouseArea;

                anchors.fill: parent;
                cursorShape: !textFieldItem.baseElement ? Qt.ArrowCursor : textFieldItem.baseElement.readOnly ? Qt.ArrowCursor : Qt.IBeamCursor;

                visible: !textFieldItem.baseElement ? false : !textFieldItem.baseElement.readOnly;

                acceptedButtons: Qt.RightButton;

                onClicked: {
                    let point = mapToItem(null, mouse.x, mouse.y);
                    console.log("inputActions");
                    if(textFieldItem.baseElement){
                        textFieldItem.baseElement.openInputAction(point.x, point.y)
                    }
                }
            }
        }
    }

    Component{
        id: textEditDecoratorComp;

        Item{
            id: textEditItem;

            width: 200;
            height: 30;
            z: -1;

            property var baseElement;
            property string text: "";

            function setHeight(){
            }

            Rectangle {
                id: mainRect;

                anchors.fill: parent;

                color: !textEditItem.baseElement ? "transparent" : textEditItem.baseElement.readOnly ? Style.alternateBaseColor : Style.baseColor;

                radius: !textEditItem.baseElement ? 0 : textEditItem.baseElement.radius;

                border.color: !textEditItem.baseElement ? "transparent" : textEditItem.baseElement.borderColorConst !== "" ? textEditItem.baseElement.borderColorConst: textEditItem.baseElement.activeFocus ? textEditItem.baseElement.borderColor : Style.hover;

                border.width: 1;
            }

            Text {
                id: placeHolder;

                anchors.left: parent.left;
                anchors.leftMargin: !textEditItem.baseElement ? 0 : textEditItem.baseElement.margin;
                anchors.verticalCenter: parent.verticalCenter;


                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;

                //TODO -> Style
                color: !textEditItem.baseElement ? "transparent" : textEditItem.baseElement.placeHolderFontColor;
                opacity: !textEditItem.baseElement ? 0 : textEditItem.baseElement.placeHolderOpacity;

                visible: !textEditItem.baseElement ? false : textEditItem.baseElement.text == "";

                text: !textEditItem.baseElement ? "" : textEditItem.baseElement.placeHolderText;
            }

            MouseArea {
                id: mouseArea;

                anchors.fill: parent;
                cursorShape: !textEditItem.baseElement ? Qt.ArrowCursor : textEditItem.baseElement.readOnly ? Qt.ArrowCursor : Qt.IBeamCursor;

                visible: !textEditItem.baseElement ? false : !textEditItem.baseElement.readOnly;

                acceptedButtons: Qt.RightButton;

                onClicked: {
                    let point = mapToItem(null, mouse.x, mouse.y);
                    console.log("inputActions");
                    if(textEditItem.baseElement){
                        textEditItem.baseElement.openInputAction(point.x, point.y)
                    }
                }
            }
        }
    }

}
