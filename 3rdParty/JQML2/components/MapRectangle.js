const { QtObject } = require('./QtObject')
const { QBorder, QReal, QBool, QVar, QColor } = require('../utils/properties')

class MapRectangle extends QtObject {
    static defaultProperties = {
        autoFadeIn: { type: QBool, value: true },
        backend: { type: QVar, value: undefined },
        topLeft: { type: QVar, value: undefined, changed: '$updateFeature' },
        bottomRight: { type: QVar, value: undefined, changed: '$updateFeature' }, 
        color: { type: QColor, value: 'transparent', changed: '$updateFeature' },
        opacity: { type: QReal, value: 1, changed: '$updateFeature' },
        border: { type: QBorder, changed: '$updateFeature' },

    }

    $updateFeature(added = false){
        if(!this.topLeft || !this.bottomRight) return
        if(!this.parent.$source){
            this.parent.$queue.add(this)
            return
        }
        if(this.$feature){
            this.parent.$source.removeFeature(this.$feature)
        }
        this.$feature = new OpenLayers.Feature({ 
            geometry: new OpenLayers.Polygon(
                [[
                    OpenLayers.transform([this.topLeft.longitude, this.topLeft.latitude], 'EPSG:4326','EPSG:3857'),
                    OpenLayers.transform([this.bottomRight.longitude, this.topLeft.latitude], 'EPSG:4326','EPSG:3857'),
                    OpenLayers.transform([this.bottomRight.longitude, this.bottomRight.latitude], 'EPSG:4326','EPSG:3857'),
                    OpenLayers.transform([this.topLeft.longitude, this.bottomRight.latitude], 'EPSG:4326','EPSG:3857')
                ]]
            ),
        })
        this.$updateStyleFeature()
        if(added) this.parent.addMapItem(this)
    }
    $updateStyleFeature(){
        if(this.$feature){
            this.$feature.setStyle(new OpenLayers.Style({
                fill: new OpenLayers.Fill({
                    color: this.color,
                }),
                stroke: new OpenLayers.Stroke({
                    color: this.border.color,
                    width: this.border.width,
                })
            }))
        }
    }

    $destroy(){
        if(this.$feature && this.parent.$map) this.parent.$source.removeFeature(this.$feature)
        if(!this.topLeft) this.topLeft.$destroy()
        if(!this.bottomRight) this.bottomRight.$destroy()
        super.$destroy()
    }
   
}

module.exports.MapRectangle = MapRectangle