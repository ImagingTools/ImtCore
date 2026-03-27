import QtQuick

Item {
    property var items: ["A", "B", "C"]

    Repeater {
        model: parent.items
        delegate: Text { text: modelData }
    }

    Component.onCompleted: {
        console.log("Количество элементов:", children.length)
        console.log("Первый текст:", children[0].text)
    }
}