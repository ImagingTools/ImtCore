const QtObject = require("../QtQml/QtObject")
const Bool = require("../QtQml/Bool")
const Var = require("../QtQml/Var")
const Real = require("../QtQml/Real")
const Point = require("../QtQml/Point")
const GeoCoordinate = require("../QtPositioning/GeoCoordinate")
const Signal = require("../QtQml/Signal")


class MapQuickItem extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        autoFadeIn: { type: Bool, value: true },
        coordinate: { type: GeoCoordinate },
        sourceItem: { type: Var },
        visible: { type: Bool, value: true },
        zoomLevel: { type: Real, value: 0 },
        anchorPoint: { type: Point },
   
        autoFadeInChanged: { type: Signal },
        coordinateChanged: { type: Signal },
        sourceItemChanged: { type: Signal },
        visibleChanged: { type: Signal },
        zoomLevelChanged: { type: Signal },
        anchorPointChanged: { type: Signal },
    })


    SLOT_coordinateChanged(oldValue, newValue){
        if(newValue !== undefined && newValue.latitude !== undefined && newValue.longitude !== undefined){
            if(this.__overlay){
                this.__overlay.setPosition(OpenLayers.transform([newValue.longitude, newValue.latitude], 'EPSG:4326','EPSG:3857'))
            } else {
                this.__updateFeature()
            }
        }
    }
    SLOT_anchorPointChanged(oldValue, newValue){
        if(this.__overlay){
            this.__overlay.setOffset([-newValue.x, -newValue.y])
        }
    }
    SLOT_sourceItemChanged(oldValue, newValue){
        if(this.__overlay && this.parent.__map) this.parent.__map.removeOverlay(this.__overlay)
        
        this.__updateFeature()
    }
    __updateFeature(added = false){
        if(!this.coordinate || this.coordinate.latitude === undefined || this.coordinate.longitude === undefined || !this.sourceItem) return
        if(!this.parent.__source){
            this.parent.__queue.add(this)
            return
        }
        this.sourceItem.__setDOMStyle({
            position: 'relative'
        })

        this.__overlay = new OpenLayers.Overlay({
            element: this.sourceItem.__getDOM(),
            offset: [-this.anchorPoint.x, -this.anchorPoint.y]
        })
        this.__overlay.setPosition(OpenLayers.transform([this.coordinate.longitude, this.coordinate.latitude], 'EPSG:4326','EPSG:3857'))
        
        if(added) this.parent.addMapItem(this)
    }

    __destroy(){
        if(this.__overlay && this.parent.__map) this.parent.__map.removeOverlay(this.__overlay)
        super.destroy()
    }
}



module.exports = MapQuickItem