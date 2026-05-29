import QtQuick
import QtQuick.Layouts

Item {
    StackLayout {
        id: stack
        width: 200
        height: 150
        currentIndex: 1

        Rectangle { color: "red";   width: 100; height: 80  }
        Rectangle { color: "green"; width: 120; height: 60  }
        Rectangle { color: "blue";  width: 80;  height: 100 }
    }

    Component.onCompleted: {
        console.log("stack.count", stack.count)
        console.log("stack.currentIndex", stack.currentIndex)
        console.log("c0.w", stack.children[0].width)
        console.log("c0.h", stack.children[0].height)
        console.log("c0.vis", stack.children[0].visible)
        console.log("c1.w", stack.children[1].width)
        console.log("c1.h", stack.children[1].height)
        console.log("c1.vis", stack.children[1].visible)
        console.log("c2.vis", stack.children[2].visible)
        stack.currentIndex = 0
        console.log("c0.vis2", stack.children[0].visible)
        console.log("c1.vis2", stack.children[1].visible)
        Qt.quit()
    }
}
