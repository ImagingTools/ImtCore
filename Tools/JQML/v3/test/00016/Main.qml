import QtQuick

Item {
    readonly property int value: 100

    Component.onCompleted: {
        console.log("value =", value)
        try {
            value = 200
            console.log("Ошибка: readonly свойство удалось изменить")
        } catch (e) {
            console.log("OK: readonly свойство защищено от записи")
        }
    }
}