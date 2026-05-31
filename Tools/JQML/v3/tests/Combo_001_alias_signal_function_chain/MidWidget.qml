import QtQuick

BaseWidget {
    id: root

    property int midValue: baseValue + 3
    property alias helperText: helper.text

    signal midSignal(int amount)

    function summary() {
        return makeMessage("mid") + ":" + helperText
    }

    Text {
        id: helper
        text: "mid"
    }

    Component.onCompleted: {
        console.log("mid", midValue, helperText, summary())
    }
}
