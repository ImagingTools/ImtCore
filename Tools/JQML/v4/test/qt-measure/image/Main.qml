import QtQuick

// Image.sourceSize of a picture that is loaded (pic.png is 40 x 20): what Qt answers with and without a requested size.
Item {
    width: 200
    height: 200
    Image { id: plain; source: "pic.png" }
    Image { id: sized; source: "pic.png"; sourceSize.width: 20; sourceSize.height: 10 }
    Image { id: onlyWidth; source: "pic.png"; sourceSize.width: 10 }
    Image { id: big; source: "pic.png"; sourceSize: Qt.size(80, 60) }
    Image { id: fixed; source: "pic.png"; width: 100; height: 100; sourceSize.width: 20; sourceSize.height: 10 }
    Timer {
        interval: 300
        running: true
        onTriggered: {
            var list = [["plain", plain], ["sized", sized], ["onlyWidth", onlyWidth], ["big", big], ["fixed", fixed]]
            for (var i = 0; i < list.length; i++) {
                var im = list[i][1]
                console.log(list[i][0], "status", im.status, "sourceSize", im.sourceSize.width, im.sourceSize.height, "implicit", im.implicitWidth, im.implicitHeight, "size", im.width, im.height, "painted", im.paintedWidth, im.paintedHeight)
            }
            Qt.quit()
        }
    }
}
