import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype GroupBox
    \inqmlmodule imtcontrols

    Titled frame container, API-compatible with Qt Quick Controls
    \c GroupBox.

    GroupBox draws a labelled border around a group of controls. The \l title
    is rendered at the top-left, breaking the frame border, and the content
    (the default children) is inset below it.

    \qml
    GroupBox {
        title: qsTr("Connection")
        Column {
            spacing: 8
            RadioButton { text: "Automatic" }
            RadioButton { text: "Manual" }
        }
    }
    \endqml

    \sa Frame, Pane
*/
Item {
    id: groupBox
    objectName: "ImtControlsGroupBox"

    /*! Title text rendered above the frame. */
    property string title: ""

    /*! Padding between the frame and the content. */
    property real padding: Style.spacingM

    /*! User content placed inside the frame. */
    default property alias contentData: contentHolder.data

    readonly property real _titleH: title !== "" ? titleLabel.implicitHeight : 0

    implicitWidth:  Math.max(contentHolder.childrenRect.width  + 2 * padding,
                             titleLabel.implicitWidth + 2 * Style.spacingM)
    implicitHeight: contentHolder.childrenRect.height + 2 * padding + _titleH
    width:  implicitWidth
    height: implicitHeight

    // Frame border, starting at the title's vertical centre.
    Rectangle {
        id: frame
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        y: groupBox._titleH / 2
        height: groupBox.height - y
        color: "transparent"
        radius: Style.radiusM
        border.width: Style.buttonBorderWidth
        border.color: Style.borderColor
    }

    // Title chip painted over the border so it visually breaks the line.
    Text {
        id: titleLabel
        x: Style.spacingM
        y: 0
        visible: groupBox.title !== ""
        text: groupBox.title
        color: Style.textColor
        font.family: Style.fontFamily
        font.pixelSize: Style.fontSizeM
        leftPadding: Style.spacingXS
        rightPadding: Style.spacingXS

        Rectangle {
            anchors.fill: parent
            z: -1
            color: Style.backgroundColor2
        }
    }

    Item {
        id: contentHolder
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.topMargin:    groupBox._titleH + groupBox.padding
        anchors.leftMargin:   groupBox.padding
        anchors.rightMargin:  groupBox.padding
        anchors.bottomMargin: groupBox.padding
    }
}
