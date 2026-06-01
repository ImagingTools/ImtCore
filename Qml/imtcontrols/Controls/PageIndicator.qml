import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype PageIndicator
    \inqmlmodule imtcontrols

    Row of page dots, API-compatible with Qt Quick Controls
    \c PageIndicator.

    PageIndicator visualises the number of pages (\l count) and which one is
    current (\l currentIndex). It is typically bound to a \l SwipeView. When
    \l interactive is true, clicking a dot changes the current page.

    \qml
    SwipeView { id: view; ... }
    PageIndicator {
        count: view.count
        currentIndex: view.currentIndex
        interactive: true
        onCurrentIndexChanged: view.currentIndex = currentIndex
    }
    \endqml

    \sa SwipeView, TabBar
*/
Item {
    id: control
    objectName: "ImtControlsPageIndicator"

    /*! Number of pages/dots. */
    property int count: 0
    /*! Index of the highlighted dot. */
    property int currentIndex: 0
    /*! Whether clicking a dot changes \l currentIndex. */
    property bool interactive: false
    /*! Spacing between dots. */
    property real spacing: Style.spacingS
    /*! Diameter of each dot. */
    property real dotSize: Style.spacingS

    implicitWidth:  row.implicitWidth
    implicitHeight: Math.max(dotSize, Style.spacingM)
    width:  implicitWidth
    height: implicitHeight

    Row {
        id: row
        anchors.centerIn: parent
        spacing: control.spacing

        Repeater {
            model: control.count
            delegate: Rectangle {
                width:  control.dotSize
                height: control.dotSize
                radius: width / 2
                color: index === control.currentIndex
                       ? Style.imaginToolsAccentColor : Style.borderColor2
                opacity: index === control.currentIndex ? 1.0 : 0.6

                MouseArea {
                    anchors.fill: parent
                    enabled: control.interactive
                    onClicked: {
                        control.currentIndex = index
                    }
                }
            }
        }
    }
}
