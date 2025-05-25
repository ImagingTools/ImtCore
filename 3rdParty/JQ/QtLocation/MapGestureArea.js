const GroupProperty = require("../QtQml/GroupProperty")
const Real = require("../QtQml/Real")
const Bool = require("../QtQml/Bool")
const Signal = require("../QtQml/Signal")

class MapGestureArea extends GroupProperty {
    static NoGesture = 0
    static PinchGesture	= 1
    static PanGesture = 2
    static FlickGesture	= 3
    static RotationGesture = 4
    static TiltGesture = 5

    static meta = {
        acceptedGestures: {type: Real, value:MapGestureArea.NoGesture, signalName:'acceptedGesturesChanged'},
        enabled: {type: Bool, value:false, signalName:'enabledChanged'},
        flickDeceleration: {type: Real, value:2500, signalName:'flickDecelerationChanged'},
        maximumZoomLevelChange: {type: Real, value:4, signalName:'maximumZoomLevelChangeChanged'},
        panActive: {type: Bool, value:false, signalName:'panActiveChanged'},
        pinchActive: {type: Bool, value:false, signalName:'pinchActiveChanged'},
        preventStealing: {type: Bool, value:false, signalName:'preventStealingChanged'},
        rotationActive: {type: Bool, value:false, signalName:'rotationActiveChanged'},
        tiltActive: {type: Bool, value:false, signalName:'tiltActiveChanged'},
 
        acceptedGesturesChanged: {type:Signal, slotName:'onAcceptedGesturesChanged', args:[]},
        enabledChanged: {type:Signal, slotName:'onEnabledChanged', args:[]},
        flickDecelerationChanged: {type:Signal, slotName:'onFlickDecelerationChanged', args:[]},
        maximumZoomLevelChange: {type:Signal, slotName:'onMaximumZoomLevelChange', args:[]},
        panActiveChanged: {type:Signal, slotName:'onPanActiveChanged', args:[]},
        pinchActiveChanged: {type:Signal, slotName:'onPinchActiveChanged', args:[]},
        preventStealingChanged: {type:Signal, slotName:'onPreventStealingChanged', args:[]},
        rotationActiveChanged: {type:Signal, slotName:'onRotationActiveChanged', args:[]},
        tiltActiveChanged: {type:Signal, slotName:'onTiltActiveChanged', args:[]},
    }

}



module.exports = MapGestureArea