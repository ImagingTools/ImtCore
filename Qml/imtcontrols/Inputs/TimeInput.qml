import QtQuick 2.15
import Acf 1.0
import imtcontrols 1.0

Rectangle {
    id: timeInput;

    width: minutesInput.width + minutesInput.x + mainMargin;
    height: 36;

    border.color: "lightgray";
    color: "transparent";

    property int mainMargin: 4;
    property int inputWidth: 36;
    property int fontSize: 18;

    property string fontColor: Style.textColor;

    property var hoursRegExp: /^([01]?\d)|(2[0-3]?)$/
    property var minutesRegExp: /^[0-5]?[0-9]$/
    property var timeRegExp: /^[01]\d|2[0-3]:[0-5]\d$/

    property bool canShowCurrentTime: false;

    property string time: (hoursInput.text + ":" + minutesInput.text).match(timeInput.timeRegExp) !== null ?
                               (hoursInput.text + ":" + minutesInput.text) : "";

    signal accepted(string value);

    Component.onCompleted:{
        if(canShowCurrentTime){
            showCurrentTime();
        }
    }

    function setTime(str){
        if(str.match(timeInput.timeRegExp) === null){
            console.log("Wrong time format!")
            return;
        }
        let arr = str.split(":");
        if(arr.length < 2){
            return;
        }

        hoursInput.text = arr[0];
        minutesInput.text = arr[1];
    }

    function showCurrentTime(){
        let date = new Date();

        let hours = date.getHours();
        let minutes = date.getMinutes();
        if(hours.length == 1){
            hours = "0" + hours;
        }
        if(minutes.length == 1){
            minutes = "0" + minutes;
        }

        let time  = hours + ":" + minutes;
        setTime(time);
    }

    TextField{
        id: hoursInput

        anchors.left: parent.left;
        anchors.leftMargin:  parent.mainMargin;
        anchors.verticalCenter: parent.verticalCenter;

        width: parent.inputWidth;
        height: parent.height - 2 * parent.mainMargin;

        textSize: parent.fontSize;
        fontColor: timeInput.fontColor;

        KeyNavigation.right: minutesInput;
        KeyNavigation.tab: minutesInput;

        textInputValidator : RegularExpressionValidator { regularExpression: timeInput.hoursRegExp }

        onFocusChanged: {
            if(!focus && text.length == 1){
                text = "0" + text;
            }
        }
        onAccepted: {
            if(text.length == 1){
                text = "0" + text;
            }
        }

        onTextChanged: {


        }
    }

    Text{
        id: colon;

        anchors.left: hoursInput.right;
        anchors.leftMargin: 4;
        anchors.verticalCenter: parent.verticalCenter;

        font.family: Style.fontFamily;
        font.pixelSize:  parent.fontSize;
        color: timeInput.fontColor;

        text: ":"

    }

    TextField{
        id: minutesInput

        anchors.left: colon.right;
        anchors.leftMargin: 4;
        anchors.verticalCenter: parent.verticalCenter;

        width: parent.inputWidth;
        height: parent.height - 2 * parent.mainMargin;

        textSize: parent.fontSize;
        fontColor: timeInput.fontColor;

        KeyNavigation.left: hoursInput;
        KeyNavigation.tab: hoursInput;

        textInputValidator : RegularExpressionValidator { regularExpression: timeInput.minutesRegExp }

        onFocusChanged: {
            if(!focus && text.length == 1){
                text = "0" + text;
            }
        }
        onAccepted: {
            if(text.length == 1){
                text = "0" + text;
            }
            let str = hoursInput.text + ":" + minutesInput.text;
            if(str.match(timeInput.timeRegExp) !== null){
                timeInput.accepted(str);
            }

        }

        onTextChanged: {

        }
    }
}
