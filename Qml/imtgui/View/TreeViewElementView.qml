import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

ElementView {
    id: root

    property var treeView: null
    property int maxTreeHeight: 500

    bottomComp: treeViewComp

    Component {
        id: treeViewComp

        FocusScope {
            width: root.contentWidth
            height: filterInput.height + Style.marginM + Math.min(checkableTree.implicitHeight, root.maxTreeHeight)

            SearchTextInput {
                id: filterInput

                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                placeHolderText: qsTr("Filter...")

                onSearchChanged: {
                    checkableTree.filterText = filterInput.text
                }
            }

            BasicTreeView {
                id: checkableTree

                anchors.top: filterInput.bottom
                anchors.topMargin: Style.marginM
                anchors.left: parent.left
                anchors.right: scrollIndicator.visible ? scrollIndicator.left : parent.right
                height: Math.min(implicitHeight, root.maxTreeHeight)

                showHeader: false
                showToolbar: true
                tristate: true

                Component.onCompleted: root.treeView = checkableTree
            }

            Rectangle {
                id: scrollIndicator

                anchors.right: parent.right
                anchors.top: checkableTree.top
                anchors.bottom: checkableTree.bottom

                width: 6
                visible: checkableTree.listView && checkableTree.listView.contentHeight > checkableTree.listView.height
                color: "transparent"

                Rectangle {
                    id: scrollThumb

                    property real viewRatio: checkableTree.listView ? Math.min(1.0, checkableTree.listView.height / Math.max(1, checkableTree.listView.contentHeight)) : 1.0
                    property real viewPos: checkableTree.listView ? checkableTree.listView.contentY / Math.max(1, checkableTree.listView.contentHeight - checkableTree.listView.height) : 0

                    width: parent.width
                    height: Math.max(20, parent.height * viewRatio)
                    y: Math.min(parent.height - height, Math.max(0, viewPos * (parent.height - height)))
                    radius: 3
                    color: scrollThumbMa.pressed ? Style.borderColor : scrollThumbMa.containsMouse ? Style.borderColor2 : Qt.rgba(0.6, 0.6, 0.6, 0.4)

                    MouseArea {
                        id: scrollThumbMa
                        anchors.fill: parent
                        hoverEnabled: true
                    }
                }
            }
        }
    }
}
