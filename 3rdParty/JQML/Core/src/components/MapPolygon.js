import {QtObject} from './QtObject'

import Feature from 'ol/Feature.js';
import Polygon from 'ol/geom/Polygon';
import { Style, Fill, Stroke } from 'ol/style.js';

export class MapPolygon extends QtObject  {
    constructor(args) {
        super(args)
        
        this.$cP('autoFadeIn', true)
        this.$cP('backend', null)
        this.$cP('path', []).connect(this.$updateFeature.bind(this))
        this.$cP('color', 'transparent').connect(this.$updateFeature.bind(this))
        this.$cP('opacity', 1).connect(this.$updateFeature.bind(this))
        this.$cPC('border', {
            width: 0,
            color: 'black',
        }).connect(this.$updateFeature.bind(this))
    }
    $updateFeature(){
        if(!this.path && !this.path.length) return
        if(!this.parent.$source){
            this.parent.$queue.add(this)
            return
        }
        if(this.$feature){
            this.parent.$source.removeFeature(this.$feature)
        }
        let path = []
        for(let p of this.path){
            path.push(QtPositioning.coordinate(p.latitude, p.longitude))
        }
        this.$feature = new Feature({
            geometry: new Polygon(
                [[
                    ...path
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

}

QML.MapPolygon = MapPolygon