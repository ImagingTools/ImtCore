import QtQuick

MidWidget {
    id: root

    property int mainValue: midValue + 5
    property alias extraText: extra.text

    signal mainSignal()

    function all() {
        return summary() + ":" + extraText
    }

    Text {
        id: extra
        text: "main"
    }

    onPinged: console.log("pinged", text)
    onMidSignal: console.log("midSignal", amount)
    onMainSignal: console.log("mainSignal")

    Component.onCompleted: {
        console.log("combo1", baseValue, midValue, mainValue, labelText, helperText, extraText, makeMessage("x"), summary(), all())
        pinged("hello")
        midSignal(7)
        mainSignal()
        Qt.quit()
    }
}
