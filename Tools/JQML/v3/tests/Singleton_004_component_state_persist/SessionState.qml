pragma Singleton
import QtQuick

QtObject {
    property int stepCount: 0
    property string lastTag: ""
    property string trace: ""

    function reset() {
        stepCount = 0
        lastTag = ""
        trace = ""
    }

    function step(tag) {
        stepCount += 1
        lastTag = tag
        trace = trace + "|" + tag + ":" + stepCount
    }
}
