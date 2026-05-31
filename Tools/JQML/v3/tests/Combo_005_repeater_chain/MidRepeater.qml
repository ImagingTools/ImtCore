import QtQuick

BaseRepeater {
    id: root

    property int bonus: 2

    function combinedCount() {
        return totalCount() + bonus
    }

    onAssembled: {
        console.log("combo5-mid", combinedCount())
    }
}
