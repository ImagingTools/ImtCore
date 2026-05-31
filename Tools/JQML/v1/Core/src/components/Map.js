import {Item} from './Item'

import $Map from 'ol/Map.js'
import $OSM from 'ol/source/OSM.js'
import $TileLayer from 'ol/layer/Tile.js'
import $View from 'ol/View.js'
import $SourceVector from 'ol/source/Vector'
import $LayerVector from 'ol/layer/Vector'
import { transform } from 'ol/proj'
import { QtPositioning } from '../utils/QtPositioning'

export class Map extends Item {
    $queue = new Set()

    constructor(args) {
        Core.loadMapTools()
        super(args)

        this.$cP('plugin', undefined, this.$pluginChanged)
        this.$cP('center', QtPositioning.coordinate(0, 0), this.$centerChanged)
        this.$cP('zoomLevel', 2, this.$zoomLevelChanged)
        this.$cP('copyrightsVisible', true)
        this.$cP('bearing', 0, this.$bearingChanged)
        this.$cP('mapReady', false, this.$mapReadyChanged)
        this.$cPC('gesture', {
            acceptedGestures : '',
            enabled : false,
            flickDeceleration : 2500,
            maximumZoomLevelChange : 4,
            panActive : false,
            pinchActive : false,
            preventStealing : false,
            rotationActive : false,
            tiltActive : false,
        })

        this.$initialized = false
        this['Component.completed'].connect(this.$init.bind(this))
    }

    $init(){
        if(this.$initialized) return

        this.$source = new $SourceVector({
			features: [],
		})

		this.$layer = new $LayerVector({
			source: this.$source,
		})
		this.$map = new $Map({
			target: this.dom,
			layers: [
				new $TileLayer({
					source: new $OSM({
                        url: this.plugin.$url,
                        crossOrigin: 'anonymous',
                    }),
				}),
				this.$layer,
			],
			controls: [],
			view: new $View({
				center: transform([this.center.longitude, this.center.latitude], 'EPSG:4326','EPSG:3857'),
				zoom: this.zoomLevel,
                rotation: -this.bearing*Math.PI/180,
			}),
		})
        this.$initialized = true

        this.$map.on('moveend', (e)=>{
            for(let child of this.children){
                if(child.$qmlClassName === 'MapQuickItem' && child.sourceItem){
                    let rect = child.sourceItem.dom.getBoundingClientRect()
                    child.x = rect.x - this.x
                    child.y = rect.y - this.y
                }
            }
            // this.$p.zoomLevel.preventDefault = true
            this.$p.center.preventDefault = false
            // this.$p.bearing.preventDefault = true
            this.zoomLevel = this.$map.getView().getZoom()

            // let center = this.$map.getView().getCenter()
            // let point = transform(center, 'EPSG:3857', 'EPSG:4326')
            // this.center = { longitude: point[0], latitude: point[1] }

            this.bearing = -this.$map.getView().getRotation()*180/Math.PI
        })
        this.$map.on('movestart', (e)=>{
            this.$p.center.preventDefault = true
            // this.$p.zoomLevel.preventDefault = false
            // this.$p.center.preventDefault = false
            // this.$p.bearing.preventDefault = false
        })
        // this.$map.getView().on('change:resolution', (e) => {
        //     this.zoomLevel = this.$map.getView().getZoom()
        // })
        this.$map.getView().on('change:center', (e) => {
            let center = this.$map.getView().getCenter()

            let point = transform(center, 'EPSG:3857', 'EPSG:4326')
            this.center = { longitude: point[0], latitude: point[1] }
        })
        // this.$map.getView().on('change:rotation', (e) => {
        //     this.bearing = -this.$map.getView().getRotation()*180/Math.PI
        // })
        this.$map.once('postrender', (e) => {
            this.mapReady = true
        })
    }

    toCoordinate(position, clipToViewPort){
        let point = transform(this.$map.getCoordinateFromPixel([position.x, position.y]), 'EPSG:3857', 'EPSG:4326')
        return {
            longitude: point[0],
            latitude: point[1],
        }
    }

    fromCoordinate(coordinate, clipToViewPort){
        let point = this.$map.getPixelFromCoordinate(transform([coordinate.longitude, coordinate.latitude], 'EPSG:4326','EPSG:3857'))
        return {
            x: point[0],
            y: point[1],
        }
    }

    addMapItem(item){
        item.$uP()
        if(item.$qmlClassName === 'MapQuickItem'){
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
                f.$uP()
                f.$updateFeature(true)
            }
            this.$queue.clear()
        }
    }

    $bearingChanged(){
        if(!this.$p.bearing.preventDefault && this.$map){
            this.$map.getView().setRotation(-this.bearing*Math.PI/180)
        }
    }

    $childChanged(child){
        if(child.$qmlClassName === 'MapQuickItem' && child.sourceItem){
            let rect = child.sourceItem.dom.getBoundingClientRect()
            child.x = rect.x - this.x
            child.y = rect.y - this.y
        }
    }

    $domCreate(){
        super.$domCreate()
        //this.dom.style.backgroundColor = this.color
    }

    $pluginChanged(){

    }
    $centerChanged(){
        if(!this.$p.center.preventDefault && this.$map){
            this.$map.getView().setCenter(transform([this.center.longitude, this.center.latitude], 'EPSG:4326','EPSG:3857'))
        }
    }
    $zoomLevelChanged(){
        if(!this.$p.zoomLevel.preventDefault && this.$map){
            this.$map.getView().setZoom(this.zoomLevel)
        }
    }
    $destroy(){
        this.$queue.clear()
        super.$destroy()
    }
}

QML.Map = Map