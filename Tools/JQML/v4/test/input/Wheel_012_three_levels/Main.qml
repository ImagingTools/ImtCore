import QtQuick
import QtTest

Item {
    id: root
    width: 300
    height: 300

    Flickable {
        id: outer
        width: 200
        height: 200
        contentWidth: 200
        contentHeight: 3000
        boundsBehavior: Flickable.StopAtBounds
        clip: true

        ListView {
            id: middle
            width: 200
            height: 3000
            model: 3
            boundsBehavior: Flickable.StopAtBounds
            delegate: Item {
                width: 200
                height: 1100

                GridView {
                    objectName: "grid"
                    width: 200
                    height: 1100
                    cellWidth: 100
                    cellHeight: 100
                    model: 24
                    boundsBehavior: Flickable.StopAtBounds
                    delegate: Rectangle { width: 100; height: 100 }
                }
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

        function grid() { return find(middle, "grid") }

        function test_wheel() {
            var plan = [-120, -120, -120, -120, -120, -120, -120, -120, -120, -120, 120, 120, 120, 120, 120, 120, 120, 120, 120, 120]
            var out = []
            for (var i = 0; i < plan.length; i++) {
                mouseWheel(root, 50, 50, 0, plan[i])
                wait(350)
                out.push([r(outer.contentY), r(middle.contentY), r(grid().contentY)])
            }
            console.log("outer, middle, grid", JSON.stringify(out))
        }
    }
}
