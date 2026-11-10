import QtQuick

Item {
    property string title
    property string backgroundColor: "black"
    property var decorator_: null
    property Component decorator

    onDecoratorChanged: {
        decorator_ = decorator.createObject(null)
    }
}
