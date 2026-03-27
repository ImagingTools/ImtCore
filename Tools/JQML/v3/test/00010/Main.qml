import QtQuick

Item {
    Component.onCompleted: {
        console.log("Родитель создан")
    }

    Item {
        Component.onCompleted: {
            console.log("Ребёнок создан")
        }
    }
}