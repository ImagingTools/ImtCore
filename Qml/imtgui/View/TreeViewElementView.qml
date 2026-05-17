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

        Column {
            width: root.contentWidth
            clip: true

            SearchTextInput {
                id: filterInput

                width: parent.width
                placeHolderText: qsTr("Filter...")

                onSearchChanged: {
                    checkableTree.filterText = filterInput.text
                }
            }
            
            Row {
                id: toolbarRow
                width: parent.width
                height: 28
                spacing: Style.spacingM
                leftPadding: Style.spacingS

                Text {
                    text: qsTr("Expand All")
                    color: Style.linkColor
                    font.underline: toolbarExpandAllMa.containsMouse
                    verticalAlignment: Text.AlignVCenter
                    height: parent.height
    
                    MouseArea {
                        id: toolbarExpandAllMa
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: checkableTree.expandAll()
                    }
                }
    
                Text {
                    text: qsTr("Collapse All")
                    color: Style.linkColor
                    font.underline: toolbarCollapseAllMa.containsMouse
                    verticalAlignment: Text.AlignVCenter
                    height: parent.height
    
                    MouseArea {
                        id: toolbarCollapseAllMa
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: checkableTree.collapseAll()
                    }
                }
    
                Text {
                    text: qsTr("Check All")
                    color: Style.linkColor
                    font.underline: toolbarCheckAllMa.containsMouse
                    visible: checkableTree.tristate
                    verticalAlignment: Text.AlignVCenter
                    height: parent.height
    
                    MouseArea {
                        id: toolbarCheckAllMa
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: checkableTree.checkAll()
                    }
                }
    
                Text {
                    text: qsTr("Uncheck All")
                    color: Style.linkColor
                    font.underline: toolbarUncheckAllMa.containsMouse
                    visible: checkableTree.tristate
                    verticalAlignment: Text.AlignVCenter
                    height: parent.height
    
                    MouseArea {
                        id: toolbarUncheckAllMa
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: checkableTree.uncheckAll()
                    }
                }
            }

            Item {
                id: treeContainer
                width: parent.width
                height: Math.min(checkableTree.height, root.maxTreeHeight)

                Flickable {
                    id: flickable
                    anchors.fill: parent

                    contentWidth: width
                    contentHeight: checkableTree.height
                    clip: true

                    BasicTreeView {
                        id: checkableTree
                        width: flickable.width - (scrollbar.visible ? scrollbar.width : 0)

                        showHeader: false
                        tristate: true
                        Component.onCompleted: {
                            root.treeView = checkableTree
                        }
                    }
                }

                CustomScrollbar {
                    id: scrollbar
                    z: 100
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    secondSize: Style.marginM
                    targetItem: flickable
                    radius: Style.radiusS
                }
            }
        }
    }
}
