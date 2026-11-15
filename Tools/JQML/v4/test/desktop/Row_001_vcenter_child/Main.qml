import QtQuick

Item {
    width: 200
    height: 100

    Row {
        id: row
        width: 200
        height: 100
        spacing: 10

        Rectangle {
            id: top
            width: 40
            height: 20
            color: "red"
        }

        Rectangle {
            id: mid
            anchors.verticalCenter: parent.verticalCenter
            width: 40
            height: 20
            color: "blue"
        }

        Rectangle {
            id: bot
            anchors.bottom: parent.bottom
            width: 40
            height: 20
            color: "green"
        }
    }

    Component.onCompleted: {
        console.log("top", top.y)
        console.log("mid", mid.y)
        console.log("bot", bot.y)
        Qt.quit()
    }
}
