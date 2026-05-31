import QtQuick

Item {
    id: root

    EmitterChild {
        id: sourceA
        offset: 0
    }

    EmitterChild {
        id: sourceB
        offset: 5
    }

    CollectorNode {
        id: collector
    }

    Component.onCompleted: {
        collector.source = sourceA
        collector.source.tick.connect(collector.dynTick)
        sourceA.fireWithOffset("A", 2)

        collector.source.tick.disconnect(collector.dynTick)

        collector.source = sourceB
        collector.source.tick.connect(collector.dynTick)
        sourceB.fireWithOffset("B", 4)

        collector.source.tick.disconnect(collector.dynTick)

        console.log("combo29-final", collector.mirrored, collector.last, collector.dynCount)
        Qt.quit()
    }
}
