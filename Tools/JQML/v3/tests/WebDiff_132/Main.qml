import QtQuick

Item {
    id: root
    width: 240
    height: 160

    Image {
        id: image
        width: 100
        height: 100
        source: "data:image/svg+xml;base64,PHN2ZyB4bWxucz0iaHR0cDovL3d3dy53My5vcmcvMjAwMC9zdmciIHZpZXdCb3g9IjAgMCAyNCAyNCI+PHJlY3QgeD0iMiIgeT0iMiIgd2lkdGg9IjIwIiBoZWlnaHQ9IjIwIiBmaWxsPSIjYjY0NDQ0Ii8+PHBhdGggZD0iTTQgMTJoMTYiIHN0cm9rZT0iI2ZmZmZmZiIgc3Ryb2tlLXdpZHRoPSIyIi8+PC9zdmc+"
        fillMode: Image.PreserveAspectCrop
        horizontalAlignment: Image.AlignLeft
        verticalAlignment: Image.AlignTop

        onStatusChanged: {
            if (status === Image.Ready || status === Image.Error) {
                console.log("wd132", "image-base64-b", status, progress, fillMode, horizontalAlignment, verticalAlignment)
                Qt.quit()
            }
        }
    }

    Component.onCompleted: {
        if (image.status === Image.Ready || image.status === Image.Error) {
            console.log("wd132", "image-base64-b", image.status, image.progress, image.fillMode, image.horizontalAlignment, image.verticalAlignment)
            Qt.quit()
        }
    }
}
