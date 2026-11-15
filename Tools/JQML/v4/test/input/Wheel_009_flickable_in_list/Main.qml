import QtQuick
import QtTest

Item {
    id: root
    width: 300
    height: 300

    ListView {
        id: outer
        width: 200
        height: 200
        model: 4
        boundsBehavior: Flickable.StopAtBounds
        clip: true
        delegate: Item {
            width: 200
            height: 600

            Flickable {
                objectName: "flick"
                anchors.fill: parent
                contentWidth: 200
                contentHeight: 100
                boundsBehavior: Flickable.StopAtBounds
            }
        }
    }

    TestCase {
        id: tc
        name: "wheel"
        when: windowShown

        function find(parent, name) {
            for (var i = 0; i < parent.children.length; i++) {
                var c = parent.children[i]
                if (c.objectName === name) return c
                var f = find(c, name)
                if (f) return f
            }
            return null
        }

        function r(v) { return Math.round(v) }

        function flick() { return find(outer, "flick") }

        function test_wheel() {
            var plan = [-120, -120, -120, -120, -120, -120, 120, 120, 120, 120, 120, 120, 120, 120]
            var out = []
            for (var i = 0; i < plan.length; i++) {
                mouseWheel(root, 50, 50, 0, plan[i])
                wait(350)
                out.push([r(outer.contentY), r(flick().contentY)])
            }
            console.log("outer, flick", JSON.stringify(out))
        }
    }
}
