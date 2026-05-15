import QtQuick

MidAlias {
    id: root

    signal mainSignal()

    onBaseSignal: {
        console.log("combo14-base")
    }

    onMidSignal: {
        console.log("combo14-mid")
    }

    onMainSignal: {
        console.log("combo14-main")
    }

    Component.onCompleted: {
        root.titleText = "root"
        root.detailText = "leaf"
        baseSignal()
        midSignal()
        mainSignal()
        console.log("combo14", root.baseValue, root.midValue, root.titleText, root.detailText, summary())
        Qt.quit()
    }
}