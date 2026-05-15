import QtQuick

Item {
    id: root

    property alias topLabel: mid.midLabel

    signal topStep()
    signal finished()

    MidAliasNode {
        id: mid

        onAfterMid: {
            console.log("slot-afterMid", root.topLabel)
            root.finished()
        }
    }

    onTopStep: {
        console.log("slot-top", topLabel)
        mid.midStep()
    }
}