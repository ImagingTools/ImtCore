import QtQuick

// A page of a multi-page view is a Loader in a Repeater whose `active` is read from a ListModel. The view sets `loaded` in
// Component.onCompleted and asks for the page at once: the loader must have built it by then.
Item {
    id: root
    width: 200
    height: 200

    ListModel {
        id: pages
    }

    Repeater {
        id: repeater
        model: pages
        delegate: Loader {
            active: model.loaded === true
            sourceComponent: model.SourceComponent
        }
    }

    Component {
        id: pageComp
        Item {
            property string name: "page"
        }
    }

    function pageAt(index) {
        var holder = repeater.itemAt(index)
        if (!holder)
            return "no loader"
        return holder.item ? holder.item.name : "no item"
    }

    Component.onCompleted: {
        pages.append({ id: "a", SourceComponent: pageComp, loaded: false })
        pages.append({ id: "b", SourceComponent: pageComp, loaded: false })
        console.log("before", root.pageAt(0), root.pageAt(1))
        pages.setProperty(0, "loaded", true)
        console.log("after set", root.pageAt(0), root.pageAt(1))
        pages.setProperty(1, "loaded", true)
        console.log("after both", root.pageAt(0), root.pageAt(1))
        Qt.quit()
    }
}
