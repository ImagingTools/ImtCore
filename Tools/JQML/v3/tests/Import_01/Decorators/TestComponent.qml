import QtQuick
import "../Buttons"

Rectangle {
    color: 'red'

    MyButton {
        Component.onCompleted: {
            console.log("MyButton: onCompleted")
        }
    }

    Component.onCompleted: {
        console.log("TestComponent: onCompleted")
    }
}