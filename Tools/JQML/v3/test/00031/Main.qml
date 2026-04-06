import QtQuick

Item {
    width: 300; height: 200

    Rectangle {
        id: rectA
        width: 150; height: 80; color: "green"
        anchors.centerIn: parent

        Component.onCompleted: {
            console.log("rectA.x (видимый):", x)
            console.log("rectA.y (видимый):", y)
            
            visible = false;
        }

        Rectangle {
            id: rectB
            width: 100; height: 60; color:"red"
            anchors.centerIn: parent
            
            Component.onCompleted:{
                console.log("rectB.x (после скрытия A):", x);
                console.log("rectB.y (после скрытия A):", y);
                console.log("rectB.visible:", visible);
            }
        }
    }
 }