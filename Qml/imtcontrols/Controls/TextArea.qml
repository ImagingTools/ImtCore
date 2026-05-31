import QtQuick 2.12
import Acf 1.0
import imtcontrols 2.0

/*!
    \qmltype TextArea
    \inqmlmodule imtcontrols

    Multi-line text input, API-compatible with Qt Quick Controls
    \c TextArea.

    TextArea is a themed multi-line editor built on \l TextEdit. It shows
    \l placeholderText while empty, wraps text by default and draws a
    focus-aware border. Wrap it in a \l ScrollView (or \l Flickable) for
    scrolling of large documents.

    \qml
    TextArea {
        placeholderText: qsTr("Description...")
        wrapMode: TextEdit.Wrap
    }
    \endqml

    \sa TextField, ScrollView
*/
Control {
    id: control
    objectName: "ImtControlsTextArea"

    /*! The edited text. */
    property alias text: edit.text
    /*! Hint shown while the area is empty. */
    property string placeholderText: ""
    /*! Whether the text can be edited. */
    property alias readOnly: edit.readOnly
    /*! Text wrapping mode. */
    property alias wrapMode: edit.wrapMode
    /*! Underlying TextEdit, for advanced access. */
    readonly property alias textEdit: edit

    /*! Emitted when editing finishes (focus lost). */
    signal editingFinished()

    padding: Style.spacingS
    leftPadding: Style.spacingM
    rightPadding: Style.spacingM

    implicitWidth:  Style.buttonWidthL
    implicitHeight: Math.max(Style.controlHeightL * 2, edit.implicitHeight + topPadding + bottomPadding)

    background: Rectangle {
        radius: Style.textFieldRadius
        color: Style.backgroundColor
        border.width: Style.buttonBorderWidth
        border.color: edit.activeFocus ? Style.imaginToolsAccentColor : Style.borderColor
        Behavior on border.color { ColorAnimation { duration: 120 } }
    }

    contentItem: Item {
        implicitWidth:  Style.buttonWidthL
        implicitHeight: edit.implicitHeight

        TextEdit {
            id: edit
            anchors.fill: parent
            wrapMode: TextEdit.Wrap
            clip: true
            selectByMouse: true
            enabled: control.enabled
            color: control.enabled ? Style.textColor : Style.inactiveTextColor
            selectionColor: Style.imaginToolsAccentColor
            selectedTextColor: "#ffffff"
            font.family: Style.fontFamily
            font.pixelSize: Style.fontSizeM
            onActiveFocusChanged: {
                if (!activeFocus)
                    control.editingFinished();
            }

            Text {
                anchors.fill: parent
                visible: edit.text === "" && !edit.activeFocus
                text: control.placeholderText
                color: Style.placeHolderTextColor
                font.family: Style.fontFamily
                font.pixelSize: Style.fontSizeM
                wrapMode: Text.Wrap
            }
        }
    }
}
