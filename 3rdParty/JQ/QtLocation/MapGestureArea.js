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
        acceptedGestures: {type: Real, value:MapGestureArea.NoGesture, },
        enabled: {type: Bool, value:false, },
        flickDeceleration: {type: Real, value:2500, },
        maximumZoomLevelChange: {type: Real, value:4, },
        panActive: {type: Bool, value:false, },
        pinchActive: {type: Bool, value:false, },
        preventStealing: {type: Bool, value:false, },
        rotationActive: {type: Bool, value:false, },
        tiltActive: {type: Bool, value:false, },
 
        acceptedGesturesChanged: {type:Signal, args:[]},
        enabledChanged: {type:Signal, args:[]},
        flickDecelerationChanged: {type:Signal, args:[]},
        maximumZoomLevelChange: {type:Signal, args:[]},
        panActiveChanged: {type:Signal, args:[]},
        pinchActiveChanged: {type:Signal, args:[]},
        preventStealingChanged: {type:Signal, args:[]},
        rotationActiveChanged: {type:Signal, args:[]},
        tiltActiveChanged: {type:Signal, args:[]},
    }

}



module.exports = MapGestureArea