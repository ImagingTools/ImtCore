import QtQuick

Item {
    property alias textValue: label.text

    Text {
        id: label
        text: "Исходный текст"
    }

    Component.onCompleted: {
        console.log("label.text =", label.text)
        textValue = "Новый текст"
        console.log("После изменения alias:")
        console.log("label.text =", label.text)
    }
}