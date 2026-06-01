import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import "../Popup"

/*!
    \qmltype ToolTip
    \inqmlmodule imtcontrols

    Short explanatory popup, API-compatible with Qt Quick Controls
    \c ToolTip.

    ToolTip is a small, non-modal \l Popup that shows a single line of
    \l text after a \l delay and hides automatically after \l timeout
    milliseconds (or when \l hide() is called). It is meant to explain the
    control the pointer is hovering.

    \qml
    Item {
        ToolTip { id: tip; text: qsTr("Save the document") }
        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            onEntered: tip.show(tip.text)
            onExited:  tip.hide()
        }
    }
    \endqml

    \sa Popup
*/
Popup {
    id: toolTip
    objectName: "ImtControlsPopup"

    /*! Tooltip text. */
    property string text: ""

    /*! Delay before the tooltip appears, in milliseconds. */
    property int delay: 500

    /*! Auto-hide timeout, in milliseconds. \c 0 disables auto-hide. */
    property int timeout: 5000

    modal: false
    focusOnOpen: false
    padding: Style.spacingS

    /*! Schedule the tooltip to show \a toolTipText after \l delay. */
    function show(toolTipText) {
        if (toolTipText !== undefined)
            text = toolTipText;
        hideTimer.stop();
        showTimer.restart();
    }

    /*! Hide the tooltip immediately and cancel any pending show. */
    function hide() {
        showTimer.stop();
        hideTimer.stop();
        close();
    }

    Timer {
        id: showTimer
        interval: toolTip.delay
        onTriggered: {
            toolTip.open();
            if (toolTip.timeout > 0)
                hideTimer.restart();
        }
    }

    Timer {
        id: hideTimer
        interval: toolTip.timeout
        onTriggered: {
            toolTip.close()
        }
    }

    background: Rectangle {
        color: Style.textColor
        radius: Style.radiusS
        opacity: 0.95
    }

    contentItem: Text {
        text: toolTip.text
        color: "#ffffff"
        font.family: Style.fontFamily
        font.pixelSize: Style.fontSizeS
        wrapMode: Text.WordWrap
    }
}
