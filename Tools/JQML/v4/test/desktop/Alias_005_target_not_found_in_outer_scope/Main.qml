import QtQuick

Item {
    id: window

    // A page that is loaded from a file inside of a delegate: the alias of View.qml to its own `container` must not
    // reach the `container` of the delegate while its own item is not created yet.
    Repeater {
        model: 1
        delegate: Item {
            Item {
                id: container
                property string tag: "outer"
            }

            Loader {
                id: loader
                source: "Page.qml"
            }

            Timer {
                interval: 50
                running: true
                onTriggered: {
                    console.log("view", loader.item.children[0].tag)
                    console.log("outer", container.tag)
                    Qt.quit()
                }
            }
        }
    }
}
