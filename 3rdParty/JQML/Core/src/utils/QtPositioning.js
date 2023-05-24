import {transform, fromLonLat} from 'ol/proj'
export var QtPositioning = {
    coordinate: function(lat, lon, alt){
        // return fromLonLat([lon, lat])
        return transform([lon, lat], 'EPSG:4326','EPSG:3857')
        //return [lon, lat]
    }
}