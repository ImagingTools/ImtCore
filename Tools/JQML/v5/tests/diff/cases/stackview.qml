import QtQuick
import QtQuick.Controls
import QtTest

// StackView: initialItem, push / pop / replace / clear (Immediate and with the transitions of the style), the attached
// StackView statuses and signals in their order, an item of its own going back to its parent, depth, busy, find
Item {
    id: root
    width: 400; height: 300
    property var log: []
    Component { id: page; Rectangle { property string name; objectName: name
        StackView.onActivating: log.push(name + " activating"); StackView.onActivated: log.push(name + " activated")
        StackView.onDeactivating: log.push(name + " deactivating"); StackView.onDeactivated: log.push(name + " deactivated")
        StackView.onRemoved: log.push(name + " removed") } }
    Rectangle { id: own; objectName: "own" }
    StackView { id: stack; width: 300; height: 200; initialItem: page; onCurrentItemChanged: log.push("current " + (currentItem ? currentItem.objectName : "null")) }
    TestCase { name: "Stack"; when: windowShown
        function flush(n) { console.log(n + ": " + log.join(", ")); log = [] }
        function test_stack() {
            console.log("init", stack.depth, stack.empty, stack.busy, stack.currentItem !== null, stack.currentItem.width, stack.currentItem.height, stack.currentItem.StackView.index, stack.currentItem.StackView.status, stack.currentItem.StackView.view === stack)
            log = []
            stack.push(page, { name: "B" }, StackView.Immediate); flush("push B immediate")
            console.log("depth", stack.depth, stack.get(0).StackView.status, stack.get(1).StackView.status, stack.get(0).visible, stack.get(1).width, stack.currentItem.objectName)
            stack.push(own); flush("push own"); console.log("busy", stack.busy, stack.depth, own.parent === stack, own.width)
            wait(600); flush("after transition"); console.log("busy", stack.busy, stack.get(1).visible, own.x)
            stack.pop(); wait(600); flush("pop"); console.log("own after pop", own.parent === stack, own.visible, stack.depth)
            stack.replace(page, { name: "C" }, StackView.Immediate); flush("replace"); console.log("depth", stack.depth, stack.currentItem.objectName)
            stack.pop(null, StackView.Immediate); flush("pop to first"); console.log("depth", stack.depth)
            stack.clear(); flush("clear"); console.log("empty", stack.empty, stack.depth, stack.currentItem)
            console.log("find", stack.find(function(item) { return item.objectName === "x" }))
        }
    }
}
