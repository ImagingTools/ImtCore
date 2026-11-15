import QtQuick

// Mirrors GridCollectionViewBase: default sourceComponent, then use-site alias binding.
Item {
    id: root
    width: 200
    height: 120

    Item {
        id: collection
        anchors.fill: parent
        property alias extendingInfoComp: extendingInfoLoader.sourceComponent
        property bool openST: true
        extendingInfoComp: detailComp

        Item {
            id: extendingInfoLoaderContainer
            anchors.top: parent.top
            anchors.topMargin: 20
            width: parent.width - 20
            height: 90
            visible: collection.openST

            Loader {
                id: extendingInfoLoader
                anchors.fill: parent
                sourceComponent: extendingInfoCompDefault
            }
        }

        Component {
            id: extendingInfoCompDefault
            Rectangle {
                color: "#ffffff"
                objectName: "default"
            }
        }
    }

    Component {
        id: detailComp
        Rectangle {
            width: 1000
            height: 300
            color: "lightblue"
            objectName: "detail"
            Text {
                anchors.centerIn: parent
                text: "Vita nostra brevis est"
            }
        }
    }

    Component.onCompleted: {
        console.log("name", extendingInfoLoader.item.objectName)
        console.log("size", extendingInfoLoader.item.width, extendingInfoLoader.item.height)
        Qt.quit()
    }
}
