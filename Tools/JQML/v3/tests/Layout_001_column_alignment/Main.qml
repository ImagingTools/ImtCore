import QtQuick
import QtQuick.Layouts

Item {
    ColumnLayout {
        id: columnLayout
        spacing: 2

        height: 420

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
            Layout.alignment: Qt.AlignRight
            color: "green"
            Layout.preferredWidth: 40
            Layout.preferredHeight: 70
        }

        onWidthChanged: {
            console.log('ColumnLayout.onWidthChanged', width)
        }

        onHeightChanged: {
            console.log('ColumnLayout.onHeightChanged', height)
        }
    }

    Component.onCompleted: {
        console.log('columnLayout.width', columnLayout.width)
        console.log('columnLayout.height', columnLayout.height)
        console.log('rect1.x', columnLayout.children[0].x)
        console.log('rect1.y', columnLayout.children[0].y)
        console.log('rect1.width', columnLayout.children[0].width)
        console.log('rect1.height', columnLayout.children[0].height)
        console.log('rect2.x', columnLayout.children[1].x)
        console.log('rect2.y', columnLayout.children[1].y)
        console.log('rect2.width', columnLayout.children[1].width)
        console.log('rect2.height', columnLayout.children[1].height)
        Qt.quit()
    }
}
