import QtQuick

Item {
    width: 300
    height: 300

    Case { id: shown; label: "shown" }
    Case { id: hidden; label: "visible false"; viewVisible: false }
    Case { id: noWidth; label: "width 0"; viewWidth: 0 }
    Case { id: noHeight; label: "height 0"; viewHeight: 0 }
    Case { id: noSize; label: "size 0"; viewWidth: 0; viewHeight: 0 }
    Case { id: parentHidden; label: "parent hidden"; holderVisible: false }
    Case { id: transparent; label: "opacity 0"; viewOpacity: 0 }

    property int step: 0

    function reportAll() {
        shown.report()
        hidden.report()
        noWidth.report()
        noHeight.report()
        noSize.report()
        parentHidden.report()
        transparent.report()
    }

    Timer {
        interval: 300
        running: true
        repeat: true
        onTriggered: {
            if (step === 0) {
                console.log("-- at the start")
                reportAll()
                hidden.viewVisible = true
                noWidth.viewWidth = 200
                noHeight.viewHeight = 200
                noSize.viewWidth = 200
                noSize.viewHeight = 200
                parentHidden.holderVisible = true
                transparent.viewOpacity = 1
            } else if (step === 1) {
                console.log("-- after they got a size and were shown")
                reportAll()
                hidden.viewVisible = false
                noHeight.viewHeight = 0
                noWidth.viewWidth = 0
            } else if (step === 2) {
                console.log("-- after they were hidden or emptied again")
                reportAll()
                Qt.quit()
            }
            step++
        }
    }
}
