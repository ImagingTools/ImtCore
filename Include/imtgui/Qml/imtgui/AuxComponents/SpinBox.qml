import QtQuick 2.12
import Acf 1.0

Rectangle{
    id: spinBox;

    width: 120;
    height: 24;
    radius: Style.size_TextFieldRadius;
    clip: true;

    //color: "transparent";
    border.width: 1;
    border.color: Style.color_gray;

    property string iconUpSource: "../../../" + "Icons/" + Style.theme + "/" + "Up" + "_On_Normal.svg";
    property string iconDownSource: "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg";
    property int iconWidth: 12;
    property int iconHeight: 10;

    property string fontColor: "#000000";
    property int fontSize: Style.fontSize_common;


    property var numberReg: /^\-{0,1}\d*\.{0,1}\d*$/;


    property real startValue: 0;

    property real difference: 5;

    property string retVal: numberTextField.text;

    Component.onCompleted: {
        numberTextField.text = String(startValue);
        numberTextField.prevText = numberTextField.text;
    }

    Rectangle {
        id: numberBlock;

        anchors.left: parent.left;

        width: parent.width - buttonsBlock.width;
        height: parent.height;

        color: "transparent";
        border.width: spinBox.border.width;
        border.color: spinBox.border.color;
        radius: spinBox.radius;

        CustomTextField {
            id: numberTextField;

            anchors.centerIn: parent;

            width: parent.width - 10;
            height: parent.height - 2*parent.border.width;
            radius: parent.radius;
            color: "#ffffff";
            fontColor: spinBox.fontColor;
            textSize: spinBox.fontSize;
            borderColor: "transparent";

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

            width: parent.width +  spinBox.border.width;

            AuxButton {
                id: upButton;

                width: buttonColumn.width;
                height: buttonsBlock.height/2;
                radius: spinBox.radius;

                iconWidth: spinBox.iconWidth;
                iconHeight: spinBox.iconHeight;
                iconSource: spinBox.iconUpSource;

                highlighted: false;
                borderColor: spinBox.border.color;

                onClicked: {
                    var arr = numberTextField.text.split(".");
                    var numAfterDot = arr[1].length;
                    var val = Number(numberTextField.text);
                    val += spinBox.difference;
                    numberTextField.text = val.toFixed(numAfterDot);
                }
            }

            AuxButton {
                id: downButton;

                width: buttonColumn.width;
                height: buttonsBlock.height/2;
                radius: spinBox.radius;

                iconWidth: spinBox.iconWidth;
                iconHeight: spinBox.iconHeight;
                iconSource: spinBox.iconDownSource;

                highlighted: false;
                borderColor: spinBox.border.color;

                onClicked: {
                    var arr = numberTextField.text.split(".");
                    var numAfterDot = arr[1].length;
                    var val = Number(numberTextField.text);
                    val -= spinBox.difference;
                    numberTextField.text = val.toFixed(numAfterDot);
                }
            }
        }

    }

    Rectangle{
        anchors.fill: parent;

        color: "transparent";

        radius: parent.radius;
        border.color: parent.border.color;
        border.width: parent.border.width;
    }

}

