import QtQuick

Item {
    ListModel {
        id: model
        ListElement { name: "Иван"; age: 25 }
        ListElement { name: "Мария"; age: 30 }
    }

    Component.onCompleted: {
        console.log("Имя первого:", model.get(0).name)
        console.log("Возраст второго:", model.get(1).age)
    }
}