import QtQuick 2.0

// Mirrors ProductEditor default properties and geometry:
//   softwareCategoryId: "Software"
//   hardwareCategoryId: "Hardware"
//   index: -1
//   instanceCount: 1
// Also replicates a content loader + loading overlay pattern
// (contentLoader fills parent; loading indicator is centered).

Item {
    id: root
    width: 800
    height: 600

    Item {
        id: productEditor
        anchors.fill: parent

        property string softwareCategoryId: "Software"
        property string hardwareCategoryId: "Hardware"
        property int index: -1
        property int instanceCount: 1
        property string orderId: ""

        // content loader fills parent
        Item {
            id: contentLoader
            anchors.fill: parent
        }

        // loading overlay - centered, fixed size
        Item {
            id: loading
            width: 80
            height: 80
            anchors.centerIn: parent
            visible: false
        }
    }

    Component.onCompleted: {
        console.log(
            "prolife25",
            root.width,
            root.height,
            productEditor.width,
            productEditor.height,
            productEditor.softwareCategoryId,
            productEditor.hardwareCategoryId,
            productEditor.index,
            productEditor.instanceCount,
            contentLoader.x,
            contentLoader.y,
            contentLoader.width,
            contentLoader.height,
            loading.width,
            loading.height,
            Math.round(loading.x),
            Math.round(loading.y),
            loading.visible
        )
        Qt.quit()
    }
}
