import QtQuick

Item {
    property string text: "Привет"

    Text {
        id: label
        text: parent.text + " мир"
    }

    Component.onCompleted: {
        console.log("label.text =", label.text)
        console.log("Проверка: label.text начинается с 'Привет'?", label.text.startsWith("Привет"))
    }
}