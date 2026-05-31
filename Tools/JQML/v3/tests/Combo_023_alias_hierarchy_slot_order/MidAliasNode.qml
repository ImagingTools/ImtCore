import QtQuick

Item {
    id: root

    property alias midLabel: leaf.label

    signal midStep()
    signal afterMid()

    LeafAliasNode {
        id: leaf
    }

    onMidStep: {
        console.log("slot-mid", midLabel)
        leaf.leafStep()
        afterMid()
    }
}