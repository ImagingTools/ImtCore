import QtQuick

// `property TreeItemModel model: TreeItemModel {}` in MenuLike is a default: the use site's binding
// `model: manager.pageModel` replaces it, so the instance follows the use site and emits modelChanged for it.
Item {
    id: root

    Item {
        id: manager
        property TreeItemModel pageModel: TreeItemModel {}
    }

    MenuLike {
        id: menu
        model: manager.pageModel
    }

    Component.onCompleted: {
        console.log("hits", menu.hits)
        console.log("same", menu.model === manager.pageModel)
        Qt.quit()
    }
}
