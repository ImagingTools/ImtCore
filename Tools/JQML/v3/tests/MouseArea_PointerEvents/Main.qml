import QtQuick
import QtTest

Item {
    id: root
    width: 240
    height: 180

    property int clickedCount: 0
    property int doubleClickedCount: 0
    property int canceledCount: 0
    property int releasedCount: 0
    property int pressedChangedCount: 0

    MouseArea {
        id: area
        x: 20
        y: 20
        width: 120
        height: 100

        onClicked: root.clickedCount += 1
        onDoubleClicked: root.doubleClickedCount += 1
        onCanceled: root.canceledCount += 1
        onReleased: root.releasedCount += 1
        onPressedChanged: root.pressedChangedCount += 1
    }

    MouseArea {
        id: otherArea
        x: 160
        y: 20
        width: 60
        height: 100
    }

    Timer {
        id: suppressTimer
        interval: 1100
        onTriggered: {
            try {
                if(tc.runAfterTouchSuppression()) console.log("pointer-events: ok")
            } catch(err) {
                console.log("pointer-events-error", err)
            }
            Qt.quit()
        }
    }

    TestCase {
        id: tc
        name: "MouseAreaPointerEvents"

        property int nextPointerId: 1

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

        function dispatchMouse(type, clientX, clientY) {
            let event = new window.MouseEvent(type, {
                bubbles: true,
                cancelable: true,
                clientX: clientX,
                clientY: clientY,
                button: 0,
                buttons: type === "mousedown" ? 1 : 0
            })
            window.dispatchEvent(event)
            return event
        }

        function verify(value, message) {
            if(!value) {
                console.log("pointer-events-error", message)
                return false
            }
            return true
        }

        function pressPoint(item) {
            let rect = item.__getDOM().getBoundingClientRect()
            return { x: rect.left + 23, y: rect.top + 32 }
        }

        function tap(item) {
            let point = pressPoint(item)
            let pointerId = tc.nextPointerId
            tc.nextPointerId = pointerId + 1
            dispatchPointer(item.__getDOM(), "pointerdown", pointerId, point.x, point.y)
            dispatchPointer(window, "pointerup", pointerId, point.x, point.y)
        }

        // A tap on another area makes the next tap on area a first click, not a double one.
        function startFirstClick() {
            tap(otherArea)
            root.clickedCount = 0
            root.doubleClickedCount = 0
            root.releasedCount = 0
            root.pressedChangedCount = 0
        }

        function checkTap() {
            startFirstClick()
            let dom = area.__getDOM()
            let point = pressPoint(area)
            dispatchPointer(dom, "pointerdown", 1000, point.x, point.y)
            if(!verify(area.pressed, "pointerdown did not set pressed")) return false
            dispatchPointer(window, "pointerup", 1000, point.x, point.y)
            if(!verify(!area.pressed, "pointerup did not clear pressed")) return false
            if(!verify(root.pressedChangedCount === 2, "pressedChanged was not emitted on release")) return false
            if(!verify(root.releasedCount === 1, "pointerup did not emit released")) return false
            if(!verify(root.clickedCount === 1, "pointerup did not emit clicked")) return false

            dispatchMouse("mousedown", point.x, point.y)
            dispatchMouse("mouseup", point.x, point.y)
            dispatchMouse("click", point.x, point.y)
            return verify(root.clickedCount === 1, "emulated mouse events duplicated the click")
        }

        function checkDoubleTap() {
            startFirstClick()
            tap(area)
            tap(area)
            if(!verify(root.doubleClickedCount === 1, "second tap did not emit doubleClicked")) return false
            return verify(!area.pressed && !area.__pressed, "double tap left the area pressed")
        }

        function checkScrolledCoordinates() {
            let html = window.document.documentElement
            let body = window.document.body
            html.style.overflow = "auto"
            html.style.height = "auto"
            html.style.minHeight = "1200px"
            body.style.height = "auto"
            body.style.minHeight = "1200px"
            window.scrollTo(0, 30)
            if(!verify(window.scrollY > 0, "test page did not scroll")) return false

            startFirstClick()
            tap(area)
            let ok = verify(root.clickedCount === 1, "tap missed the area after scrolling")
                && verify(area.mouseX === 23 && area.mouseY === 32, "pointer coordinates were not viewport-relative after scrolling")

            window.scrollTo(0, 0)
            html.style.overflow = ""
            html.style.height = ""
            html.style.minHeight = ""
            body.style.height = ""
            body.style.minHeight = ""
            return ok
        }

        function checkEditableTarget() {
            let input = window.document.createElement("input")
            input.style.position = "absolute"
            input.style.left = "10px"
            input.style.top = "10px"
            input.style.width = "30px"
            input.style.height = "20px"
            area.__getDOM().appendChild(input)
            let rect = input.getBoundingClientRect()
            let pointerDown = dispatchPointer(input, "pointerdown", 1001, rect.left + 5, rect.top + 5)
            let ok = verify(!pointerDown.defaultPrevented, "editable target default was prevented")
                && verify(area.pressed, "editable target did not reach MouseArea")
            dispatchPointer(window, "pointerup", 1001, rect.left + 5, rect.top + 5)
            input.remove()
            return ok
        }

        function checkCancel() {
            let dom = area.__getDOM()
            let point = pressPoint(area)
            root.canceledCount = 0

            dispatchPointer(dom, "pointerdown", 1002, point.x, point.y)
            dispatchPointer(window, "pointercancel", 1002, point.x, point.y)
            if(!verify(!area.pressed && !area.__pressed, "pointercancel did not clear pressed")) return false
            if(!verify(root.canceledCount === 1, "enabled MouseArea did not emit canceled")) return false

            dispatchPointer(dom, "pointerdown", 1003, point.x, point.y)
            area.enabled = false
            dispatchPointer(window, "pointercancel", 1003, point.x, point.y)
            area.enabled = true
            // Qt: MouseArea.enabled does not end a press, so the cancel still reaches it
            if(!verify(!area.pressed && !area.__pressed, "disabled MouseArea remained pressed after pointercancel")) return false
            if(!verify(root.canceledCount === 2, "MouseArea disabled during the press did not emit canceled")) return false

            dispatchPointer(dom, "pointerdown", 1004, point.x, point.y)
            area.visible = false
            dispatchPointer(window, "pointercancel", 1004, point.x, point.y)
            area.visible = true
            // Qt: hiding ends the press at once (ungrabMouse), the later cancel finds nothing
            if(!verify(!area.pressed && !area.__pressed, "hidden MouseArea remained pressed after pointercancel")) return false
            return verify(root.canceledCount === 3, "hidden MouseArea did not emit canceled once")
        }

        function checkDisabledBeforeRelease() {
            let dom = area.__getDOM()
            let point = pressPoint(area)

            startFirstClick()
            dispatchPointer(dom, "pointerdown", 1005, point.x, point.y)
            area.enabled = false
            dispatchPointer(window, "pointerup", 1005, point.x, point.y)
            area.enabled = true
            // Qt: MouseArea.enabled does not end a press, the release is still a click
            if(!verify(!area.pressed && !area.__pressed, "area disabled before the release remained pressed")) return false
            if(!verify(root.clickedCount === 1, "area disabled before the release did not emit clicked")) return false

            startFirstClick()
            tap(area)
            dispatchPointer(dom, "pointerdown", 1006, point.x, point.y)
            area.enabled = false
            dispatchPointer(window, "pointerup", 1006, point.x, point.y)
            area.enabled = true
            // Qt: doubleClicked comes with the second press, before the area is disabled
            if(!verify(!area.pressed && !area.__pressed, "area disabled before a double-click release remained pressed")) return false
            if(!verify(root.doubleClickedCount === 1, "second press did not emit doubleClicked")) return false

            startFirstClick()
            tap(area)
            dispatchPointer(dom, "pointerdown", 1007, point.x, point.y)
            area.visible = false
            dispatchPointer(window, "pointerup", 1007, point.x, point.y)
            area.visible = true
            return verify(!area.pressed && !area.__pressed, "area hidden before a double-click release remained pressed")
        }

        function checkReleaseOutside() {
            let dom = area.__getDOM()
            let point = pressPoint(area)
            let rect = dom.getBoundingClientRect()
            let outsideX = rect.right + 5
            let outsideY = rect.top + 32

            startFirstClick()
            dispatchPointer(dom, "pointerdown", 1008, point.x, point.y)
            dispatchPointer(window, "pointermove", 1008, outsideX, outsideY)
            dispatchPointer(window, "pointerup", 1008, outsideX, outsideY)
            if(!verify(root.clickedCount === 0, "release outside the area emitted clicked")) return false
            if(!verify(root.releasedCount === 1, "release outside the area did not emit released")) return false
            if(!verify(!area.pressed && !area.__pressed, "release outside the area left it pressed")) return false
            if(!verify(root.pressedChangedCount === 2, "release outside the area did not emit pressedChanged")) return false
            if(!verify(!area.containsMouse, "release outside the area left containsMouse set")) return false

            tap(area)
            if(!verify(root.clickedCount === 1 && root.doubleClickedCount === 0, "tap after a release outside was not a first click")) return false

            // Qt: moving out and back in before the release is still a click.
            startFirstClick()
            dispatchPointer(dom, "pointerdown", 1009, point.x, point.y)
            dispatchPointer(window, "pointermove", 1009, outsideX, outsideY)
            dispatchPointer(window, "pointermove", 1009, point.x, point.y)
            dispatchPointer(window, "pointerup", 1009, point.x, point.y)
            if(!verify(root.clickedCount === 1, "release back inside the area did not emit clicked")) return false

            // Qt: the second press is the double click, wherever it is released; the press after it starts again
            startFirstClick()
            tap(area)
            dispatchPointer(dom, "pointerdown", 1010, point.x, point.y)
            if(!verify(root.doubleClickedCount === 1, "second press did not emit doubleClicked")) return false
            dispatchPointer(window, "pointerup", 1010, outsideX, outsideY)
            tap(area)
            return verify(root.doubleClickedCount === 1 && root.clickedCount === 2, "press after a double click was not a first click")
        }

        // The press enters the area through the first move, so it holds containsMouse.
        function pressAndEnter(pointerId, point) {
            dispatchPointer(area.__getDOM(), "pointerdown", pointerId, point.x, point.y)
            dispatchPointer(window, "pointermove", pointerId, point.x + 1, point.y)
            return verify(area.containsMouse, "move during a press did not enter the area")
        }

        function checkHoverCleared() {
            let point = pressPoint(area)

            if(!pressAndEnter(1011, point)) return false
            area.enabled = false
            dispatchPointer(window, "pointerup", 1011, point.x, point.y)
            area.enabled = true
            if(!verify(!area.containsMouse, "area disabled before the release kept containsMouse")) return false

            if(!pressAndEnter(1012, point)) return false
            area.visible = false
            dispatchPointer(window, "pointerup", 1012, point.x, point.y)
            area.visible = true
            if(!verify(!area.containsMouse, "area hidden before the release kept containsMouse")) return false

            if(!pressAndEnter(1013, point)) return false
            dispatchPointer(window, "pointercancel", 1013, point.x, point.y)
            if(!verify(!area.containsMouse, "pointercancel kept containsMouse")) return false

            if(!pressAndEnter(1014, point)) return false
            area.enabled = false
            dispatchPointer(window, "pointercancel", 1014, point.x, point.y)
            area.enabled = true
            return verify(!area.containsMouse, "area disabled before pointercancel kept containsMouse")
        }

        function checkContextMenuWithoutPress(message) {
            let event = dispatchMouse("contextmenu", 5, 5)
            return verify(event.defaultPrevented, message)
        }

        function runAfterTouch() {
            return checkTap()
                && checkDoubleTap()
                && checkScrolledCoordinates()
                && checkEditableTarget()
                && checkCancel()
                && checkDisabledBeforeRelease()
                && checkReleaseOutside()
                && checkHoverCleared()
                && checkContextMenuWithoutPress("contextmenu right after touch was not suppressed")
        }

        function runAfterTouchSuppression() {
            if(!checkContextMenuWithoutPress("contextmenu without a press was not handled")) return false

            // No tap here: it would reopen the suppression window.
            root.clickedCount = 0
            let point = pressPoint(area)
            dispatchMouse("mousedown", point.x, point.y)
            if(!verify(area.pressed, "real mouse press was suppressed after the touch window")) return false
            dispatchMouse("mouseup", point.x, point.y)
            dispatchMouse("click", point.x, point.y)
            if(!verify(!area.pressed, "real mouse release left the area pressed")) return false
            if(!verify(root.clickedCount === 1, "real mouse click was suppressed after the touch window")) return false

            // Browsers send click to the common ancestor when the release lands elsewhere.
            let rect = area.__getDOM().getBoundingClientRect()
            root.clickedCount = 0
            dispatchMouse("mousedown", point.x, point.y)
            dispatchMouse("mouseup", rect.right + 5, point.y)
            dispatchMouse("click", rect.right + 5, point.y)
            if(!verify(root.clickedCount === 0, "mouse release outside the area emitted clicked")) return false
            return verify(!area.pressed && !area.__pressed, "mouse release outside the area left it pressed")
        }
    }

    Component.onCompleted: {
        if(typeof window === "undefined") {
            console.log("pointer-events: ok")
            Qt.quit()
            return
        }

        try {
            if(tc.runAfterTouch()) {
                suppressTimer.start()
                return
            }
        } catch(err) {
            console.log("pointer-events-error", err)
        }
        Qt.quit()
    }
}
