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
    property Component checkBoxDecorator: checkBoxDecoratorComp
    property Component switchDecorator: switchDecoratorComp
    property Component sliderDecorator: sliderDecoratorComp

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
                //console.log("setHeight");
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

    Component{
        id: checkBoxDecoratorComp;

        Item{
            id: checkBoxItem;

            width: checkBoxText.text == "" ? checkRect.width : checkRect.width + checkBoxText.width + checkBoxItem.mainMargin;
            height: 20;

            property var baseElement: null;

            property int mainMargin: !checkBoxItem.baseElement ? 0 : 8;
            property string text: !baseElement ? "" : baseElement.text == undefined ? "" : baseElement.text;
            property string borderColor: !baseElement ? "" : baseElement.borderColor == undefined ? "" : baseElement.borderColor;
            property int fontPixelSize: !baseElement ? 10 : baseElement.fontPixelSize == undefined ? 10 : baseElement.fontPixelSize;
            property bool fontBold: !baseElement ? false : baseElement.fontBold == undefined ? false : baseElement.fontBold;

            Rectangle {
                id: checkRect;

                anchors.left: parent.left;
                anchors.leftMargin: !checkBoxItem.baseElement ? 0 : !checkBoxItem.baseElement.isLeftText ? 0 : checkBoxText.width + checkBoxItem.baseElement.mainMargin;
                anchors.verticalCenter: parent.verticalCenter;

                width: height;
                height: parent.height;

                color: "transparent";

                border.width: 1;
                border.color: !checkBoxItem.baseElement ? "transparent" : checkBoxItem.borderColor !=="" ? checkBoxItem.borderColor  : checkBoxItem.baseElement.isActive ? Style.borderColor : Style.disabledInActiveTextColor;

                Image {
                    id: image;

                    anchors.centerIn: parent;

                    height: checkRect.height - 1;
                    width: height;

                    sourceSize.width: width;
                    sourceSize.height: height;

                    visible: !checkBoxItem.baseElement ? false : checkBoxItem.baseElement.checkState != Qt.PartiallyChecked;
                    source: !checkBoxItem.baseElement ? "" : checkBoxItem.baseElement.isActive ? checkBoxItem.baseElement.checkState == Qt.Checked ? checkBoxItem.baseElement.imageSourceActive : "" :
                        checkBoxItem.baseElement.checkState == Qt.Checked ? checkBoxItem.baseElement.imageSourceNotActive : "";
                }
            }

            Rectangle {
                id: rect;

                anchors.centerIn: checkRect;

                height: checkRect.height - 4;
                width: height;

                color: Style.textColor;

                visible: !checkBoxItem.baseElement ? false : checkBoxItem.baseElement.checkState == Qt.PartiallyChecked;
            }



            Text {
                id: checkBoxText;

                anchors.left: parent.left;
                anchors.leftMargin: !checkBoxItem.baseElement ? 0 : checkBoxItem.baseElement.isLeftText ? 0 : checkRect.width + checkBoxItem.baseElement.mainMargin;
                anchors.verticalCenter: parent.verticalCenter;

                color: !checkBoxItem.baseElement ? "transparent" :  checkBoxItem.baseElement.enabled ? Style.buttonText : Style.inactive_buttonText;

                font.pixelSize: checkBoxItem.fontPixelSize;
                font.family: Style.fontFamily;
                font.bold: checkBoxItem.fontBold;

                text: checkBoxItem.text;
            }

        }

    }

    Component{
        id: switchDecoratorComp;

        Item{
            id: switchItem;

            width: switchText.text == "" ? backgroundRec.width : backgroundRec.width + switchText.width + switchItem.mainMargin;
            height: 20;

            property var baseElement: null;
            property bool checked: !baseElement ? false : baseElement.checked;

            property int mainMargin: !baseElement ? 0 : 8;
            property string text: !baseElement ? "" : baseElement.text == undefined ? "" : baseElement.text;
            property string borderColor: !baseElement ? "" : baseElement.borderColor == undefined ? "" : baseElement.borderColor;
            property int fontPixelSize: !baseElement ? 10 : baseElement.fontPixelSize == undefined ? 10 : baseElement.fontPixelSize;
            property bool fontBold: !baseElement ? false : baseElement.fontBold == undefined ? false : baseElement.fontBold;

            property real backgroundWidth: 40;

            Rectangle{
                id: backgroundRec;

                anchors.left: parent.left;
                anchors.verticalCenter: parent.verticalCenter;

                width: switchItem.backgroundWidth;
                height: !switchItem.baseElement ? 0 : switchItem.baseElement.backgroundHeight;
                color: !switchItem.baseElement ? "transparent" : switchItem.checked ? switchItem.baseElement.backgroundColor_checked :
                                              switchItem.baseElement.backgroundColor;
                radius: height;

                opacity: !switchItem.baseElement ? 0 : switchItem.baseElement.backgroundOpacity;

            }

            Rectangle{
                id: controlRec;

                anchors.verticalCenter: parent.verticalCenter;

                x: !switchItem.baseElement ? 0 : switchItem.baseElement.controlRecX;
                width: parent.height;
                height: width;
                radius: width;
                color: !switchItem.baseElement ? "transparent" : switchItem.checked ? switchItem.baseElement.controlColor_checked :
                                              switchItem.baseElement.controlColor;
            }

            Text {
                id: switchText;

                anchors.left: parent.left;
                anchors.leftMargin: !switchItem.baseElement ? 0 : switchItem.baseElement.isLeftText ? 0 : backgroundRec.width + switchItem.baseElement.mainMargin;
                anchors.verticalCenter: parent.verticalCenter;

                color: !switchItem.baseElement ? "transparent" :  switchItem.baseElement.enabled ? Style.buttonText : Style.inactive_buttonText;

                font.pixelSize: switchItem.fontPixelSize;
                font.family: Style.fontFamily;
                font.bold: switchItem.fontBold;

                text: switchItem.text;
            }
        }

    }

    Component{
        id: sliderDecoratorComp;

        Item{
            id: sliderItem;

            width: 150;
            height: 20;

            property var baseElement: null;
            property real value: 0;

            property string borderColor: !baseElement ? "" : baseElement.borderColor == undefined ? "" : baseElement.borderColor;

            Rectangle{
                id: backgroundRec;

                anchors.left: parent.left;
                anchors.verticalCenter: parent.verticalCenter;

                width: parent.width;
                height: !sliderItem.baseElement ? 0 : sliderItem.baseElement.backgroundHeight;
                color: !sliderItem.baseElement ? "transparent" : sliderItem.baseElement.backgroundColor;

                radius: height;

                opacity: !sliderItem.baseElement ? 0 : sliderItem.baseElement.backgroundOpacity;

                Rectangle{
                    width: !sliderItem.baseElement ? 0 : sliderItem.baseElement.controlRecX + sliderItem.baseElement.height/2;
                    height: parent.height;
                    radius: parent.radius;
                    color: !sliderItem.baseElement ? "transparent" : sliderItem.baseElement.controlColor;
                }

            }

            Rectangle{
                id: controlRec;

                anchors.verticalCenter: parent.verticalCenter;

                x: !sliderItem.baseElement ? 0 : sliderItem.baseElement.controlRecX;
                width: parent.height;
                height: width;
                radius: width;
                color: !sliderItem.baseElement ? "transparent" : sliderItem.baseElement.controlColor;

            }

        }
    }

}
