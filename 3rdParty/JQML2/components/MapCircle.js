const { QtObject } = require('./QtObject')
const { QBorder, QReal, QBool, QVar, QColor } = require('../utils/properties')

class MapCircle extends QtObject {
    static defaultProperties = {
        autoFadeIn: { type: QBool, value: true },
        backend: { type: QVar, value: undefined },
        center: { type: QBool, value: undefined, changed: '$updateFeature' },
        color: { type: QColor, value: 'transparent', changed: '$updateFeature' },
        opacity: { type: QReal, value: 1, changed: '$updateFeature' },
        radius: { type: QReal, value: 0, changed: '$updateFeature' },
        border: { type: QBorder, changed: '$updateFeature' },

    }

    $updateFeature(added = false){
        if(!this.center) return
        if(!this.parent.$source){
            this.parent.$queue.add(this)
            return
        }
        if(this.$feature){
            this.parent.$source.removeFeature(this.$feature)
        }
        this.$feature = new OpenLayers.Feature({
            geometry: new OpenLayers.Circle(
                OpenLayers.transform([this.center.longitude, this.center.latitude], 'EPSG:4326','EPSG:3857'),
                this.radius
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
        if(!this.center) this.center.$destroy()
        super.$destroy()
    }
}

module.exports.MapCircle = MapCircle