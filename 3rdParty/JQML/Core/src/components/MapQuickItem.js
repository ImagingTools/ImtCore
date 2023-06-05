import {QtObject} from './QtObject'
import Overlay from 'ol/Overlay.js'
import {transform} from 'ol/proj'

export class MapQuickItem extends QtObject  {
    constructor(args) {
        super(args)
        
        this.$cP('autoFadeIn', true)
        this.$cP('zoomLevel', 0)
        this.$cP('coordinate', undefined).connect(this.$coordinateChanged.bind(this))
        this.$cP('sourceItem', undefined).connect(this.$updateFeature.bind(this))
        this.$cPC('anchorPoint', {
            x: 0,
            y: 0,
        }).connect(this.$updateFeature.bind(this))
        this.$cP('visible', true)
    }
    $coordinateChanged(){
        if(this.coordinate && this.coordinate.latitudeChanged && this.coordinate.longitudeChanged){
            this.coordinate.latitudeChanged.connect(this.$updateFeature.bind(this))
            this.coordinate.longitudeChanged.connect(this.$updateFeature.bind(this))
        }
        this.$updateFeature()
    }
    $updateFeature(added = false){
        if(!this.coordinate || this.coordinate.latitude === undefined || this.coordinate.longitude === undefined || !this.sourceItem) return
        if(!this.parent.$source){
            this.parent.$queue.add(this)
            return
        }
        this.sourceItem.dom.style.position = 'relative'

        this.$overlay = new Overlay({
            element: this.sourceItem.dom,
            offset: [-this.anchorPoint.x, -this.anchorPoint.y]
        })
        //let point = QtPositioning.coordinate(this.coordinate.latitude, this.coordinate.longitude)
        this.$overlay.setPosition(transform([this.coordinate.longitude, this.coordinate.latitude], 'EPSG:4326','EPSG:3857'))
        if(added) this.parent.addMapItem(this)
    }

    $destroy(){
        if(this.$overlay && this.parent.$map) this.parent.$map.removeOverlay(this.$overlay)
        super.$destroy()
    }
}

QML.MapQuickItem = MapQuickItem