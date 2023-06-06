import {QtObject} from './QtObject'
import Overlay from 'ol/Overlay.js'
import {transform} from 'ol/proj'

export class MapQuickItem extends QtObject  {
    constructor(args) {
        super(args)
        
        this.$cP('autoFadeIn', true)
        this.$cP('zoomLevel', 0)
        this.$cP('coordinate', undefined).connect(this.$coordinateChanged.bind(this))
        this.$cP('sourceItem', undefined).connect(this.$sourceItemChanged.bind(this))
        this.$cPC('anchorPoint', {
            x: 0,
            y: 0,
        }).connect(this.$anchorPointChanged.bind(this))
        this.$cP('visible', true)
    }
    $coordinateChanged(){
        if(this.coordinate !== undefined && this.coordinate.latitude !== undefined && this.coordinate.longitude !== undefined){
            if(this.$overlay){
                this.$overlay.setPosition(transform([this.coordinate.longitude, this.coordinate.latitude], 'EPSG:4326','EPSG:3857'))
            } else {
                this.$updateFeature()
            }
        }
    }
    $anchorPointChanged(){
        if(this.$overlay){
            this.$overlay.setOffset([-this.anchorPoint.x, -this.anchorPoint.y])
        }
    }
    $sourceItemChanged(){
        if(this.$overlay && this.parent.$map) this.parent.$map.removeOverlay(this.$overlay)
        
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
        this.$overlay.setPosition(transform([this.coordinate.longitude, this.coordinate.latitude], 'EPSG:4326','EPSG:3857'))
        
        if(added) this.parent.addMapItem(this)
    }

    $destroy(){
        if(this.$overlay && this.parent.$map) this.parent.$map.removeOverlay(this.$overlay)
        super.$destroy()
    }
}

QML.MapQuickItem = MapQuickItem