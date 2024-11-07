import QtQuick 2.15
import Acf 1.0
import imtcontrols 1.0

CustomTextField {
    id: root;
    placeHolderText: "dd.MM.yyyy hh:mm"
    textInputMask: "00.00.0000 00:00"
    textInputValidator: RegularExpressionValidator {
        regularExpression: /^(0[1-9]|[12][0-9]|3[01])\.(0[1-9]|1[0-2])\.\d{4} ([01]\d|2[0-3]):([0-5]\d)$/;
    }

    property bool calendarPopup: false;
    property date maximumDate: new Date("9999-12-30");
    property date minimumDate: new Date("1752-09-14");
    property string displayFormat: "dd.MM.yyyy H:mm";

    property string iconUpSource: readOnly ? "../../../" + Style.getIconPath("Icons/Up", Icon.State.Off, Icon.Mode.Disabled)
                                           : "../../../" + Style.getIconPath("Icons/Up", Icon.State.On, Icon.Mode.Normal);
    property string iconDownSource: readOnly ? "../../../" + Style.getIconPath("Icons/Down", Icon.State.Off, Icon.Mode.Disabled)
                                             : "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);

    property var selectedDate: new Date("01.01.2000 00:00");

    signal dateChanged();

    Keys.onUpPressed: {
        upButton.clicked();
    }

    Keys.onDownPressed: {
        downButton.clicked();
    }

    Component.onCompleted: {
        let dateStr = dateToString(root.selectedDate);
        root.text = dateStr;

    }

    onSelectionEndChanged: {
        console.log("onSelectionEndChanged", selectionEnd);
    }

    onDateChanged: {
        let dateStr = dateToString(root.selectedDate);
        root.text = dateStr;

        root.select(internal.selectionStart, internal.selectionEnd);
    }

    QtObject {
        id: internal;

        property int selectionStart: root.selectionStart;
        property int selectionEnd: root.selectionEnd;
    }

    MouseArea {
        anchors.fill: parent;

        onWheel: {
            if(wheel.angleDelta.y >= 0){
                upButton.clicked();
            }
            else{
                downButton.clicked();
            }
        }
    }

    Column {
        anchors.right: root.right;
        anchors.rightMargin: Style.size_smallMargin;
        anchors.verticalCenter: root.verticalCenter;
        width: Style.size_mainMargin;
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

        return dayStr + "." + monthStr + "." + date.getFullYear() + " " + hoursStr + ":" + minutesStr;
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
