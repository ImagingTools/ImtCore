import QtQuick

Item {
    width: 300
    height: 100

    Rectangle { id: plain; width: 10; height: 10 }
    Rectangle { id: colorOnly; width: 10; height: 10; border.color: "red" }
    Rectangle { id: widthOnly; width: 10; height: 10; border.width: 3 }
    Rectangle { id: zero; width: 10; height: 10; border.color: "red"; border.width: 0 }
    Rectangle { id: computed; width: 10; height: 10; border.color: Qt.rgba(1, 0, 0, 1) }

    Component.onCompleted: {
        console.log("plain", plain.border.width, Qt.colorEqual(plain.border.color, "black"))
        console.log("colorOnly", colorOnly.border.width, Qt.colorEqual(colorOnly.border.color, "red"))
        console.log("widthOnly", widthOnly.border.width, Qt.colorEqual(widthOnly.border.color, "black"))
        console.log("zero", zero.border.width)
        console.log("computed", computed.border.width)
        colorOnly.border.color = "blue"
        plain.border.color = "green"
        console.log("script", plain.border.width, colorOnly.border.width)
        Qt.quit()
    }
}
