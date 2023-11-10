import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtcontrols 1.0

Item {
    id: spinBox;

    width: 120;
    height: 30;

    property var baseElement: null;

    property real value: !baseElement ? 0 : baseElement.startValue;

    property var numberReg: !baseElement ? /^.*$/ : baseElement.numberReg;

    property real startValue: !baseElement ? 0 : baseElement.startValue;
    property real from: !baseElement ? 0 : baseElement.from;
    property real to: !baseElement ? 99 : baseElement.to;
    property real stepSize: !baseElement ? 1 : baseElement.stepSize;

    property bool editable: !baseElement ? true : baseElement.editable;
    property bool wrap: !baseElement ? false : baseElement.wrap;

    property string icon: !baseElement ? "" : baseElement.icon;

    property string fontColor: "#000000";
    property int fontSize: Style.fontSize_common;

    Component.onCompleted: {
        numberTextField.text = String(startValue);
        numberTextField.prevText = numberTextField.text;
    }

    onValueChanged: {
        if(numberTextField.text !== String(value)){
            numberTextField.text = String(value)
        }
        if(baseElement){
            baseElement.value = value;
        }
    }

    Rectangle{
        id: mainRec;

        anchors.fill: parent;

        radius: 4
        border.width: 1;
        border.color: "lightgrey";

        Item {
            id: numberBlock;

            anchors.left: parent.left;

            width: parent.width - buttonsBlock.width;
            height: parent.height;

            TextField {
                id: numberTextField;

                anchors.centerIn: parent;

                width: parent.width - 10;
                height: parent.height - 2*mainRec.border.width;
                fontColor: spinBox.fontColor;
                textSize: spinBox.fontSize;
                borderColorConst: "transparent";
                readOnly: !spinBox.editable;
                property string prevText: "";

                onTextChanged: {
                    numberTextField.text = numberTextField.text.replace(",",".");
                    if(numberTextField.text.search(spinBox.numberReg) < 0){
                        numberTextField.text = numberTextField.prevText;
                    }
                    else {
                        spinBox.value = parseFloat(numberTextField.text);
                    }
                    numberTextField.prevText = numberTextField.text;
                }
            }
        }

        Rectangle{
            id: buttonsBlock;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: parent.right;
            anchors.rightMargin: 2*mainRec.border.width;

            width: 20;
            height: parent.height - 2*mainRec.border.width;
            Column {
                id: buttonColumn;

                anchors.left: parent.left;
                anchors.leftMargin: -mainRec.border.width;
                anchors.verticalCenter: parent.verticalCenter;
                spacing: -mainRec.border.width

                width: parent.width +  mainRec.border.width;

                Button {
                    id: upButton;

                    width: buttonColumn.width;
                    height: buttonsBlock.height/2 + mainRec.border.width/2;

                    iconSource: spinBox.icon;

                    rotation: 180

                    onClicked: {

                        spinBox.increase();
                    }
                }

                Button {
                    id: downButton;

                    width: buttonColumn.width;
                    height: buttonsBlock.height/2 + mainRec.border.width/2;

                    iconSource: spinBox.icon;

                    onClicked: {
                        spinBox.decrease();
                    }
                }
            }
        }


    }//mainRec

    function increase(){
        let value = parseFloat(numberTextField.text)
        if(value + spinBox.stepSize > spinBox.to){
            if(spinBox.wrap){
                numberTextField.text = spinBox.from;
            }
            else {
                return;
            }
        }
        else {
            let precision = 0
            if (numberTextField.text.includes('.')){
                precision = numberTextField.text.split('.')[1].length
            }

            if (!numberTextField.text){
                numberTextField.text = spinBox.stepSize
            }
            else {

                value += spinBox.stepSize
                numberTextField.text = value.toFixed(precision)
            }
        }
        spinBox.value = parseFloat(numberTextField.text);
    }

    function decrease(){
        let value = parseFloat(numberTextField.text)
        if(value - spinBox.stepSize < spinBox.from){
            if(spinBox.wrap){
                numberTextField.text = spinBox.to;
            }
            else {
                return;
            }
        }
        else {
            let precision = 0
            if (numberTextField.text.includes('.')){
                precision = numberTextField.text.split('.')[1].length
            }

            if (!numberTextField.text){
                numberTextField.text = -spinBox.stepSize
            }
            else {
                value -= spinBox.stepSize
                numberTextField.text = value.toFixed(precision)
            }
        }
        spinBox.value = parseFloat(numberTextField.text);
    }


    Shortcut {
        sequence: "Down";
        enabled: spinBox.focus || numberTextField.activeFocus;
        onActivated: {
            spinBox.decrease();
        }
    }

    Shortcut {
        sequence: "Up";
        enabled: spinBox.focus || numberTextField.activeFocus;
        onActivated: {
            spinBox.increase();
        }
    }

    MouseArea{
        id: wheelMa;

        anchors.fill: parent;

        propagateComposedEvents: true;
        onClicked: {
            if(spinBox.editable){
                numberTextField.forceActiveFocus();
            }
            mouse.accepted = false;
        }
        onWheel: {
            if(wheel.angleDelta.y > 0){
                spinBox.increase();
            }
            else {
                spinBox.decrease();
            }

        }
    }
}
