import QtQuick

Item {
    property bool attachedProperty: false

    Text {
        id: textElement
        anchors.centerIn: parent
        text: parent.attachedProperty ? "Включено" : "Выключено"
    }
    
    Component.onCompleted: {
        console.log("Изначальное состояние:", textElement.text)
        attachedProperty = true
        console.log("После изменения:", textElement.text)
    }
}