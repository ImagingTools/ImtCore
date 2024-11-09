import QtQuick 2.15
import Acf 1.0
import imtcontrols 1.0

CustomTextField {
    id: root;
    placeHolderText: displayFormat
    textInputMask: timeEdit ? "00.00.0000 00:00" : "00.00.0000"
    maximumLength: timeEdit ? 16 : 10;
    // textInputValidator: timeEdit ? dateTimeRegexp : dateRegexp;

    RegularExpressionValidator {
        id: dateRegexp;
        regularExpression: /^(0[1-9]|[12][0-9]|3[01])\.(0[1-9]|1[0-2])\.\d{4}/;
    }

    RegularExpressionValidator {
        id: dateTimeRegexp;
        regularExpression: /^(0[1-9]|[12][0-9]|3[01])\.(0[1-9]|1[0-2])\.\d{4} ([01]\d|2[0-3]):([0-5]\d)$/;
    }

    property bool calendarPopup: false;
    property bool timeEdit: true;
    property date maximumDate: new Date("9999-12-30");
    property date minimumDate: new Date("1752-09-14");
    property string displayFormat: timeEdit ? "dd.MM.yyyy hh:mm" : "dd.MM.yyyy"

    property string iconUpSource: readOnly ? "../../../" + Style.getIconPath("Icons/Up", Icon.State.Off, Icon.Mode.Disabled)
                                           : "../../../" + Style.getIconPath("Icons/Up", Icon.State.On, Icon.Mode.Normal);
    property string iconDownSource: readOnly ? "../../../" + Style.getIconPath("Icons/Down", Icon.State.Off, Icon.Mode.Disabled)
                                             : "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);

    property var selectedDate: timeEdit ? new Date("01.01.2000 00:00") : new Date("01.01.2000");

    signal dateChanged();

    Component.onCompleted: {
        let dateStr = dateToString(root.selectedDate);
        root.text = dateStr;
    }

    onDateChanged: {
        let dateStr = dateToString(root.selectedDate);
        root.text = dateStr;
        root.select(internal.selectionStart, internal.selectionEnd);
    }

    Timer {
        id: timer;

        onTriggered: {
            root.editingFinished();
        }
    }

    onFocusChanged: {
        root.select(internal.selectionStart, internal.selectionEnd);
    }

    onSelectionEndChanged: {
        console.log("onSelectionEndChanged", root.selectionEnd)
        if (root.selectionStart === root.selectionEnd){
            // if (root.selectionEnd <= 2){
            //     root.select(0, 2);
            // }
            // else if (root.selectionEnd > 2 && root.selectionEnd <= 5){
            //     root.select(3, 5);
            // }
            // else if (root.selectionEnd > 5 && root.selectionEnd <= 10){
            //     root.select(6, 10);
            // }
        }
    }

    QtObject {
        id: internal;

        property int selectionStart: root.selectionStart;
        property int selectionEnd: root.selectionEnd;
        property string text: root.text;
        property string prevText: "";
        property bool block: false;
        onTextChanged: {
            if (block){
                return;
            }

            console.log("onTextChanged", text, internal.selectionStart, internal.selectionEnd);

            if (prevText.length < text.length){
                block = true;

                root.text = internal.convert(root.text);
                if (root.text.length === root.maximumLength){
                    let newDate = new Date(root.text);

                    root.selectedDate = newDate;
                }

                block = false;
            }

            prevText = text;
        }

        // 11.11.1222 22:22
        // 11 11 1222 2222
        function convert(text){
            let date = text
            let len = date.length;
            let maxLen = 16;
            let currentPos = root.selectionEnd;

            while(!(date.indexOf(":") < 0)){
                date = date.replace(":", "")
                currentPos--;
            }

            while(!(date.indexOf(".") < 0)){
                date = date.replace(".", "")
                currentPos--;
            }

            while(!(date.indexOf(" ") < 0)){
                date = date.replace(" ", "")
                currentPos--;
            }

            let result = ""
            for (let i = 1; i <= date.length; i++){
                result += date[i - 1]
                if (i != 0 && i % 2 == 0 && i < 5){
                    result += ".";
                }

                if (root.timeEdit){
                    if (i == 8){
                        result += " ";
                    }
                    else if (i == 10){
                        result += ":";
                    }
                }
                console.log("result", i, result)
            }

            return result;
        }
    }

    MouseArea {
        id: ma;
        anchors.fill: parent;
        onWheel: {
            if(wheel.angleDelta.y >= 0){
                upButton.clicked();
            }
            else{
                downButton.clicked();
            }
        }

        Keys.onUpPressed: {
            upButton.clicked();
        }

        Keys.onDownPressed: {
            downButton.clicked();
        }
    }

    Column {
        anchors.right: root.right;
        anchors.rightMargin: Style.size_smallMargin;
        anchors.verticalCenter: root.verticalCenter;
        width: Style.size_largeMargin;
        z: root.parent.z + 1;

        Button {
            id: upButton;
            width: parent.width;
            height: Style.size_mainMargin;
            iconSource: root.iconUpSource;
            enabled: !root.readOnly;
            decorator: Component { ButtonDecorator {
                    color: upButton.hovered ? Style.buttonHoverColor : "transparent";
                    border.width: 0;
                }}
            onClicked: {
                console.log("onClicked", internal.selectionStart, internal.selectionEnd)
                let tempStart = root.selectionStart
                internal.selectionStart = tempStart;

                let tempEnd = root.selectionEnd
                internal.selectionEnd = tempEnd;

                if (root.selectionEnd <= 2){
                    root.dayIncrement();
                }
                else if (root.selectionEnd > 2 && root.selectionEnd <= 5){
                    root.monthIncrement();
                }
                else if (root.selectionEnd > 5 && root.selectionEnd <= 10){
                    root.yearIncrement();
                }
                else if (root.selectionEnd > 10 && root.selectionEnd <= 13){
                    root.hourIncrement();
                }
                else{
                    root.minutesIncrement();
                }
            }
        }

        Button {
            id: downButton;
            width: parent.width;
            height: Style.size_mainMargin;
            iconSource: root.iconDownSource;
            enabled: !root.readOnly;
            decorator: Component { ButtonDecorator {
                    color: downButton.hovered ? Style.buttonHoverColor : "transparent";
                    border.width: 0;
                }}
            onClicked: {
                let tempStart = root.selectionStart
                internal.selectionStart = tempStart;

                let tempEnd = root.selectionEnd
                internal.selectionEnd = tempEnd;

                if (root.selectionEnd <= 2){
                    root.dayDecrement();
                }
                else if (root.selectionEnd > 2 && root.selectionEnd <= 5){
                    root.monthDecrement();
                }
                else if (root.selectionEnd > 5 && root.selectionEnd <= 10){
                    root.yearDecrement();
                }
                else if (root.selectionEnd > 10 && root.selectionEnd <= 13){
                    root.hourDecrement();
                }
                else{
                    root.minutesDecrement();
                }
            }
        }
    }

    function dateIsValid(){
        return true;
    }

    function setDate(date){
        root.selectedDate = date;
        root.dateChanged();
    }

    function setDateAsString(dateStr){
        root.selectedDate = new Date(dateStr);
        root.dateChanged();
    }

    function getDateAsString(){
        return dateToString(selectedDate);
    }

    // Return string in format dd.MM.yyyy H:mm
    function dateToString(date){
        let monthStr = String(date.getMonth() + 1)
        if (monthStr.length == 1){
            monthStr = "0" + monthStr;
        }

        let dayStr = String(date.getDate())
        if (dayStr.length == 1){
            dayStr = "0" + dayStr;
        }

        let hoursStr = String(date.getHours())
        if (hoursStr.length == 1){
            hoursStr = "0" + hoursStr;
        }

        let minutesStr = String(date.getMinutes())
        if (minutesStr.length == 1){
            minutesStr = "0" + minutesStr;
        }

        if (root.timeEdit){
            return dayStr + "." + monthStr + "." + date.getFullYear() + " " + hoursStr + ":" + minutesStr;
        }
        else{
            return dayStr + "." + monthStr + "." + date.getFullYear();
        }
    }

    function dayIncrement(){
        root.selectedDate.setDate(root.selectedDate.getDate() + 1);
        root.dateChanged();
    }

    function dayDecrement(){
        root.selectedDate.setDate(root.selectedDate.getDate() - 1);
        root.dateChanged();
    }

    function monthIncrement(){
        root.selectedDate.setMonth(root.selectedDate.getMonth() + 1)
        root.dateChanged();
    }

    function monthDecrement(){
        root.selectedDate.setMonth(root.selectedDate.getMonth() - 1)
        root.dateChanged();
    }

    function yearIncrement(){
        root.selectedDate.setFullYear(root.selectedDate.getFullYear() + 1)
        root.dateChanged();
    }

    function yearDecrement(){
        root.selectedDate.setFullYear(root.selectedDate.getFullYear() - 1)
        root.dateChanged();
    }

    function hourIncrement(){
        root.selectedDate.setHours(root.selectedDate.getHours() + 1)
        root.dateChanged();
    }

    function hourDecrement(){
        root.selectedDate.setHours(root.selectedDate.getHours() - 1)
        root.dateChanged();
    }

    function minutesIncrement(){
        root.selectedDate.setMinutes(root.selectedDate.getMinutes() + 1)
        root.dateChanged();
    }

    function minutesDecrement(){
        root.selectedDate.setMinutes(root.selectedDate.getMinutes() - 1)
        root.dateChanged();
    }
}
