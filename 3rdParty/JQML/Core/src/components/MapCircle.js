import {QtObject} from './QtObject'

import Feature from 'ol/Feature.js';
import Circle from 'ol/geom/Circle.js';
import { Style, Fill, Stroke } from 'ol/style.js';

export class MapCircle extends QtObject  {
    constructor(args) {
        super(args)
        
        this.$cP('autoFadeIn', true)
        this.$cP('backend', null)
        this.$cP('center', null).connect(this.$updateFeature.bind(this))
        this.$cP('color', 'transparent').connect(this.$updateFeature.bind(this))
        this.$cP('opacity', 1).connect(this.$updateFeature.bind(this))
        this.$cP('radius', 0).connect(this.$updateFeature.bind(this))
        this.$cPC('border', {
            width: 0,
            color: 'black',
        }).connect(this.$updateFeature.bind(this))
    }
    $updateFeature(){
        if(!this.center) return
        if(!this.parent.$source){
            this.parent.$queue.add(this)
            return
        }
        if(this.$feature){
            this.parent.$source.removeFeature(this.$feature)
        }
        this.$feature = new Feature({
            geometry: new Circle(
                QtPositioning.coordinate(this.center.latitude, this.center.longitude),
                this.radius
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

}

QML.MapCircle = MapCircle