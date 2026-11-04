import QtQuick
import QtTest

Item {
    id: root
    width: 240
    height: 180

    property int clickedCount: 0
    property int canceledCount: 0
    property int pressedChangedCount: 0

    MouseArea {
        id: area
        x: 20
        y: 20
        width: 120
        height: 100

        onClicked: root.clickedCount += 1
        onCanceled: root.canceledCount += 1
        onPressedChanged: root.pressedChangedCount += 1
    }

    TestCase {
        id: tc
        name: "MouseAreaPointerEvents"

        function dispatchPointer(target, type, pointerId, clientX, clientY) {
            let event = new window.PointerEvent(type, {
                bubbles: true,
                cancelable: true,
                pointerId: pointerId,
                pointerType: "touch",
                isPrimary: true,
                clientX: clientX,
                clientY: clientY,
                button: 0,
                buttons: type === "pointerup" || type === "pointercancel" ? 0 : 1
            })
            target.dispatchEvent(event)
            return event
        }

        function verify(value, message) {
            if(!value) {
                console.log("pointer-events-error", message)
                return false
            }
            return true
        }

        function runScenario() {
            if(typeof window === "undefined" || typeof PointerEvent === "undefined") {
                console.log("pointer-events: ok")
                return
            }

            let dom = area.__getDOM()
            let rect = dom.getBoundingClientRect()
            let x = rect.left + 23
            let y = rect.top + 32

            root.clickedCount = 0
            root.pressedChangedCount = 0
            dispatchPointer(dom, "pointerdown", 1, x, y)
            if(!verify(area.pressed, "pointerdown did not set pressed")) return
            dispatchPointer(window, "pointerup", 1, x, y)
            if(!verify(!area.pressed, "pointerup did not clear pressed")) return
            if(!verify(root.pressedChangedCount === 2, "pressedChanged was not emitted on release")) return
            if(!verify(root.clickedCount === 1, "pointerup did not emit clicked")) return

            window.dispatchEvent(new window.MouseEvent("mousedown", { bubbles: true, clientX: x, clientY: y, button: 0, buttons: 1 }))
            window.dispatchEvent(new window.MouseEvent("mouseup", { bubbles: true, clientX: x, clientY: y, button: 0, buttons: 0 }))
            window.dispatchEvent(new window.MouseEvent("click", { bubbles: true, clientX: x, clientY: y, button: 0, buttons: 0 }))
            if(!verify(root.clickedCount === 1, "emulated mouse events duplicated the click")) return

            document.documentElement.style.minHeight = "1200px"
            document.body.style.minHeight = "1200px"
            window.scrollTo(0, 30)
            if(!verify(window.scrollY > 0, "test page did not scroll")) return
            rect = dom.getBoundingClientRect()
            x = rect.left + 23
            y = rect.top + 32
            dispatchPointer(dom, "pointerdown", 2, x, y)
            dispatchPointer(window, "pointerup", 2, x, y)
            if(!verify(area.mouseX === 23 && area.mouseY === 32, "pointer coordinates were not viewport-relative after scrolling")) return
            window.scrollTo(0, 0)
            rect = dom.getBoundingClientRect()
            x = rect.left + 23
            y = rect.top + 32

            let input = document.createElement("input")
            input.style.position = "absolute"
            input.style.left = "10px"
            input.style.top = "10px"
            input.style.width = "30px"
            input.style.height = "20px"
            dom.appendChild(input)
            rect = input.getBoundingClientRect()
            let pointerDown = dispatchPointer(input, "pointerdown", 3, rect.left + 5, rect.top + 5)
            if(!verify(!pointerDown.defaultPrevented, "editable target default was prevented")) return
            if(!verify(area.pressed, "editable target did not reach MouseArea")) return
            dispatchPointer(window, "pointerup", 3, rect.left + 5, rect.top + 5)
            input.remove()

            root.canceledCount = 0
            dispatchPointer(dom, "pointerdown", 4, x, y)
            dispatchPointer(window, "pointercancel", 4, x, y)
            if(!verify(!area.pressed && !area.__pressed, "pointercancel did not clear pressed")) return
            if(!verify(root.canceledCount === 1, "enabled MouseArea did not emit canceled")) return

            dispatchPointer(dom, "pointerdown", 5, x, y)
            area.enabled = false
            dispatchPointer(window, "pointercancel", 5, x, y)
            if(!verify(!area.pressed && !area.__pressed, "disabled MouseArea remained pressed after pointercancel")) return
            if(!verify(root.canceledCount === 1, "disabled MouseArea emitted canceled")) return
            area.enabled = true

            dispatchPointer(dom, "pointerdown", 6, x, y)
            area.visible = false
            dispatchPointer(window, "pointercancel", 6, x, y)
            if(!verify(!area.pressed && !area.__pressed, "hidden MouseArea remained pressed after pointercancel")) return
            if(!verify(root.canceledCount === 1, "hidden MouseArea emitted canceled")) return
            area.visible = true

            dispatchPointer(dom, "pointerdown", 7, x, y)
            area.enabled = false
            dispatchPointer(window, "pointerup", 7, x, y)
            if(!verify(!area.pressed && !area.__pressed, "disabled MouseArea remained pressed after pointerup")) return
            area.enabled = true

            console.log("pointer-events: ok")
        }
    }

    Component.onCompleted: {
        try {
            tc.runScenario()
        } catch(err) {
            console.log("pointer-events-error", err)
        }
        Qt.quit()
    }
}
