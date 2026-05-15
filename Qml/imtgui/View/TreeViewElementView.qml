import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

ElementView {
    id: root

    property var treeView: null

    bottomComp: treeViewComp

    Component {
        id: treeViewComp

        FocusScope {
            width: root.contentWidth
            height: filterInput.height + Style.marginM + checkableTree.height

            SearchTextInput {
                id: filterInput

                width: parent.width
                placeHolderText: qsTr("Filter...")

                onSearchChanged: {
                    checkableTree.filterText = filterInput.text
                }
            }

            BasicTreeView {
                id: checkableTree

                anchors.top: filterInput.bottom
                anchors.topMargin: Style.marginM
                width: parent.width

                showHeader: false

                Component.onCompleted: root.treeView = checkableTree
            }
        }
    }
}
