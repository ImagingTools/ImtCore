import QtQuick

BaseListView {
    id: root

    property string prefix: "mid"

    function message() {
        return prefix + ":" + entryCount()
    }

    onReady: {
        console.log("combo6-mid", message())
    }
}
