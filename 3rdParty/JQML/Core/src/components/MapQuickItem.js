import {QtObject} from './QtObject'

import Feature from 'ol/Feature.js'
import Polygon from 'ol/geom/Polygon'
import { Style, Fill, Stroke } from 'ol/style.js'
import Overlay from 'ol/Overlay.js'

export class MapQuickItem extends QtObject  {
    constructor(args) {
        super(args)
        
        this.$cP('autoFadeIn', true)
        this.$cP('zoomLevel', 0)
        this.$cP('coordinate', null).connect(this.$updateFeature.bind(this))
        this.$cP('sourceItem', null).connect(this.$updateFeature.bind(this))
        this.$cPC('anchorPoint', {
            x: 0,
            y: 0,
        }).connect(this.$updateFeature.bind(this))
        this.$cP('visible', true)
    }
    $updateFeature(){
        if(!this.coordinate || !this.sourceItem) return
        if(!this.parent.$source){
            this.parent.$queue.add(this)
            return
        }
        this.sourceItem.dom.style.position = 'relative'
        
        this.$overlay = new Overlay({
            element: this.sourceItem.dom,
            offset: [-this.anchorPoint.x, -this.anchorPoint.y]
        })
        this.$overlay.setPosition(this.coordinate)
        this.parent.$map.addOverlay(this.$overlay)
    }

    $destroy(){
        if(this.$overlay && this.parent.$map) this.parent.$map.removeOverlay(this.$overlay)
        super.$destroy()
    }
}

QML.MapQuickItem = MapQuickItem