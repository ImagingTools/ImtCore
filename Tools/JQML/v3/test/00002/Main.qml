import QtQuick

Item {
    property int value: 0
    onValueChanged: console.log("Сигнал valueChanged:", value)

    Component.onCompleted: {
        value = 10
        value = 20
        console.log("Проверка: последнее значение value =", value)
    }
}