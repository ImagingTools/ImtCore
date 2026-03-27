import QtQuick

Item {
    property Item child: Item {}

    Component.onCompleted: {
        child.destroy()
        console.log("child уничтожен")
    }
}