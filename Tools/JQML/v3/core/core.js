
import Map from 'ol/Map.js'
import OSM from 'ol/source/OSM.js'
import TileLayer from 'ol/layer/Tile.js'
import View from 'ol/View.js'
import SourceVector from 'ol/source/Vector'
import LayerVector from 'ol/layer/Vector'
import { transform } from 'ol/proj'
import { defaults, MouseWheelZoom } from 'ol/interaction'

import Feature from 'ol/Feature.js'
import Circle from 'ol/geom/Circle.js'
import Polygon from 'ol/geom/Polygon'
import LineString from 'ol/geom/LineString'
import Overlay from 'ol/Overlay.js'
import { Style, Fill, Stroke } from 'ol/style.js'

global.OpenLayers = {
    Map: Map,
    OSM: OSM,
    TileLayer: TileLayer,
    View: View,
    SourceVector: SourceVector,
    LayerVector: LayerVector,
    Feature: Feature,
    Circle: Circle,
    Polygon: Polygon,
    Style: Style, 
    Fill: Fill, 
    Stroke: Stroke,
    LineString: LineString,
    Overlay: Overlay,
    MouseWheelZoom: MouseWheelZoom,
    transform: transform,
    defaults: defaults,
}

global.objectsCount = 0
global.JQContext = require('./JQContext')
global.JQApplication = require('./JQApplication')
global.JQApplication.init()

global.objectsNames = {}
global.addObjectName = function(name){
    if(!(name in global.objectsNames)){
        global.objectsNames[name] = 0
    }

    global.objectsNames[name]++
}

global.removeObjectName = function(name){
    if(name in global.objectsNames){
        global.objectsNames[name]--
    }
}