import QtQuick
import QtQuick.Layouts

// LayoutItemProxy test:
// A Rectangle (target) is controlled by a LayoutItemProxy inside a RowLayout.
// The proxy fills the layout, so the target should be sized to match the proxy.
Item {
    width: 300
    height: 100

    // Target lives outside any layout
    Rectangle {
        id: target
        objectName: "target"
        color: "red"
        width: 10
        height: 10
        visible: false
    }

    RowLayout {
        id: row
        width: 300
        height: 100
        spacing: 0

        LayoutItemProxy {
            id: proxy
            objectName: "proxy"
            target: target
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }

    Component.onCompleted: {
        // After completion the proxy is visible and owns the target:
        // target should be sized to proxy size (= layout size = 300x100)
        console.log("proxy.visible", proxy.visible)
        console.log("target.visible", target.visible)
        console.log("proxy.width",  proxy.width)
        console.log("proxy.height", proxy.height)
        console.log("target.width",  target.width)
        console.log("target.height", target.height)

        // Hiding the proxy should hide the target
        proxy.visible = false
        console.log("after hide: target.visible", target.visible)

        // Showing proxy again — target should reappear and resize
        proxy.visible = true
        console.log("after show: target.visible", target.visible)
        console.log("after show: target.width",  target.width)

        Qt.quit()
    }
}
