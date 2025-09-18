const GroupProperty = require("../QtQml/GroupProperty")
const Real = require("../QtQml/Real")
const Signal = require("../QtQml/Signal")

class GeoCoordinate extends GroupProperty {
    static meta = {
        latitude: {type:Real, value:0, },
        longitude: {type:Real, value:0, },
        altitude: {type:Real, value:0, },
        
        latitudeChanged: {type:Signal, args:[]},
        longitudeChanged: {type:Signal, args:[]},
        altitudeChanged: {type:Signal, args:[]},
    }

    
}



module.exports = GeoCoordinate