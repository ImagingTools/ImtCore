const { QtObject } = require('./QtObject')
const { QPoint, QBool, QVar, QReal } = require('../utils/properties')

class MapQuickItem extends QtObject {
    static defaultProperties = {
        autoFadeIn: { type: QBool, value: true },
        coordinate: { type: QVar, value: undefined, changed: '$coordinateChanged' },
        sourceItem: { type: QVar, value: undefined, changed: '$sourceItemChanged' },
        visible: { type: QBool, value: true },
        zoomLevel: { type: QReal, value: 0 },
        anchorPoint: { type: QPoint, changed: '$anchorPointChanged' },

    }

    $coordinateChanged(){
        if(this.coordinate !== undefined && this.coordinate.latitude !== undefined && this.coordinate.longitude !== undefined){
            if(this.$overlay){
                this.$overlay.setPosition(OpenLayers.transform([this.coordinate.longitude, this.coordinate.latitude], 'EPSG:4326','EPSG:3857'))
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
        this.sourceItem.getDom().style.position = 'relative'

        this.$overlay = new OpenLayers.Overlay({
            element: this.sourceItem.getDom(),
            offset: [-this.anchorPoint.x, -this.anchorPoint.y]
        })
        this.$overlay.setPosition(OpenLayers.transform([this.coordinate.longitude, this.coordinate.latitude], 'EPSG:4326','EPSG:3857'))
        
        if(added) this.parent.addMapItem(this)
    }

    $destroy(){
        if(this.$overlay && this.parent.$map) this.parent.$map.removeOverlay(this.$overlay)
        super.$destroy()
    }
   
}

module.exports.MapQuickItem = MapQuickItem