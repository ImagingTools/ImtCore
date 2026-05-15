import QtQuick

MidRect {
    id: root

    signal mainPing()

    onBasePing: {
        console.log("combo11-ping")
    }

    onMainPing: {
        console.log("combo11-mainPing")
    }

    Component.onCompleted: {
        root.titleText = "root"
        root.detailText = "leaf"
        basePing("hello")
        mainPing()
        console.log("combo11", root.baseWidth, root.extraWidth, root.titleText, root.detailText, describe(), merged())
        Qt.quit()
    }
}