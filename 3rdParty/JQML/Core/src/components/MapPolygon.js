import {QtObject} from './QtObject'

import Feature from 'ol/Feature.js'
import Polygon from 'ol/geom/Polygon'
import { Style, Fill, Stroke } from 'ol/style.js'
import {transform} from 'ol/proj'

export class MapPolygon extends QtObject  {
    constructor(args) {
        super(args)
        
        this.$cP('autoFadeIn', true)
        this.$cP('backend', null)
        this.$cP('path', [], this.$updateFeature)
        this.$cP('color', 'transparent', this.$updateFeature)
        this.$cP('opacity', 1, this.$updateFeature)
        this.$cPC('border', {
            width: 0,
            color: 'black',
        }, this.$updateFeature)
    }
    $updateFeature(added = false){
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
            path.push(transform([p.longitude, p.latitude], 'EPSG:4326','EPSG:3857'))
        }
        this.$feature = new Feature({
            geometry: new Polygon(
                [[
                    ...path
                ]]
            ),
        })
        this.$updateStyleFeature()
        if(added) this.parent.addMapItem(this)
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
        if(this.$feature && this.parent.$map) this.parent.$source.removeFeature(this.$feature)
        super.$destroy()
    }

}

QML.MapPolygon = MapPolygon