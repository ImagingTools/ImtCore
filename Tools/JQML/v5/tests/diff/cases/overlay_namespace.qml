import QtQuick
import QtQuick.Templates as T
import QtTest

// T.Overlay.modal / T.Overlay.overlay: the attached Overlay of a type of a namespace (ActionMenu of QmlAppTemplate)
Item {
    id: root
    width: 200; height: 200
    T.Popup {
        id: popup
        parent: T.Overlay.overlay
        modal: true
        dim: true
        width: 50; height: 50
        T.Overlay.modal: Rectangle { objectName: "dimmer"; color: "#80ff0000" }
    }
    TestCase {
        name: "OverlayNamespace"
        when: windowShown
        function test_modal() {
            popup.open()
            wait(50)
            let overlay = popup.parent
            console.log(overlay.width, overlay.height, popup.visible)
            let names = []
            for (let i = 0; i < overlay.children.length; i++) names.push(overlay.children[i].objectName || "item")
            console.log(names.join(" "))
            for (let i = 0; i < overlay.children.length; i++) if (overlay.children[i].objectName === "dimmer") console.log(overlay.children[i].color, overlay.children[i].width, overlay.children[i].height)
            popup.close()
            wait(50)
        }
    }
}
