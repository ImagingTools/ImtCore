import QtQuick

// The tab focus chain: nextItemInFocusChain() and the items it visits, in both directions
Window {
    id: root
    visible: true
    width: 300
    height: 300

    function name(item){
        return item ? (item.objectName !== "" ? item.objectName : "unnamed") : "null"
    }

    Column {
        TextInput { id: first; objectName: "first"; width: 100; height: 20; activeFocusOnTab: true }
        Rectangle {
            objectName: "withChildren"
            width: 100; height: 20
            activeFocusOnTab: true
            MouseArea { anchors.fill: parent }
            Text { text: "x" }
        }
        Item { objectName: "notInChain"; width: 10; height: 10 }
        Rectangle { objectName: "disabledStop"; width: 10; height: 10; activeFocusOnTab: true; enabled: false }
        Rectangle { objectName: "hiddenStop"; width: 10; height: 10; activeFocusOnTab: true; visible: false }
        Item {
            width: 100; height: 20
            visible: false
            TextInput { objectName: "insideHidden"; width: 10; height: 10; activeFocusOnTab: true }
        }
        Item {
            width: 100; height: 20
            TextInput { id: last; objectName: "nested"; width: 100; height: 20; activeFocusOnTab: true }
        }
    }

    Timer {
        interval: 200
        running: true
        onTriggered: root.check()
    }

    function check() {
        let forward = [name(first)]
        let item = first
        for (let i = 0; i < 4; i++){
            item = item.nextItemInFocusChain(true)
            forward.push(name(item))
        }
        let backward = [name(last)]
        item = last
        for (let j = 0; j < 4; j++){
            item = item.nextItemInFocusChain(false)
            backward.push(name(item))
        }
        console.log("forward:", forward.join(" "))
        console.log("backward:", backward.join(" "))
        Qt.quit();
    }
}
