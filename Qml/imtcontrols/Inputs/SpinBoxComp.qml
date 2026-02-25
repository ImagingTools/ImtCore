import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

Item {
    id: spinBox;
    
    width: 120;
    height: Style.controlHeightM;
    
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
    
    property string fontColor: Style.textColor
    property int fontSize: Style.fontSizeM;
    property bool textInputEnabled: true
    
    Component.onCompleted: {
        numberTextField.text = String(startValue);
    }
    
    onStartValueChanged: {
        numberTextField.text = String(startValue);
    }
    
    Connections {
        target: spinBox.baseElement ? spinBox.baseElement : undefined
        function onValueChanged(){
            numberTextField.text = target.value
        }
    }
    
    Rectangle{
        id: mainRec;
        
        anchors.fill: parent;
        
        radius: Style.radiusM;
        border.width: 1;
        border.color: Style.borderColor;
        
        Item {
            id: numberBlock;
            
            anchors.left: parent.left;
            
            width: parent.width - buttonsBlock.width;
            height: parent.height;
            
            CustomTextField {
                id: numberTextField;
                
                anchors.centerIn: parent;
                
                width: parent.width - Style.marginM;
                height: parent.height - 2*mainRec.border.width;
                fontColor: spinBox.fontColor;
                textSize: spinBox.fontSize;
                borderColorConst: "transparent";
                readOnly: !spinBox.editable || !spinBox.textInputEnabled;
                editingFinishedTimerInterval: 1000

                onTextChanged: {
                    if (!spinBox.baseElement){
                        return
                    }

                    text = text.replace(",",".");
                }

                onEditingFinished: {
                    if (text === "" || text.search(spinBox.numberReg) < 0){
                       text = spinBox.baseElement.value
                    }
                    else{
                        let value = parseFloat(text)
                        if (value < spinBox.from || value > spinBox.to){
                            text = spinBox.baseElement.value
                        }
                        else{
                            spinBox.baseElement.value = value
                        }
                    }
                }

				BaseText{
					id: textSize

					anchors.left: parent.left
					anchors.leftMargin: numberTextField.textFieldLeftMargin

					visible: false

					text: numberTextField.text
				}

				BaseText{
					id: unitsText

					anchors.verticalCenter: parent.verticalCenter
					anchors.left: textSize.right
					anchors.leftMargin: Style.marginXS

					visible: !spinBox.baseElement ? false : spinBox.baseElement.units !== ""

					text: spinBox.baseElement ? spinBox.baseElement.units : ""
				}
            }
        }
        
        Rectangle{
            id: buttonsBlock;
            
            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: parent.right;
            anchors.rightMargin: 2*mainRec.border.width;
            
            width: Style.buttonWidthS;
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
                    enabled: !spinBox.editable ? false : !spinBox.baseElement ? true : spinBox.baseElement.value < spinBox.baseElement.to
                    onClicked: {
                        if (spinBox.baseElement){
                            spinBox.baseElement.increase();
                        }
                    }
                }
                
                Button {
                    id: downButton;
                    
                    width: buttonColumn.width;
                    height: buttonsBlock.height/2 + mainRec.border.width/2;
                    
                    iconSource: spinBox.icon;
                    enabled: !spinBox.editable ? false : !spinBox.baseElement ? true : spinBox.baseElement.value > spinBox.baseElement.from
                    
                    onClicked: {
                        if (spinBox.baseElement){
                            spinBox.baseElement.decrease();
                        }
                    }
                }
            }
        }
    }//mainRec
    
    Shortcut {
        sequence: "Down";
        enabled: spinBox.focus || numberTextField.activeFocus;
        onActivated: {
            if (spinBox.baseElement){
                spinBox.baseElement.decrease();
            }
        }
    }
    
    Shortcut {
        sequence: "Up";
        enabled: spinBox.focus || numberTextField.activeFocus;
        onActivated: {
            if (spinBox.baseElement){
                spinBox.baseElement.increase();
            }
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
            if (!spinBox.baseElement){
                return
            }
            if(!spinBox.editable){
                return
            }
            if(wheel.angleDelta.y > 0){
                spinBox.baseElement.increase();
            }
            else {
                spinBox.baseElement.decrease();
            }
        }
    }
}
