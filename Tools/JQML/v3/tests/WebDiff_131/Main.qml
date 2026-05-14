import QtQuick

Item {
    id: root
    width: 240
    height: 160

    Image {
        id: image
        width: 120
        height: 80
        source: "data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHdpZHRoPSIzMiIgaGVpZ2h0PSIxOCIgdmlld0JveD0iMCAwIDMyIDE4Ij48cmVjdCB4PSIwIiB5PSIwIiB3aWR0aD0iMzIiIGhlaWdodD0iMTgiIGZpbGw9IiM3Nzc3N2UiLz48Y2lyY2xlIGN4PSIxNiIgY3k9IjkiIHI9IjYiIGZpbGw9IiMwMDAwMDAiLz48L3N2Zz4="
        fillMode: Image.Stretch

        onStatusChanged: {
            if (status === Image.Ready || status === Image.Error) {
                console.log("wd131", "image-base64-a", status, progress, source, width, height)
                Qt.quit()
            }
        }
    }

    Component.onCompleted: {
        if (image.status === Image.Ready || image.status === Image.Error) {
            console.log("wd131", "image-base64-a", image.status, image.progress, image.source, image.width, image.height)
            Qt.quit()
        }
    }
}
