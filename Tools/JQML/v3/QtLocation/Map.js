const Item = require("../QtQuick/Item")
const Bool = require("../QtQml/Bool")
const Real = require("../QtQml/Real")
const Var = require("../QtQml/Var")
const Signal = require("../QtQml/Signal")
const MapGestureArea = require("./MapGestureArea")
const MapQuickItem = require("./MapQuickItem")
const GeoCoordinate = require("../QtPositioning/GeoCoordinate")
const QtPositioning = require("../QtPositioning/QtPositioning")


class Map extends Item {
    static meta = Object.assign({}, Item.meta, {
        plugin: {type: Var, value:'', },
        center: {type: GeoCoordinate, value: QtPositioning.coordinate(0, 0), },
        zoomLevel: {type: Real, value:2, },
        copyrightsVisible: {type: Bool, value:true, },
        bearing: {type: Real, value:0, },
        mapReady: {type: Bool, value:false, },
        gesture: {type: MapGestureArea},

        pluginChanged: {type:Signal, args:[]},
        centerChanged: {type:Signal, args:[]},
        zoomLevelChanged: {type:Signal, args:[]},
        copyrightsVisibleChanged: {type:Signal, args:[]},
        bearingChanged: {type:Signal, args:[]},
        mapReadyChanged: {type:Signal, args:[]},
   
    })

    static create(parent = null, properties = {}){
        let obj = super.create(parent, properties)
        obj.__getDOM().classList.add('Map')

        return obj
    }

    __queue = new Set()

    __complete(){
        this.__source = new OpenLayers.SourceVector({
			features: [],
		})

		this.__layer = new OpenLayers.LayerVector({
			source: this.__source,
		})

		this.__map = new OpenLayers.Map({
			target: this.__getDOM(),
            interactions: OpenLayers.defaults({mouseWheelZoom:false}),
			layers: [
				new OpenLayers.TileLayer({
					source: new OpenLayers.OSM({
                        url: this.plugin.__url,
                        crossOrigin: 'anonymous',
                    }),
				}),
				this.__layer,
			],
			controls: [],
			view: new OpenLayers.View({
				center: OpenLayers.transform([this.center.longitude, this.center.latitude], 'EPSG:4326','EPSG:3857'),
				zoom: this.zoomLevel,
                rotation: -this.bearing*Math.PI/180,
			}),
		})

        super.__complete()

        this.mapReady = true
    }

    toCoordinate(position, clipToViewPort){
        let point = OpenLayers.transform(this.__map.getCoordinateFromPixel([position.x, position.y]), 'EPSG:3857', 'EPSG:4326')
        return {
            longitude: point[0],
            latitude: point[1],
        }
    }

    fromCoordinate(coordinate, clipToViewPort){
        let point = this.__map.getPixelFromCoordinate(OpenLayers.transform([coordinate.longitude, coordinate.latitude], 'EPSG:4326','EPSG:3857'))
        return {
            x: point[0],
            y: point[1],
        }
    }

    addMapItem(item){
        if(item instanceof MapQuickItem){
            this.__map.addOverlay(item.__overlay)
        } else {
            this.__source.addFeature(item.__feature)
        }
    }
    removeMapItem(item){
        item.destroy()
    }

    SLOT_mapReadyChanged(oldValue, newValue){
        if(newValue){
            for(let f of this.__queue){
                f.__updateFeature(true)
            }
            this.__queue.clear()
        }
    }

    SLOT_centerChanged(oldValue, newValue){
        if(this.__map){
            this.__map.getView().setCenter(OpenLayers.transform([newValue.longitude, newValue.latitude], 'EPSG:4326','EPSG:3857'))
        }
    }

    SLOT_zoomLevelChanged(oldValue, newValue){
        if(this.__map){
            this.__map.getView().setZoom(newValue)
        }
    }

}



module.exports = Map