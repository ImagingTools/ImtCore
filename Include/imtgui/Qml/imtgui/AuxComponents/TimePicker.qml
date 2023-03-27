import QtQuick 2.12
import Acf 1.0

Item {
    id: timePicker;

    height: 20;

    property bool completed: false;

    //icons
    property int iconWidth: 12;
    property int iconHeight: 10;
    property string iconUpSource: "../../../" + "Icons/" + Style.theme + "/" + "Up" + "_On_Normal.svg";
    property string iconDownSource: "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg";
    //margins
    property int mainMargin: 0;
    //CustomTextField
    property int textFieldHeight: 20;
    property int textFieldWidthHour: 25;
    property int textFieldWidthMinute: 25;
    property int textFieldRadius:0;
    property int textSize: 15;
    property string textFieldColor: Style.baseColor;
    property string fontColor: Style.textColor;
    property string textFieldBorderColor: "";//textField.acceptableInput ? Style.iconColorOnSelected : Style.errorTextColor;
    property int textFieldMargin: 5;
    //buttons
    property int buttonWidth: 15;
    property int buttonHeight: 10;

    property string buttonColor:"";
    property int buttonHighlighted: -1;
    //

    property int selectedHour: 0;
    property int selectedMinute: 0;

    signal timeChanged();

    onSelectedHourChanged: {
        console.log("onSelectedHourChanged");

        hourField.text = timePicker.selectedHour;

        timePicker.timeChanged()
    }

    onSelectedMinuteChanged: {
        console.log("onSelectedMinuteChanged");

        minuteField.text = timePicker.selectedMinute;

        timePicker.timeChanged()
    }

    function fromString(str){
        let data = str.split(":");

        if (data.length !== 2){
            return;
        }

        let hourStr = data[0];
        let minuteStr = data[1];

        timePicker.selectedHour = Number(hourStr);
        timePicker.selectedMinute = Number(minuteStr);
    }

    function toStr(){
        let hourStr = String(timePicker.selectedHour);
        let minuteStr = String(timePicker.selectedMinute);

        if (hourStr.length == 1){
            hourStr = '0' + hourStr;
        }

        if (minuteStr.length == 1){
            minuteStr = '0' + minuteStr;
        }

        return hourStr + ":" + minuteStr;
    }

    Row {
        height: timePicker.height;

        spacing: 10;

        Row {
            id: mainRow;
            height: parent.height;
            spacing: timePicker.mainMargin;

            RegExpValidator {
                id: hourValid;

                regExp: /^([0-1]?[0-9]|2[0-3])/;
            }

            RegExpValidator {
                id: minuteValid;

                regExp: /^[0-5]?[0-9]$/;
            }


            // ^([0-1]?[0-9]|2[0-3]):[0-5][0-9]$

            CustomTextField {
                id: hourField;

                anchors.verticalCenter: parent.verticalCenter;

                height: timePicker.textFieldHeight;
                width: timePicker.textFieldWidthHour;
                radius: timePicker.textFieldRadius;
                textSize: timePicker.textSize;
                fontColor: timePicker.fontColor;
                color: timePicker.textFieldColor;
                borderColorConst: timePicker.textFieldBorderColor !== "" ? timePicker.textFieldBorderColor : acceptableInput ? Style.iconColorOnSelected : Style.errorTextColor;
                margin: timePicker.textFieldMargin;

                textInputValidator: hourValid;

                text: timePicker.selectedHour;

                horizontalAlignment: TextInput.AlignHCenter;

                Keys.onUpPressed: {
                    hourUpButton.clicked();
                }

                Keys.onDownPressed: {
                    hourDownButton.clicked();
                }

                onEditingFinished: {
                    timePicker.selectedHour = Number(hourField.text);
                }

                onTextChanged: {
                    if (hourField.text.length == 1){
                        hourField.text = '0' + hourField.text;
                    }
                }

                MouseArea {
                    anchors.fill: hourField;

                    onWheel: {
                        console.log("hourField onWheel");

                        if (wheel.angleDelta.y > 0){
                            hourUpButton.clicked();
                        }
                        else{
                            hourDownButton.clicked();
                        }
                    }
                }
            }

            Column {
                anchors.verticalCenter: parent.verticalCenter;
                AuxButton {
                    id: hourUpButton;

                    width: timePicker.buttonWidth;
                    height: timePicker.buttonHeight;

                    iconWidth: timePicker.iconWidth;
                    iconHeight: timePicker.iconHeight;
                    iconSource: timePicker.iconUpSource;

                    highlighted: timePicker.buttonHighlighted >= 0 ? timePicker.buttonHighlighted : containsMouse;
                    color: timePicker.buttonColor !== "" ? timePicker.buttonColor : defaultColor;

                    onClicked: {
                        if (timePicker.selectedHour < 23){
                            timePicker.selectedHour++;
                        }
                        else{
                            timePicker.selectedHour = 0;
                        }
                    }
                }

                AuxButton {
                    id: hourDownButton;

                    width: timePicker.buttonWidth;
                    height: timePicker.buttonHeight;

                    iconWidth: timePicker.iconWidth;
                    iconHeight: timePicker.iconHeight;
                    iconSource: timePicker.iconDownSource;

                    highlighted: timePicker.buttonHighlighted >= 0 ? timePicker.buttonHighlighted : containsMouse;
                    color: timePicker.buttonColor !== "" ? timePicker.buttonColor : defaultColor;


                    onClicked: {
                        if (timePicker.selectedHour > 0){
                            timePicker.selectedHour--;
                        }
                        else{
                            timePicker.selectedHour = 23;
                        }
                    }
                }
            }

            CustomTextField {
                id: minuteField;

                anchors.verticalCenter: parent.verticalCenter;

                height: timePicker.textFieldHeight;
                width: timePicker.textFieldWidthMinute;
                radius: timePicker.textFieldRadius;
                textSize: timePicker.textSize;
                fontColor: timePicker.fontColor;
                color: timePicker.textFieldColor;
                borderColorConst: timePicker.textFieldBorderColor !== "" ? timePicker.textFieldBorderColor : acceptableInput ? Style.iconColorOnSelected : Style.errorTextColor;
                margin: timePicker.textFieldMargin;

                text: timePicker.selectedMinute;

                textInputValidator: minuteValid;

                horizontalAlignment: TextInput.AlignHCenter;

                Keys.onUpPressed: {
                    minuteUpButton.clicked();
                }

                Keys.onDownPressed: {
                    minuteDownButton.clicked();
                }

                onEditingFinished: {
                    if (minuteField.text.length == 1){
                        minuteField.text = '0' + minuteField.text;
                    }

                    timePicker.selectedMinute = Number(minuteField.text);
                }

                MouseArea {
                    anchors.fill: minuteField;

                    onWheel: {
                        console.log("hourField onWheel");

                        if (wheel.angleDelta.y > 0){
                            minuteUpButton.clicked();
                        }
                        else{
                            minuteDownButton.clicked();
                        }
                    }
                }

                onTextChanged: {
                    if (minuteField.text.length == 1){
                        minuteField.text = '0' + minuteField.text;
                    }
                }
            }

            Column {
                id: minuteButtons;

                anchors.verticalCenter: parent.verticalCenter;

                AuxButton {
                    id: minuteUpButton;

                    width: timePicker.buttonWidth;
                    height: timePicker.buttonHeight;

                    iconWidth: timePicker.iconWidth;
                    iconHeight: timePicker.iconHeight;
                    iconSource: timePicker.iconUpSource;

                    highlighted: timePicker.buttonHighlighted >= 0 ? timePicker.buttonHighlighted : containsMouse;
                    color: timePicker.buttonColor !== "" ? timePicker.buttonColor : defaultColor;


                    onClicked: {
                        if (timePicker.selectedMinute < 59){
                            timePicker.selectedMinute++;
                        }
                        else{
                            timePicker.selectedMinute = 0;
                        }
                    }
                }

                AuxButton {
                    id: minuteDownButton;

                    width: timePicker.buttonWidth;
                    height: timePicker.buttonHeight;

                    iconWidth: timePicker.iconWidth;
                    iconHeight: timePicker.iconHeight;
                    iconSource: timePicker.iconDownSource;

                    highlighted: timePicker.buttonHighlighted >= 0 ? timePicker.buttonHighlighted : containsMouse;
                    color: timePicker.buttonColor !== "" ? timePicker.buttonColor : defaultColor;


                    onClicked: {
                        if (timePicker.selectedMinute > 0){
                            timePicker.selectedMinute--;
                        }
                        else{
                            timePicker.selectedMinute = 59;
                        }
                    }
                }
            }
        }
    }
}
