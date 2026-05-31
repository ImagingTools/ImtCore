import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype AbstractButton
    \inqmlmodule imtcontrols

    Clickable control base, API-compatible with Qt Quick Controls
    \c AbstractButton.

    AbstractButton adds press/click/toggle handling and the checkable state
    machine on top of \l Control. Concrete buttons (\l RoundButton,
    \l TabButton, \l Switch, ...) extend this type and supply their own
    \l background and \l contentItem.

    \list
      \li \l pressed reflects the current press state.
      \li \l clicked() is emitted on release inside the button.
      \li when \l checkable is true, a click toggles \l checked and emits
          \l toggled().
      \li \l autoExclusive groups buttons so that only one is checked at a
          time (used by \l TabBar and radio-style groups).
    \endlist

    \sa Control, RoundButton, TabButton, Switch
*/
Control {
    id: button
    objectName: "ImtControlsAbstractButton"

    // -------------------------------------------------------------------- //
    //                                Public API                             //
    // -------------------------------------------------------------------- //

    /*! Display text. */
    property string text: ""

    /*! Optional icon source. */
    property string iconSource: ""

    /*! Whether the button toggles \l checked when clicked. */
    property bool checkable: false

    /*! Checked state for \l checkable buttons. */
    property bool checked: false

    /*! When true, checking this button unchecks its siblings. */
    property bool autoExclusive: false

    /*! Whether the button stays pressed while held (auto-repeat off). */
    property bool autoRepeat: false

    /*! True while the button is pressed. */
    readonly property bool pressed: d.pressed

    /*! True while the mouse hovers the button. */
    hoverEnabled: true

    // -------------------------------------------------------------------- //
    //                                Signals                                //
    // -------------------------------------------------------------------- //

    signal clicked()
    signal released()
    signal canceled()
    signal toggled()
    signal doubleClicked()

    // -------------------------------------------------------------------- //
    //                              Behaviour                                //
    // -------------------------------------------------------------------- //

    /*! Programmatic activation: toggles if checkable, then emits clicked(). */
    function toggle() {
        if (!enabled)
            return;
        if (checkable) {
            if (autoExclusive && checked)
                return;       // exclusive buttons cannot be unchecked by self
            checked = !checked;
            toggled();
        }
        clicked();
    }

    QtObject {
        id: d
        property bool pressed: false
    }

    onCheckedChanged: {
        if (checked && checkable && autoExclusive)
            _uncheckSiblings();
    }

    // Uncheck other autoExclusive siblings sharing the same parent.
    function _uncheckSiblings() {
        if (!parent)
            return;
        var kids = parent.children;
        for (var i = 0; i < kids.length; ++i) {
            var k = kids[i];
            if (k !== button && k && k.autoExclusive === true
                    && k.checkable === true && k.checked === true)
                k.checked = false;
        }
    }

    MouseArea {
        id: mouse
        anchors.fill: parent
        enabled: button.enabled
        // Let the base Control hover handler (below) track `hovered`;
        // a hoverEnabled MouseArea on top would otherwise swallow hover.
        hoverEnabled: false
        onPressed: {
            d.pressed = true
        }
        onReleased: {
            if (d.pressed) {
                d.pressed = false;
                button.released();
            }
        }
        onCanceled: {
            if (d.pressed) {
                d.pressed = false;
                button.canceled();
            }
        }
        onClicked: {
            button.toggle()
        }
        onDoubleClicked: {
            button.doubleClicked()
        }
    }
}
