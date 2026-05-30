import QtQuick
import QtQuick.Layouts

Item {
    RowLayout {
        id: rowLayout
        spacing: 2

        width: 420

        Rectangle {
            Layout.alignment: Qt.AlignCenter
            color: "red"
            Layout.preferredWidth: 40
            Layout.preferredHeight: 40

            onXChanged: {
                console.log('Rectangle.onXChanged', x)
            }

            onYChanged: {
                console.log('Rectangle.onYChanged', y)
            }

            onWidthChanged: {
                console.log('Rectangle.onWidthChanged', width)
            }

            onHeightChanged: {
                console.log('Rectangle.onHeightChanged', height)
            }
        }

        Rectangle {
            Layout.alignment: Qt.AlignBottom
            color: "green"
            Layout.preferredWidth: 70
            Layout.preferredHeight: 40
        }

        onWidthChanged: {
            console.log('RowLayout.onWidthChanged', width)
        }

        onHeightChanged: {
            console.log('RowLayout.onHeightChanged', height)
        }
    }

    Component.onCompleted: {
        console.log('rowLayout.width', rowLayout.width)
        console.log('rowLayout.height', rowLayout.height)
        console.log('rect1.x', rowLayout.children[0].x)
        console.log('rect1.y', rowLayout.children[0].y)
        console.log('rect1.width', rowLayout.children[0].width)
        console.log('rect1.height', rowLayout.children[0].height)
        console.log('rect2.x', rowLayout.children[1].x)
        console.log('rect2.y', rowLayout.children[1].y)
        console.log('rect2.width', rowLayout.children[1].width)
        console.log('rect2.height', rowLayout.children[1].height)
        Qt.quit()
    }
}
