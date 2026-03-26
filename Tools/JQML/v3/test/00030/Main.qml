import QtQuick

Item {
    id: root; width: 500; height: 400

    Rectangle {
        id: container
        anchors.centerIn: parent
        width: 300; height: 200

        Rectangle {
            id: innerRect
            anchors.fill: parent // Привязка к container, а не к root!

            Component.onCompleted: {
                console.log("innerRect.x (относительно container):", x)
                console.log("innerRect.y (относительно container):", y)
                console.log("innerRect.width:", width)
                console.log("innerRect.height:", height)
                console.log("container.x:", container.x)
                console.log("container.y:", container.y)
                // Проверим абсолютные координаты (относительно root)
                var absX = container.x + x;
                var absY = container.y + y;
                console.log("Абсолютные координаты innerRect:", absX, absY);
                console.log("container.width:", container.width);
                console.log("container.height:", container.height);
                console.log("Совпадение размеров?", Math.abs(width - container.width) < 1 && Math.abs(height - container.height) < 1)
            }
        }
    }
}