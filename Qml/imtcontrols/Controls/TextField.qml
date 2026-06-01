import QtQuick 2.12
import Acf 1.0
import imtcontrols 2.0

/*!
    \qmltype TextField
    \inqmlmodule imtcontrols

    Single-line text input, API-compatible with Qt Quick Controls
    \c TextField.

    TextField is a themed single-line editor built on \l TextInput. It shows
    \l placeholderText while empty and draws a focus-aware border. All the
    common \c TextInput properties (\c text, \c readOnly, \c echoMode,
    \c validator, ...) are exposed directly.

    \qml
    TextField {
        placeholderText: qsTr("Search...")
        onAccepted: model.filter(text)
    }
    \endqml

    \sa TextArea
*/
Control {
    id: control
    objectName: "ImtControlsTextField"

    /*! The edited text. */
    property alias text: input.text
    /*! Hint shown while the field is empty. */
    property string placeholderText: ""
    /*! Whether the text can be edited. */
    property alias readOnly: input.readOnly
    /*! Echo mode (e.g. \c TextInput.Password). */
    property alias echoMode: input.echoMode
    /*! Optional input validator. */
    property alias validator: input.validator
    /*! Optional input mask. */
    property alias inputMask: input.inputMask
    /*! Maximum number of characters. */
    property alias maximumLength: input.maximumLength
    /*! True while the field holds keyboard focus. */
    readonly property alias activeFocusInput: input.activeFocus

    /*! Emitted when Return/Enter is pressed. */
    signal accepted()
    /*! Emitted when editing finishes (focus lost or Enter). */
    signal editingFinished()

    padding: Style.spacingS
    leftPadding: Style.spacingM
    rightPadding: Style.spacingM

    implicitWidth:  Style.buttonWidthL
    implicitHeight: Style.controlHeightL

    background: Rectangle {
        radius: Style.textFieldRadius
        color: Style.backgroundColor
        border.width: Style.buttonBorderWidth
        border.color: input.activeFocus ? Style.imaginToolsAccentColor : Style.borderColor
    }

    contentItem: Item {
        implicitWidth:  Style.buttonWidthL
        implicitHeight: input.implicitHeight

        TextInput {
            id: input
            anchors.fill: parent
            verticalAlignment: Text.AlignVCenter
            clip: true
            selectByMouse: true
            enabled: control.enabled
            color: control.enabled ? Style.textColor : Style.inactiveTextColor
            selectionColor: Style.imaginToolsAccentColor
            selectedTextColor: "#ffffff"
            font.family: Style.fontFamily
            font.pixelSize: Style.fontSizeM
            onAccepted: {
                control.accepted()
            }
            onEditingFinished: {
                control.editingFinished()
            }

            Text {
                anchors.fill: parent
                verticalAlignment: Text.AlignVCenter
                visible: input.text === "" && !input.activeFocus
                text: control.placeholderText
                color: Style.placeHolderTextColor
                font.family: Style.fontFamily
                font.pixelSize: Style.fontSizeM
                elide: Text.ElideRight
            }
        }
    }
}
