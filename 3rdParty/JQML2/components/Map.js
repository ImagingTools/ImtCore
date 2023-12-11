const { Item } = require('./Item')
const { QReal, QBool, MapGestureArea, QVar } = require('../utils/properties')
const { QtPositioning } = require('../utils/QtPositioning')

class Map extends Item {
    static defaultProperties = {
        plugin: { type: QVar, value: '', changed: '$pluginChanged' },
        center: { type: QVar, value: QtPositioning.coordinate(0, 0), changed: '$centerChanged' },
        zoomLevel: { type: QReal, value: 2, changed: '$zoomLevelChanged' },
        copyrightsVisible: { type: QBool, value: true },
        bearing: { type: QReal, value: 0, changed: '$bearingChanged' },
        mapReady: { type: QBool, value: false, changed: '$mapReadyChanged' },
        gesture: { type: MapGestureArea },

    }



    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.getProperty('context').get().getProperty('mapTools').reset(true)
        this.$queue = new Set()
        this.$initialized = false
        this.getSignal('Component.completed').connect(this, this.$init)
    }

    $init(){
        if(this.$initialized) return

        this.$source = new OpenLayers.SourceVector({
			features: [],
		})

		this.$layer = new OpenLayers.LayerVector({
			source: this.$source,
		})
		this.$map = new OpenLayers.Map({
			target: this.getDom(),
			layers: [
				new OpenLayers.TileLayer({
					source: new OpenLayers.OSM({
                        url: this.plugin.$url,
                        crossOrigin: 'anonymous',
                    }),
				}),
				this.$layer,
			],
			controls: [],
			view: new OpenLayers.View({
				center: OpenLayers.transform([this.center.longitude, this.center.latitude], 'EPSG:4326','EPSG:3857'),
				zoom: this.zoomLevel,
                rotation: -this.bearing*Math.PI/180,
			}),
		})
        this.$initialized = true

        this.$map.on('moveend', (e)=>{
            for(let child of this.$children){
                if(child instanceof MapQuickItem && child.sourceItem){
                    let rect = child.sourceItem.getDom().getBoundingClientRect()
                    child.x = rect.x - this.x
                    child.y = rect.y - this.y
                }
            }

            this.getProperty('center').preventDefault = false

            this.zoomLevel = this.$map.getView().getZoom()

            this.bearing = -this.$map.getView().getRotation()*180/Math.PI
        })
        this.$map.on('movestart', (e)=>{
            this.getProperty('center').preventDefault = true
        })

        this.$map.getView().on('change:center', (e) => {
            let center = this.$map.getView().getCenter()

            let point = OpenLayers.transform(center, 'EPSG:3857', 'EPSG:4326')
            this.center = { longitude: point[0], latitude: point[1] }
        })

        this.$map.once('postrender', (e) => {
            this.mapReady = true
        })
    }

    toCoordinate(position, clipToViewPort){
        let point = OpenLayers.transform(this.$map.getCoordinateFromPixel([position.x, position.y]), 'EPSG:3857', 'EPSG:4326')
        return {
            longitude: point[0],
            latitude: point[1],
        }
    }

    fromCoordinate(coordinate, clipToViewPort){
        let point = this.$map.getPixelFromCoordinate(OpenLayers.transform([coordinate.longitude, coordinate.latitude], 'EPSG:4326','EPSG:3857'))
        return {
            x: point[0],
            y: point[1],
        }
    }

    addMapItem(item){
        if(item instanceof MapQuickItem){
            this.$map.addOverlay(item.$overlay)
        } else {
            this.$source.addFeature(item.$feature)
        }
    }
    removeMapItem(item){
        item.$destroy()
    }

    $mapReadyChanged(){
        if(this.mapReady){
            for(let f of this.$queue){
                f.$updateFeature(true)
            }
            this.$queue.clear()
        }
    }

    $bearingChanged(){
        if(!this.getProperty('bearing').preventDefault && this.$map){
            this.$map.getView().setRotation(-this.bearing*Math.PI/180)
        }
    }

    $childChanged(child){
        if(child instanceof MapQuickItem && child.sourceItem){
            let rect = child.sourceItem.getDom().getBoundingClientRect()
            child.x = rect.x - this.x
            child.y = rect.y - this.y
        }
    }

    $pluginChanged(){

    }
    $centerChanged(){
        if(!this.getProperty('center').preventDefault && this.$map){
            this.$map.getView().setCenter(OpenLayers.transform([this.center.longitude, this.center.latitude], 'EPSG:4326','EPSG:3857'))
        }
    }
    $zoomLevelChanged(){
        if(!this.getProperty('zoomLevel').preventDefault && this.$map){
            this.$map.getView().setZoom(this.zoomLevel)
        }
    }
    $destroy(){
        this.$queue.clear()
        super.$destroy()
    }
}

module.exports.Map = Map