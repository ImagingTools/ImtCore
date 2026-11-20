import QtQuick

Item {
    id: root
    width: 100
    height: 100
    property var types: [Easing.InQuad, Easing.OutQuad, Easing.InOutQuad, Easing.OutInQuad, Easing.OutCubic, Easing.InOutQuart, Easing.OutQuint, Easing.InSine, Easing.InOutSine, Easing.InExpo, Easing.OutExpo, Easing.InOutExpo, Easing.OutCirc, Easing.InOutCirc, Easing.InElastic, Easing.OutElastic, Easing.InOutElastic, Easing.InBack, Easing.OutBack, Easing.InOutBack, Easing.OutBounce, Easing.InBounce, Easing.InOutBounce, Easing.OutInElastic]
    property var samples: ({})
    property int index: 0
    property real startedAt: 0

    Item {
        id: probe
        property real v: 0
        onVChanged: {
            if (root.startedAt === 0) return
            var key = "t" + root.types[root.index]
            if (!root.samples[key]) root.samples[key] = []
            root.samples[key].push([Date.now() - root.startedAt, v])
        }
    }
    NumberAnimation {
        id: anim
        target: probe
        property: "v"
        from: 0
        to: 1
        duration: 2000
        onStopped: {
            var key = "t" + root.types[root.index]
            var list = root.samples[key] || []
            console.log("EASING", root.types[root.index], JSON.stringify(list.map(function (p) { return [Math.round(p[0]), Math.round(p[1] * 10000) / 10000] })))
            root.index++
            root.startedAt = 0
            probe.v = 0
            if (root.index < root.types.length) next.start()
            else Qt.quit()
        }
    }
    Timer {
        id: next
        interval: 30
        onTriggered: { anim.easing.type = root.types[root.index]; root.startedAt = Date.now(); anim.start() }
    }
    Component.onCompleted: next.start()
}
