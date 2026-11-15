import QtQuick

Item {
    id: menuLike
    property int hits: 0
    property TreeItemModel model: TreeItemModel {}
    onModelChanged: hits += 1
}
