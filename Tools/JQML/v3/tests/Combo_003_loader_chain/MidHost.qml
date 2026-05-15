import QtQuick

BaseHost {
    id: root

    property int extraWidth: 4

    function summary() {
        return extraWidth + ":" + loadedMetrics()
    }
}
