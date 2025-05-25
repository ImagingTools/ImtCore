const GroupProperty = require("../QtQml/GroupProperty")
const Real = require("../QtQml/Real")
const Signal = require("../QtQml/Signal")

class GeoCoordinate extends GroupProperty {
    static meta = {
        latitude: {type:Real, value:0, signalName:'latitudeChanged'},
        longitude: {type:Real, value:0, signalName:'longitudeChanged'},
        altitude: {type:Real, value:0, signalName:'altitudeChanged'},
        
        latitudeChanged: {type:Signal, slotName:'onLatitudeChanged', args:[]},
        longitudeChanged: {type:Signal, slotName:'onLongitudeChanged', args:[]},
        altitudeChanged: {type:Signal, slotName:'onAltitudeChanged', args:[]},
    }

    
}



module.exports = GeoCoordinate