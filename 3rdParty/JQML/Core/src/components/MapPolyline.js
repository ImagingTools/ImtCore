import {QtObject} from './QtObject'

import Feature from 'ol/Feature.js';
import LineString from 'ol/geom/LineString';
import { Style, Stroke } from 'ol/style.js';

export class MapPolyline extends QtObject  {
    constructor(args) {
        super(args)
        
        this.$cP('autoFadeIn', true)
        this.$cP('backend', null)
        this.$cP('path', []).connect(this.$updateFeature.bind(this))
        this.$cPC('line', {
            width: 1,
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
            geometry: new LineString(path),
        })
        this.$updateStyleFeature()
        this.parent.$source.addFeature(this.$feature)
    }
    $updateStyleFeature(){
        if(this.$feature){
            this.$feature.setStyle(new Style({
                stroke: new Stroke({
                    color: this.line.color,
                    width: this.line.width,
                })
            }))
        }
    }

}

QML.MapPolyline = MapPolyline