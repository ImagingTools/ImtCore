import QtQuick

Item {
    id: root
    width: StyleLike.sizeHintXS

    Rectangle {
        id: box
        width: root.width
        height: box.width
        color: "red"
    }
}
