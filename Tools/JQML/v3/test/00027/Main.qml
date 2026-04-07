import QtQuick

Item {
    id: root; width: 600; height: 400

    Rectangle {
        id: rect; width: 150; height: 80; color: "brown"

        Component.onCompleted: {
            console.log("--- Начальное положение ---")
            console.log("x:", x); console.log("y:", y);
            
            anchors.left = root.left; anchors.top = root.top;
            
            console.log("--- После привязки к левому верхнему углу ---")
            console.log("x:", x); console.log("y:", y);
            
            anchors.left = undefined; anchors.right = root.right;
            
            console.log("--- После привязки к правому краю ---")
            console.log("x:", x); console.log("width:", width);
            
            anchors.right = undefined; anchors.horizontalCenter = root.horizontalCenter;
            
            console.log("--- После центрирования по горизонтали ---")
            console.log("x:", x);
            
            anchors.top = undefined; anchors.bottom = root.bottom;
            
            console.log("--- После привязки к нижнему краю ---")
            console.log("y:", y);
            
            anchors.bottom = undefined; anchors.verticalCenter = root.verticalCenter;
            
            console.log("--- После центрирования по вертикали ---")
            console.log("y:", y);
            
            console.log("--- Финальные размеры ---")
            console.log("width:", width); console.log("height:", height);
            
            // Проверка центрирования
            var isCentered = Math.abs(x + width/2 - root.width/2) < 1 && Math.abs(y + height/2 - root.height/2) < 1;
            console.log("Элемент отцентрирован?", isCentered);
        }
    }
}