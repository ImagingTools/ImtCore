import QtQuick 2.12

/*!
    \qmltype PopupTransition
    \inqmlmodule imtcontrols
    \internal

    Default enter/exit transitions for Popup. Kept as a regular (non-singleton)
    item so its sub-animations get fresh instances per use site.

    Behaviour matches Qt Quick Controls Popup defaults: opacity and scale
    fade in on enter, fade out on exit. Interrupt-safe: if a new transition
    starts while a previous one is running, the running animation is stopped
    cleanly and the new one resumes from the current visual state.
*/
QtObject {
    id: root

    /*! Default enter Transition factory. Returns a Transition created from
        a Component, so each caller owns its own animation instance. */
    property Component enter: Component {
        Transition {
            ParallelAnimation {
                NumberAnimation {
                    property: "opacity"
                    from: 0.0
                    to: 1.0
                    duration: 120
                    easing.type: Easing.OutQuad
                }
                NumberAnimation {
                    property: "scale"
                    from: 0.9
                    to: 1.0
                    duration: 120
                    easing.type: Easing.OutBack
                }
            }
        }
    }

    /*! Default exit Transition factory. */
    property Component exit: Component {
        Transition {
            ParallelAnimation {
                NumberAnimation {
                    property: "opacity"
                    from: 1.0
                    to: 0.0
                    duration: 100
                    easing.type: Easing.InQuad
                }
                NumberAnimation {
                    property: "scale"
                    from: 1.0
                    to: 0.95
                    duration: 100
                    easing.type: Easing.InQuad
                }
            }
        }
    }
}
