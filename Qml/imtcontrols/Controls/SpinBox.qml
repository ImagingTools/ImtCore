import QtQuick 2.12
import Acf 1.0
import imtcontrols 2.0

/*!
    \qmltype SpinBox
    \inqmlmodule imtcontrols

    Numeric spin box, API-compatible with Qt Quick Controls \c SpinBox.

    SpinBox lets the user choose an integer \l value from a range
    [\l from, \l to] by pressing the up/down buttons or editing the text
    field. \l stepSize controls the increment applied by the buttons.

    \qml
    SpinBox {
        from: 0; to: 100; value: 50; stepSize: 5
        onValueModified: console.log("value:", value)
    }
    \endqml

    \sa Slider
*/
Control {
    id: control
    objectName: "ImtControlsSpinBox"

    /*! Lower bound of the range. */
    property int from: 0
    /*! Upper bound of the range. */
    property int to: 99
    /*! Current value, clamped to [\l from, \l to]. */
    property int value: 0
    /*! Increment applied by the up/down buttons. */
    property int stepSize: 1
    /*! Whether the text field can be edited directly. */
    property bool editable: true

    /*! Emitted whenever the user changes the value. */
    signal valueModified()

    implicitWidth:  Style.buttonWidthL
    implicitHeight: Style.controlHeightL

    function increase() { _apply(value + stepSize); }
    function decrease() { _apply(value - stepSize); }

    function _apply(v) {
        var clamped = Math.max(from, Math.min(to, v));
        if (clamped !== value) {
            value = clamped;
            valueModified();
        }
    }

    background: Rectangle {
        radius: Style.radiusS
        color: Style.backgroundColor
        border.width: Style.buttonBorderWidth
        border.color: Style.borderColor
    }

    contentItem: Item {
        implicitWidth:  Style.buttonWidthL
        implicitHeight: Style.controlHeightL

        ToolButton {
            id: downButton
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            width: height
            height: parent.height
            text: "-"
            enabled: control.enabled && control.value > control.from
            onClicked: control.decrease()
        }

        TextInput {
            id: editor
            anchors.left: downButton.right
            anchors.right: upButton.left
            anchors.verticalCenter: parent.verticalCenter
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            readOnly: !control.editable
            enabled: control.enabled
            selectByMouse: true
            text: "" + control.value
            color: control.enabled ? Style.textColor : Style.inactiveTextColor
            font.family: Style.fontFamily
            font.pixelSize: Style.fontSizeM
            validator: IntValidator { bottom: control.from; top: control.to }
            onEditingFinished: control._apply(parseInt(text, 10))
        }

        ToolButton {
            id: upButton
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            width: height
            height: parent.height
            text: "+"
            enabled: control.enabled && control.value < control.to
            onClicked: control.increase()
        }
    }
}
