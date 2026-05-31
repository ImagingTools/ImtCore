import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype TabBar
    \inqmlmodule imtcontrols

    Row of tab buttons, API-compatible with Qt Quick Controls \c TabBar.

    TabBar lays out its \l TabButton children horizontally and keeps exactly
    one of them checked. \l currentIndex tracks the selected tab and is the
    natural binding source for a \c StackLayout / \l SwipeView showing the
    matching page.

    \qml
    TabBar {
        id: bar
        TabButton { text: qsTr("General") }
        TabButton { text: qsTr("Network") }
        TabButton { text: qsTr("About") }
    }
    StackLayout { currentIndex: bar.currentIndex; ... }
    \endqml

    \sa TabButton
*/
Item {
    id: tabBar
    objectName: "ImtControlsTabBar"

    /*! Tab buttons declared inside the bar. */
    default property alias contentData: tabRow.data

    /*! Index of the currently selected tab (-1 when empty). */
    property int currentIndex: 0

    /*! Number of tab buttons in the bar. */
    readonly property int count: tabRow.children.length

    /*! Spacing between tabs. */
    property real spacing: 0

    /*! Emitted whenever \l currentIndex changes by user interaction. */
    signal tabSelected(int index)

    implicitWidth:  tabRow.implicitWidth
    implicitHeight: tabRow.implicitHeight > 0 ? tabRow.implicitHeight : Style.controlHeightL
    width:  implicitWidth
    height: implicitHeight

    Rectangle {
        anchors.fill: parent
        color: Style.backgroundColor2
    }

    Row {
        id: tabRow
        anchors.fill: parent
        spacing: tabBar.spacing
    }

    // Items already wired up (identity tracking; QML items cannot hold
    // arbitrary marker properties, so we keep the set here).
    property var _wiredItems: []

    onCurrentIndexChanged: _syncChecked()
    Component.onCompleted: _wireUp()

    Connections {
        target: tabRow
        function onChildrenChanged() { tabBar._wireUp(); }
    }

    // Bind each tab's checked state to currentIndex and route clicks back.
    function _wireUp() {
        var kids = tabRow.children;
        for (var i = 0; i < kids.length; ++i) {
            var t = kids[i];
            if (!t || t.checkable === undefined)
                continue;
            if (_wiredItems.indexOf(t) !== -1)
                continue;
            _wiredItems.push(t);
            t.autoExclusive = true;
            // Capture index lazily via indexOf so reordering stays correct.
            t.clicked.connect((function(btn) {
                return function() {
                    var idx = tabBar._indexOf(btn);
                    if (idx >= 0) {
                        tabBar.currentIndex = idx;
                        tabBar.tabSelected(idx);
                    }
                };
            })(t));
        }
        _syncChecked();
    }

    function _indexOf(item) {
        var kids = tabRow.children;
        for (var i = 0; i < kids.length; ++i)
            if (kids[i] === item)
                return i;
        return -1;
    }

    function _syncChecked() {
        var kids = tabRow.children;
        for (var i = 0; i < kids.length; ++i) {
            var t = kids[i];
            if (t && t.checkable !== undefined)
                t.checked = (i === currentIndex);
        }
    }
}
