import QtQuick

Item {
    id: root
    width: 220
    height: 160

    Image {
        id: image
        width: 120
        height: 80
        source: "icon-explicit.svg"
        fillMode: Image.Stretch

        onStatusChanged: {
            if (status === Image.Ready || status === Image.Error) {
                console.log("wd129", "image-explicit", status, progress, source, width, height)
                Qt.quit()
            }
        }
    }

    Component.onCompleted: {
        if (image.status === Image.Ready || image.status === Image.Error) {
            console.log("wd129", "image-explicit", image.status, image.progress, image.source, image.width, image.height)
            Qt.quit()
        }
    }
}
