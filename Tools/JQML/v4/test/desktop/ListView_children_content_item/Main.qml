import QtQuick

// The visual children of a ListView are in its contentItem, which is the only entry of `children` (as for a Flickable): the delegate of
// row i is `view.children[0].children[i]`, and a Loader delegate gives its loaded item with `.item`. InputView of ImtCore sets the
// parameters of a row that way.
Item {
    width: 200
    height: 200

    ListView {
        id: view
        width: 200
        height: 100
        model: 3
        delegate: Loader {
            width: 100
            height: 20
            sourceComponent: Component {
                Item {
                    property string tag: "row"
                }
            }
        }

        Component.onCompleted: {
            console.log("children", view.children.length, view.children[0] === view.contentItem)
            var content = view.children[0]
            console.log("content", content.children.length >= 3, content.children[0].item !== null && content.children[0].item !== undefined)
            content.children[1].item.tag = "second"
            console.log("tags", content.children[0].item.tag, content.children[1].item.tag, content.children[2].item.tag)
            console.log("parent", content.children[0].parent === content, content.parent === view)
            Qt.quit()
        }
    }
}
