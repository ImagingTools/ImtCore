RegExp.prototype.toPartialMatchRegex = function() {
    "use strict";
    
    var re = this,
        source = this.source,
        i = 0;
    
    function process () {
        var result = "",
            tmp;

        function appendRaw(nbChars) {
            result += source.substr(i, nbChars);
            i += nbChars;
        };
        
        function appendOptional(nbChars) {
            result += "(?:" + source.substr(i, nbChars) + "|$)";
            i += nbChars;
        };

        while (i < source.length) {
            switch (source[i])
            {
                case "\\":
                    switch (source[i + 1])
                    {
                        case "c":
                            appendOptional(3);
                            break;
                            
                        case "x":
                            appendOptional(4);
                            break;
                            
                        case "u":
                            if (re.unicode) {
                                if (source[i + 2] === "{") {
                                    appendOptional(source.indexOf("}", i) - i + 1);
                                } else {
                                    appendOptional(6);
                                }
                            } else {
                                appendOptional(2);
                            }
                            break;

                        case "p":
                        case "P":
                            if (re.unicode) {
                                appendOptional(source.indexOf("}", i) - i + 1);
                            } else {
                                appendOptional(2);
                            }
                            break;

                        case "k":
                            appendOptional(source.indexOf(">", i) - i + 1);
                            break;
                            
                        default:
                            appendOptional(2);
                            break;
                    }
                    break;
                    
                case "[":
                    tmp = /\[(?:\\.|.)*?\]/g;
                    tmp.lastIndex = i;
                    tmp = tmp.exec(source);
                    appendOptional(tmp[0].length);
                    break;
                    
                case "|":
                case "^":
                case "$":
                case "*":
                case "+":
                case "?":
                    appendRaw(1);
                    break;
                    
                case "{":
                    tmp = /\{\d+,?\d*\}/g;
                    tmp.lastIndex = i;
                    tmp = tmp.exec(source);
                    if (tmp) {
                        appendRaw(tmp[0].length);
                    } else {
                        appendOptional(1);
                    }
                    break;
                    
                case "(":
                    if (source[i + 1] == "?") {
                        switch (source[i + 2])
                        {
                            case ":":
                                result += "(?:";
                                i += 3;
                                result += process() + "|$)";
                                break;
                                
                            case "=":
                                result += "(?=";
                                i += 3;
                                result += process() + ")";
                                break;
                                
                            case "!":
                                tmp = i;
                                i += 3;
                                process();
                                result += source.substr(tmp, i - tmp);
                                break;

                            case "<":
                                switch (source[i + 3])
                                {
                                    case "=":
                                    case "!":
                                        tmp = i;
                                        i += 4;
                                        process();
                                        result += source.substr(tmp, i - tmp);
                                        break;

                                    default:
                                        appendRaw(source.indexOf(">", i) - i + 1);
                                        result += process() + "|$)";
                                        break;        
                                }
                                break;
                        }
                    } else {
                        appendRaw(1);
                        result += process() + "|$)";
                    }
                    break;
                    
                case ")":
                    ++i;
                    return result;
                    
                default:
                    appendOptional(1);
                    break;
            }
        }
        
        return result;
    }
    
    return new RegExp(process(), this.flags);
};

let XMLProxy = XMLHttpRequest.prototype.open
XMLHttpRequest.prototype.open = function() {  
    let opened = XMLProxy.apply(this, [].slice.call(arguments))
    if(XMLHttpRequest.QMLAuthToken) this.setRequestHeader("X-authentication-token", XMLHttpRequest.QMLAuthToken)
    return opened
}

class ContextController {
    internal = {}

    constructor(externalContext, externalContext2){
        this.external = externalContext
        this.external2 = externalContext2
    }
    add(name, obj){
        this.internal[name] = obj
    }
    get(name, path = []){
        if(name in this.internal){
            return this.internal[name]
        } else if(this.external || this.external2) {
            let obj = undefined
            if(this.external && path.indexOf(this.external) < 0) {
                path.push(this.external)
                obj = this.external.get(name, path)
            }
            if(obj) return obj
            if(this.external2 && path.indexOf(this.external2) < 0) {
                path.push(this.external2)
                obj = this.external2.get(name, path)
            }
            return obj
        } else if(name in Singletons){
            return Singletons[name]
        } else {
            return global[name]
        }
    }
}

global.ContextController = ContextController
global.OriginImage = Image
global.OriginWebSocket = WebSocket
global.queueLink = []
global.UIDList = {}
global.RemoveList = []
global.Singletons = {}
global.classList = {}


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

global.Enums = {}

const listProperties = require('../utils/properties')
for(let prop in listProperties){
    global[prop] = listProperties[prop]
    for(let propName in listProperties[prop].defaultProperties){
        Object.defineProperty(listProperties[prop].prototype, propName, {
            get: function(){
                let property = this.getProperty(propName)
                let caller = global.queueLink[global.queueLink.length-1]
                if(caller) caller.subscribe(property)
                return property.get()
            },
            set: function(newVal){
                this.getProperty(propName).reset(newVal)
            }
        })
    }
    for(let signalName in listProperties[prop].defaultSignals){
        Object.defineProperty(listProperties[prop].prototype, signalName, {
            get: function(){
                return this.getSignal(signalName)
            }
        })
    }
}

const { QSignal } = require('../utils/signal')
global.QSignal = QSignal

const { Qt } = require('../utils/Qt')
global.Qt = Qt

const { QtPositioning } = require('../utils/QtPositioning')
global.QtPositioning = QtPositioning

const listControllers = require('../utils/controllers')


const { listComponents } = require('../components/list')
for(let componentName of listComponents){
    const component = require(`../components/${componentName}`)[componentName]
    for(let propName in component.defaultProperties){
        Object.defineProperty(component.prototype, propName, {
            get: function(){
                let property = this.getProperty(propName)
                let caller = global.queueLink[global.queueLink.length-1]
                if(caller) caller.subscribe(property)
                return property.get()
            },
            set: function(newVal){
                this.getProperty(propName).reset(newVal)
            }
        })
        if(!component.defaultProperties[propName].signalWithout)
        Object.defineProperty(component.prototype, propName+'Changed', {
            get: function(){
                return this.getProperty(propName).getNotify()
            },
        })
    }
    for(let sigName in component.defaultSignals){
        // if(!(sigName in component.prototype))
        Object.defineProperty(component.prototype, sigName, {
            get: function(){
                return this.getSignal(sigName)
            },
        })
    }
    global[componentName] = component
}

global.updateList = []
global.SingletonClass = {}

function removeFunc(){
    while(RemoveList.length){
        RemoveList.shift().$free()
    }
    setTimeout(removeFunc, 5000)
}

window.onload = ()=>{
    for(let prop in listControllers){
        global[prop] = new listControllers[prop]()
    }
    
    global.rootPath = document.body.dataset.root
    document.head.insertAdjacentHTML("beforeend", `
    <style>
        *{padding:0;margin:0;border:0;overflow:hidden;-ms-overflow-style:none;scrollbar-width:none;user-select:none;}
        *::-webkit-scrollbar{width:0;height:0;}
        *,*:before,*:after{-moz-box-sizing:border-box;-webkit-box-sizing:border-box;box-sizing:border-box;}
        :focus,:active{outline:none;}
        a:focus,a:active{outline:none;}
        nav,footer,header,aside{display:block;}
        html,body{height:100%;width:100%;font-size:100%;line-height:1;font-size:14px;-ms-text-size-adjust:100%;-moz-text-size-adjust:100%;-webkit-text-size-adjust:100%;}
        input,button,textarea{font-family:inherit;}
        input::-ms-clear{display:none;}
        button{cursor:pointer;}
        button::-moz-focus-inner{padding:0;border:0;}
        a,a:visited{text-decoration:none;}
        a:hover{text-decoration:none;}
        ul li{list-style:none;}
        img{vertical-align:top;}
        h1,h2,h3,h4,h5,h6{font-size:inherit;font-weight:inherit;}
        br {}

    </style>`)  

    const mainRoot = new Item()
    window.mainRoot = mainRoot
    mainRoot.setDom(document.body)
    mainRoot.getProperty('width').reset(window.innerWidth)
    mainRoot.getProperty('height').reset(window.innerHeight)
    window.addEventListener('resize', ()=>{
        mainRoot.getProperty('width').reset(window.innerWidth)
        mainRoot.getProperty('height').reset(window.innerHeight)
    })
    mainRoot.getProperty('context').reset(mainRoot)

    location.changeHref = (href)=>{
        location.href = href
    }
    location.searchParams = {
        get(){
            let res = {}
            let params = location.search.slice(1).split('&')
            for(let param of params){
                let temp = param.split('=')
                if(temp.length === 2) res[temp[0]] = temp[1]
            }
            return res
        }
    }

    mainRoot.languages = {}
    mainRoot.XMLParser = new DOMParser()
    mainRoot.updateLanguage = ()=>{
        if(mainRoot.application && mainRoot.language && !mainRoot.languages[mainRoot.language]){
            let langPrefixList = []
            if(typeof mainRoot.application === 'string'){
                langPrefixList.push(mainRoot.application)
            } else if(Array.isArray(mainRoot.application)){
                langPrefixList = mainRoot.application
            }
            for(let langPrefix of langPrefixList){
                let xhr = new XMLHttpRequest()
                if(langPrefix){
                    xhr.open('GET', `../Translations/${langPrefix}_${mainRoot.language}.ts`, false)
                } else {
                    xhr.open('GET', `../Translations/${mainRoot.language}.ts`, false)
                }
                xhr.onload = ()=>{
                    if (xhr.readyState === XMLHttpRequest.DONE){
                        let xml = mainRoot.XMLParser.parseFromString(xhr.responseText, 'text/xml')
                        let messages = xml.getElementsByTagName('message')
                        let dict = {}
                        for(let message of messages){
                            let source = message.getElementsByTagName('source')[0]
                            let translation = message.getElementsByTagName('translation')[0]
                            dict[source.innerHTML] = translation.innerHTML
                        }
                        if(!mainRoot.languages[mainRoot.language]){
                            mainRoot.languages[mainRoot.language] = dict
                        } else {
                            Object.assign(mainRoot.languages[mainRoot.language], dict)
                        }
                    }
                }
                xhr.send()
            }
            
        }
        
    }

    mainRoot.createProperty('location',QVar,location)
    mainRoot.createProperty('history',QVar,history)
    mainRoot.createProperty('language',QString,'')
    mainRoot.createProperty('application',QVar,'')
    mainRoot.getProperty('language').getNotify().connect(mainRoot, mainRoot.updateLanguage)
    mainRoot.getProperty('application').getNotify().connect(mainRoot, mainRoot.updateLanguage)
    mainRoot.$focusedElements = []
    mainRoot.$activeFocusedElements = []

    mainRoot.createProperty('mapTools',QBool,false)
    mainRoot.getProperty('mapTools').getNotify().connect(mainRoot, ()=>{
        let mapStyle = document.createElement('link')
        mapStyle.rel = 'stylesheet'
        mapStyle.href = 'https://cdn.jsdelivr.net/npm/ol@v7.3.0/ol.css'
        document.head.appendChild(mapStyle)
    })

    console.time('build')
    for(let name in SingletonClass){
        Object.defineProperty(Singletons, name, {
            get(){
                // obj.$complete()
                if(!Singletons[`$${name}`]) {
                    Singletons[`$${name}`] = new SingletonClass[name]()
                    if(!(Singletons[`$${name}`].$id in Singletons)){
                        Object.defineProperty(Singletons, Singletons[`$${name}`].$id, {
                            get(){
                                return Singletons[`$${name}`]
                            }
                        })
                    }
                    Singletons[`$${name}`].$complete()
                }
                return Singletons[`$${name}`]
            }
        })
    }
    let root = new (Function('return '+document.body.dataset.qml.replace('.qml', ''))())(mainRoot)

    while(updateList.length){
        for(let update of updateList.splice(0, updateList.length)){
            update()
        }
    }
    
    mainRoot.$complete()

    removeFunc()
    console.timeEnd('build')
}
