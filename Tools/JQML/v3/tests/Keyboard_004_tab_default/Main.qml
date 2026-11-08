import QtQuick

Item {
    id: root
    width: 200
    height: 120

    property bool acceptTab: true
    property int tabCount: 0
    property int backtabCount: 0

    Rectangle {
        id: focusRect
        anchors.fill: parent
        color: "#1a1a2e"
        focus: true

        Keys.onTabPressed: function(event) {
            root.tabCount += 1
            event.accepted = root.acceptTab
        }
        Keys.onBacktabPressed: function(event) {
            root.backtabCount += 1
            event.accepted = root.acceptTab
        }
    }

    function sendTab(shift) {
        let event = new window.KeyboardEvent("keydown", { key: "Tab", code: "Tab", shiftKey: shift, bubbles: true, cancelable: true })
        window.dispatchEvent(event)
        return event.defaultPrevented
    }

    // An accepted Tab keeps the browser from moving the focus; an unaccepted one, with nothing to focus next, leaves the browser default
    Component.onCompleted: {
        if(typeof window === "undefined") {
            console.log("tab-default accepted true true 1 1")
            console.log("tab-default unaccepted false false 2 2")
            Qt.quit()
            return
        }

        try {
            focusRect.forceActiveFocus()
            root.acceptTab = true
            let tabPrevented = root.sendTab(false)
            let backtabPrevented = root.sendTab(true)
            console.log("tab-default accepted", tabPrevented, backtabPrevented, root.tabCount, root.backtabCount)

            root.acceptTab = false
            tabPrevented = root.sendTab(false)
            backtabPrevented = root.sendTab(true)
            console.log("tab-default unaccepted", tabPrevented, backtabPrevented, root.tabCount, root.backtabCount)
        } catch(err) {
            console.log("tab-default-error", err)
        }
        Qt.quit()
    }
}
