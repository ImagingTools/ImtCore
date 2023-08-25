import QtQuick 2.12
import imtqml 1.0
import imtgui 1.0
import Acf 1.0

Rectangle{
    id: spinBox;

    width: 120;
    height: 24;
    radius: Style.size_TextFieldRadius;
    clip: true;

    border.width: 1;
    border.color: Style.color_gray;

    property string icon: "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg";
    property int iconWidth: 5;
    property int iconHeight: 5;

    property string fontColor: "#000000";
    property int fontSize: Style.fontSize_common;

    property var numberReg: /^\-{0,1}\d*\.{0,1}\d*$/;

    property real startValue: 0;

    property real incrementStep: 1;

	property alias value: numberTextField.text;

    Component.onCompleted: {
        numberTextField.text = String(startValue);
        numberTextField.prevText = numberTextField.text;
    }

    Item {
        id: numberBlock;

        anchors.left: parent.left;

        width: parent.width - buttonsBlock.width;
        height: parent.height;

        CustomTextField {
            id: numberTextField;

            anchors.centerIn: parent;

            width: parent.width - 10;
            height: parent.height;
            color: "transparent";
            fontColor: spinBox.fontColor;
            textSize: spinBox.fontSize;
            borderColorConst: "transparent";

            property string prevText: "";

            onTextChanged: {
                numberTextField.text = numberTextField.text.replace(",",".");
                if(numberTextField.text.search(spinBox.numberReg) < 0){
                    numberTextField.text = numberTextField.prevText;
                }
                numberTextField.prevText = numberTextField.text;
            }
        }
    }

    Rectangle{
        id: buttonsBlock;

        anchors.top: parent.top;
        anchors.left: numberBlock.right;

        width: 20;
        height: parent.height;
        Column {
            id: buttonColumn;

            anchors.left: parent.left;
            anchors.leftMargin: -spinBox.border.width;
            anchors.verticalCenter: parent.verticalCenter;
            spacing: -spinBox.border.width

            width: parent.width +  spinBox.border.width;

            AuxButton {
                id: upButton;

                width: buttonColumn.width;
                height: buttonsBlock.height/2 + spinBox.border.width/2;
                radius: spinBox.radius;

                iconWidth: spinBox.iconWidth;
                iconHeight: spinBox.iconHeight;
                iconSource: spinBox.icon;

                rotation: 180
                highlighted: false;
                borderColor: spinBox.border.color;

                onClicked: {
                    let precision = 0
                    if (numberTextField.text.includes('.')){
                        precision = numberTextField.text.split('.')[1].length
                    }

                    if (!numberTextField.text){
                        numberTextField.text = spinBox.incrementStep
                    }
                    else {
                        let value = parseFloat(numberTextField.text)
                        value += spinBox.incrementStep
                        numberTextField.text = value.toFixed(precision)
                    }

                }
            }

            AuxButton {
                id: downButton;

                width: buttonColumn.width;
                height: buttonsBlock.height/2 + spinBox.border.width/2;
                radius: spinBox.radius;

                iconWidth: spinBox.iconWidth;
                iconHeight: spinBox.iconHeight;
                iconSource: spinBox.icon;

                highlighted: false;
                borderColor: spinBox.border.color;

                onClicked: {
                    let precision = 0
                    if (numberTextField.text.includes('.')){
                        precision = numberTextField.text.split('.')[1].length
                    }

                    if (!numberTextField.text){
                        numberTextField.text = -spinBox.incrementStep
                    }
                    else {
                        let value = parseFloat(numberTextField.text)
                        value -= spinBox.incrementStep
                        numberTextField.text = value.toFixed(precision)
                    }
                }
            }
        }
    }
}

