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
                console.log('Red Rectangle.onXChanged', x)
            }

            onYChanged: {
                console.log('Red Rectangle.onYChanged', y)
            }

            onWidthChanged: {
                console.log('Red Rectangle.onWidthChanged', width)
            }

            onHeightChanged: {
                console.log('Red Rectangle.onHeightChanged', height)
            }
        }

        Rectangle {
            Layout.alignment: Qt.AlignRight
            color: "green"
            Layout.preferredWidth: 40
            Layout.preferredHeight: 70
        }

        Rectangle {
            Layout.alignment: Qt.AlignBottom
            Layout.fillHeight: true
            color: "blue"
            Layout.preferredWidth: 70
            Layout.preferredHeight: 40

            onXChanged: {
                console.log('Blue Rectangle.onXChanged', x)
            }

            onYChanged: {
                console.log('Blue Rectangle.onYChanged', y)
            }

            onWidthChanged: {
                console.log('Blue Rectangle.onWidthChanged', width)
            }

            onHeightChanged: {
                console.log('Blue Rectangle.onHeightChanged', height)
            }
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
        console.log('rect3.x', columnLayout.children[2].x)
        console.log('rect3.y', columnLayout.children[2].y)
        console.log('rect3.width', columnLayout.children[2].width)
        console.log('rect3.height', columnLayout.children[2].height)
        Qt.quit()
    }
}
