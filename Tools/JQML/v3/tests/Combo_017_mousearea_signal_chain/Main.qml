import QtQuick

MidMouse {
    id: root

    signal mainHit()

    onBaseHit: {
        console.log("combo17-base")
    }

    onMidHit: {
        console.log("combo17-mid")
    }

    onMainHit: {
        console.log("combo17-main")
    }

    Component.onCompleted: {
        root.baseHit()
        root.midHit()
        root.mainHit()
        console.log("combo17", root.baseState, root.midState, root.summary())
        Qt.quit()
    }
}