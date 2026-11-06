import QtQuick

Item {
    property int foo: 1
    property Component decorator
    property var made: null
    property int fooChanges: 0

    onFooChanged: fooChanges++
    onDecoratorChanged: made = decorator.createObject(null)
}
