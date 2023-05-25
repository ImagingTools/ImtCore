import {QtObject} from './QtObject'

import Feature from 'ol/Feature.js';
import Polygon from 'ol/geom/Polygon';
import { Style, Fill, Stroke } from 'ol/style.js';

export class MapRectangle extends QtObject  {
    constructor(args) {
        super(args)
        
        this.$cP('autoFadeIn', true)
        this.$cP('backend', null)
        this.$cP('topLeft', null).connect(this.$updateFeature.bind(this))
        this.$cP('bottomRight', null).connect(this.$updateFeature.bind(this))
        this.$cP('color', 'transparent').connect(this.$updateFeature.bind(this))
        this.$cP('opacity', 1).connect(this.$updateFeature.bind(this))
        this.$cPC('border', {
            width: 0,
            color: 'black',
        }).connect(this.$updateFeature.bind(this))
    }
    $updateFeature(){
        if(!this.topLeft || !this.bottomRight) return
        if(!this.parent.$source){
            this.parent.$queue.add(this)
            return
        }
        if(this.$feature){
            this.parent.$source.removeFeature(this.$feature)
        }
        this.$feature = new Feature({
            geometry: new Polygon(
                [[
                    QtPositioning.coordinate(this.topLeft.latitude, this.topLeft.longitude),
                    QtPositioning.coordinate(this.topLeft.latitude, this.bottomRight.longitude),
                    QtPositioning.coordinate(this.bottomRight.latitude, this.bottomRight.longitude),
                    QtPositioning.coordinate(this.bottomRight.latitude, this.topLeft.longitude),
                ]]
            ),
        })
        this.$updateStyleFeature()
        this.parent.$source.addFeature(this.$feature)
    }
    $updateStyleFeature(){
        if(this.$feature){
            this.$feature.setStyle(new Style({
                fill: new Fill({
                    color: this.color,
                }),
                stroke: new Stroke({
                    color: this.border.color,
                    width: this.border.width,
                })
            }))
        }
    }

    $destroy(){
        if(!this.topLeft) this.topLeft.$destroy()
        if(!this.bottomRight) this.bottomRight.$destroy()
        super.$destroy()
    }
}

QML.MapRectangle = MapRectangle