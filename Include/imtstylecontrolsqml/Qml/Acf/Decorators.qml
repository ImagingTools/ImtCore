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

    property Component comboBoxDecorator: comboBoxDecoratorComp

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
                console.log("setHeight");
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

    Component{
        id: comboBoxDecoratorComp;

        Rectangle {
            id: cbMainRect;

            width: 200;
            height: 30;

            property var baseElement;
            property var model;
            property int currentIndex: -1;


            border.color: !baseElement ? "transparent" : baseElement.borderColor;
            border.width: 1;

            radius: !baseElement ? 0 : baseElement.radius;
            color: !baseElement ? "transparent" : baseElement.backgroundColor;

            gradient: Gradient {
                id: grad;
                property bool isColor: !baseElement ? false : baseElement.isColor;

                GradientStop { position: 0.0; color: grad.isColor ? cbMainRect.color : Style.imagingToolsGradient1; }
                GradientStop { position: 0.97; color: grad.isColor ? cbMainRect.color : Style.imagingToolsGradient2; }
                GradientStop { position: 0.98; color: grad.isColor ? cbMainRect.color : Style.imagingToolsGradient3; }
                GradientStop { position: 1.0; color: grad.isColor ? cbMainRect.color : Style.imagingToolsGradient4; }

            }

            Text {
                id: cbTitleTxt;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: !baseElement ? 0 : baseElement.textCentered ? parent.width/2 - width/2 : 10;
                anchors.right: parent.right;
                anchors.rightMargin: 10;

                color: !baseElement ? "transparent" : baseElement.fontColorTitle;
                text: !baseElement ? "" : baseElement.currentText !== "" ? baseElement.currentText : baseElement.placeHolderText;
                font.family: Style.fontFamily;
                font.pixelSize: !baseElement ? 10 : baseElement.textSize;

                elide: Text.ElideRight;
            }

            Image {
                id: cbArrowIcon;

                anchors.right: cbMainRect.right;
                anchors.verticalCenter: cbMainRect.verticalCenter;
                anchors.rightMargin: 5;

                width: 12;
                height: 10;
                rotation: !baseElement ? 0 : baseElement.isOpen ? 180 : 0
                source: "../../../" + Style.theme + "/Icons/" + "/" + "Down" + "_On_Normal.svg";

                sourceSize.width: width;
                sourceSize.height: height;
            }
        }

    }

}
