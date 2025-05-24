import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

CustomTextField {
    id: root;
    placeHolderText: displayFormat
    textInputMask: timeEdit ? "00.00.0000 00:00" : "00.00.0000"
    maximumLength: timeEdit ? 16 : 10;
    hasActionMenu: false;

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
        internal.updateTextFromSelectedDate();
    }

    onSelectedDateChanged: {
        internal.updateTextFromSelectedDate();
    }

    onDateChanged: {
        internal.updateTextFromSelectedDate();
        root.select(internal.selectionStart, internal.selectionEnd);
        timer.restart();
    }

    Timer {
        id: timer;
        interval: 500;
        onTriggered: {
            root.editingFinished();
        }
    }

    onFocusChanged: {
        root.select(internal.selectionStart, internal.selectionEnd);
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

            root.text = root.getDateAsString();

            // soon
//            return;

//            if (prevText.length < text.length){
//                block = true;

//                let tempText = internal.convert(root.text);
//                if (root.text.length === root.maximumLength){
//                    let newDate = new Date(tempText);
//                    console.log("newDate", newDate);

//                    if (!isNaN(newDate)){
//                        root.selectedDate = newDate;
//                    }
//                    else{
//                        block = false;
////                        text = prevText;
//                        return;
//                    }
//                }

//                root.text = tempText;

//                block = false;
//            }

//            prevText = text;
        }

        function updateTextFromSelectedDate(){
            let dateStr = root.dateToString(root.selectedDate);
            root.text = dateStr;
        }

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
        anchors.rightMargin: Style.sizeSmallMargin;
        anchors.verticalCenter: root.verticalCenter;
        width: Style.sizeLargeMargin;
        z: root.parent.z + 1;

        Button {
            id: upButton;
            width: parent.width;
            height: Style.sizeMainMargin;
            iconSource: root.iconUpSource;
            enabled: !root.readOnly;
            decorator: Component { ButtonDecorator {
                    color: upButton.hovered ? Style.buttonHoverColor : "transparent";
                    border.width: 0;
                }}
            onClicked: {
                if (!enabled){
                    return;
                }

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
            height: Style.sizeMainMargin;
            iconSource: root.iconDownSource;
            enabled: !root.readOnly;
            decorator: Component { ButtonDecorator {
                    color: downButton.hovered ? Style.buttonHoverColor : "transparent";
                    border.width: 0;
                }}
            onClicked: {
                if (!enabled){
                    return;
                }

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

    function setDate(date){
        root.selectedDate = date;
        root.editingFinished();
    }

    function setDateAsString(dateStr){
        if (!validateDateFormat(dateStr)){
            console.error("Unable to set date in format '",dateStr, "'. Please check valid data or use next format: 'dd.MM.yyyy HH:mm' or 'dd.MM.yyyy'");
            return;
        }

        var data = dateStr.split(' ');

        if (data.length > 0){
            let datePartStr = data[0];

            var parts = datePartStr.split('.');
            let date = new Date(parts[2], parts[1] - 1, parts[0]);

            if (data.length > 1 && root.timeEdit){
                let timePartStr = data[1];

                if (timePartStr.split(';').length == 2){
                    let hours = timePartStr.split(';')[0]
                    let minutes = timePartStr.split(';')[1]

                    date.setHours(hours);
                    date.setMinutes(minutes);
                }
            }

            root.selectedDate = date;
            root.editingFinished();
        }
    }

    function getDateAsString(){
        return dateToString(selectedDate);
    }

    // Return string in format dd.MM.yyyy HH:mm
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

    function validateDateFormat(dateString){
        let formatWithTime = /^\d{2}\.\d{2}\.\d{4} \d{2}:\d{2}$/; // dd.MM.yyyy HH:mm
        let formatWithoutTime = /^\d{2}\.\d{2}\.\d{4}$/;          // dd.MM.yyyy

        if (formatWithTime.test(dateString)){
            return isValidDate(dateString, true); // Проверяем дату с временем
        } else if (formatWithoutTime.test(dateString)){
            return isValidDate(dateString, false); // Проверяем дату без времени
        }
        return false; // Строка не соответствует ни одному из форматов
    }

    function isValidDate(dateString, hasTime){
        let parts = dateString.split(/[\s.:]/); // Разделяем по пробелу, точке и двоеточию
        let day = parts[0];
        let month = parts[1];
        let year = parts[2];
        let hour = parts[3] || "0"; // Устанавливаем "0", если час не указан
        let minute = parts[4] || "0"; // Устанавливаем "0", если минуты не указаны

        let date = new Date(
            parseInt(year, 10),
            parseInt(month, 10) - 1, // Месяц в JavaScript начинается с 0
            parseInt(day, 10),
            parseInt(hour, 10),
            parseInt(minute, 10)
        );

        return (
            date.getFullYear() === parseInt(year, 10) &&
            date.getMonth() === parseInt(month, 10) - 1 &&
            date.getDate() === parseInt(day, 10) &&
            (!hasTime || (date.getHours() === parseInt(hour, 10) && date.getMinutes() === parseInt(minute, 10)))
        );
    }

    function dayIncrement(){
        if (root.readOnly){
            return;
        }

        root.selectedDate.setDate(root.selectedDate.getDate() + 1);
        root.dateChanged();
    }

    function dayDecrement(){
        if (root.readOnly){
            return;
        }
        root.selectedDate.setDate(root.selectedDate.getDate() - 1);
        root.dateChanged();
    }

    function monthIncrement(){
        if (root.readOnly){
            return;
        }
        root.selectedDate.setMonth(root.selectedDate.getMonth() + 1)
        root.dateChanged();
    }

    function monthDecrement(){
        if (root.readOnly){
            return;
        }
        root.selectedDate.setMonth(root.selectedDate.getMonth() - 1)
        root.dateChanged();
    }

    function yearIncrement(){
        if (root.readOnly){
            return;
        }
        root.selectedDate.setFullYear(root.selectedDate.getFullYear() + 1)
        root.dateChanged();
    }

    function yearDecrement(){
        if (root.readOnly){
            return;
        }
        root.selectedDate.setFullYear(root.selectedDate.getFullYear() - 1)
        root.dateChanged();
    }

    function hourIncrement(){
        if (root.readOnly){
            return;
        }
        root.selectedDate.setHours(root.selectedDate.getHours() + 1)
        root.dateChanged();
    }

    function hourDecrement(){
        if (root.readOnly){
            return;
        }
        root.selectedDate.setHours(root.selectedDate.getHours() - 1)
        root.dateChanged();
    }

    function minutesIncrement(){
        if (root.readOnly){
            return;
        }
        root.selectedDate.setMinutes(root.selectedDate.getMinutes() + 1)
        root.dateChanged();
    }

    function minutesDecrement(){
        if (root.readOnly){
            return;
        }
        root.selectedDate.setMinutes(root.selectedDate.getMinutes() - 1)
        root.dateChanged();
    }
}
