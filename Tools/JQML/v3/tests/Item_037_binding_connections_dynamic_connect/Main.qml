import QtQuick

Item {
    id: root

    property var activeEmitter: emitterA
    property int dynHits: 0

    function dynHandler(tag) {
        dynHits += 1
        console.log("item37-dyn", tag, dynHits, sink.boundValue)
    }

    Item {
        id: emitterA
        property int source: 0
        signal ping(string tag)
    }

    Item {
        id: emitterB
        property int source: 0
        signal ping(string tag)
    }

    Item {
        id: sink
        property int boundValue: 0
        property string lastTag: ""
    }

    Binding {
        target: sink
        property: "boundValue"
        value: root.activeEmitter.source * 10
    }

    Connections {
        target: root.activeEmitter
        function onPing(tag) {
            sink.lastTag = tag
            console.log("item37-conn", tag, sink.boundValue)
        }
    }

    Component.onCompleted: {
        activeEmitter.ping.connect(dynHandler)

        emitterA.source = 2
        activeEmitter.ping("A1")

        activeEmitter = emitterB
        emitterB.source = 7
        activeEmitter.ping.connect(dynHandler)
        activeEmitter.ping("B1")

        activeEmitter.ping.disconnect(dynHandler)

        console.log("item37-final", sink.boundValue, sink.lastTag, dynHits)
        Qt.quit()
    }
}
