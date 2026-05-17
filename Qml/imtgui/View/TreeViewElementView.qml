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

            CustomScrollbar {
                id: scrollbar
                z: parent.z + 1
                anchors.right: parent.right
                anchors.top: checkableTree.top
                anchors.bottom: parent.bottom
                secondSize: Style.marginM
                targetItem: flickable
                radius: Style.radiusS
            }

            Flickable {
                id: flickable
                anchors.fill: parent
                contentWidth: checkableTree.width
                contentHeight: checkableTree.implicitHeight + 2 * Style.marginXL
                boundsBehavior: Flickable.StopAtBounds

                BasicTreeView {
                    id: checkableTree
    
                    anchors.top: filterInput.bottom
                    anchors.topMargin: Style.marginM
                    anchors.left: parent.left
                    anchors.right: scrollbar.visible ? scrollbar.left : parent.right
                    height: Math.min(implicitHeight, root.maxTreeHeight)
    
                    showHeader: false
                    showToolbar: true
                    tristate: true
                    Component.onCompleted: {
                        root.treeView = checkableTree
                    }
                }
            }
        }
    }
}
