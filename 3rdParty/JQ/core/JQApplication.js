const MouseController = require('./MouseController')
const KeyboardController = require('./KeyboardController')
const ImageController = require('./ImageController')
const TextController = require('./TextController')
const MemoryController = require('./MemoryController')

module.exports = {
    root: null,

    init: function(){ 
        global.queueFlag = []
        
        global.JQModules = {
            QtBase: require('../QtBase/QtBase'),
            Qt: require('../Qt/Qt'),
            QtQml: require('../QtQml/QtQml'),
            QtQuick: require('../QtQuick/QtQuick'),
            Qt5Compat: require('../Qt5Compat/Qt5Compat'),
            QtWebSockets: require('../QtWebSockets/QtWebSockets'),
            QtPositioning: require('../QtPositioning/QtPositioning'),
            QtLocation: require('../QtLocation/QtLocation'),
            __queue: [],
        }

        global.JQModules.QtQml.XMLHttpRequest = XMLHttpRequest
        global.JQModules.QtQml.FileReader = FileReader

        let XMLParser = new DOMParser()

        let Context = (class extends JQModules.QtQml.QObject {
            static singleton = true
            static meta = {
                appName: {type:JQModules.QtQml.string, value:''},
                language: {type:JQModules.QtQml.string, value:'', },
                application: {type:JQModules.QtQml.var, value:[]},
                location: {type:JQModules.QtQml.var, value:window.location},

                languageChanged: {type:JQModules.QtQml.Signal, args:[]},
            }

            static create(...args){
                let obj = super.create(...args)
                obj.__languages = {}
                return obj
            }

            translate(sourceText){
                let translatedText = this.__languages[this.language]
                let result = translatedText ? translatedText[sourceText] : sourceText
                return result ? result : sourceText
            }

            SLOT_languageChanged(oldValue, newValue){
                if(this.application && this.language && !this.__languages[this.language]){
                    let langPrefixList = []
                    if(typeof this.application === 'string'){
                        langPrefixList.push(this.application)
                    } else if(Array.isArray(this.application)){
                        langPrefixList = this.application
                    }
                    for(let langPrefix of langPrefixList){
                        let xhr = new XMLHttpRequest()
                        if(langPrefix){
                            xhr.open('GET', `../Translations/${langPrefix}_${this.language}.ts`, false)
                        } else {
                            xhr.open('GET', `../Translations/${this.language}.ts`, false)
                        }
                        xhr.onload = ()=>{
                            if (xhr.readyState === XMLHttpRequest.DONE){
                                let xml = XMLParser.parseFromString(xhr.responseText, 'text/xml')
                                let messages = xml.getElementsByTagName('message')
                                let dict = {}
                                for(let message of messages){
                                    let source = message.getElementsByTagName('source')[0]
                                    let translation = message.getElementsByTagName('translation')[0]
                                    dict[source.innerHTML] = translation.innerHTML
                                }
                                if(!this.__languages[this.language]){
                                    this.__languages[this.language] = dict
                                } else {
                                    Object.assign(this.__languages[this.language], dict)
                                }
                            }
                        }
                        xhr.send()
                    }
                    
                }
            }
        })
        
        global.JSContext = Context.create()
       
        XMLHttpRequest.querySet = {}
        let XMLProxy = XMLHttpRequest.prototype.open
        XMLHttpRequest.prototype.open = function() {  
            let opened = XMLProxy.apply(this, [].slice.call(arguments))
            if(XMLHttpRequest.QMLAuthToken) this.setRequestHeader("X-authentication-token", XMLHttpRequest.QMLAuthToken)
            return opened
        }
        
        document.head.insertAdjacentHTML("beforeend", `
        <style>
            *{padding:0;margin:0;border:0;overflow:hidden;-ms-overflow-style:none;scrollbar-width:none;user-select:none;-webkit-user-select: none;pointer-events: none;}
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

            .pointer {
                pointer-events: all;
            }

            .Item {
                position: absolute;
                display: flex;
                overflow: unset;
                z-index: 0;
            }
            .Rectangle {
                background-color: white;
            }
            .Repeater {
                display: none;
            }
            .Text, .TextInput, .TextEdit {
                text-align: inherit;
                white-space: pre;
                line-height: normal;
                font-size: 14px;
                font-family: 'Segoe UI';
            }
            .TextInput .impl, .TextEdit .impl {
                display: flex;
                width: 100%;
                height: 100%;
                flex-direction: column;
                z-index: 1;
                pointer-events: all;
            }
            .Image {
                background-repeat: no-repeat;
                background-position: center;
            }

            .Row {
                flex-direction: row;
            }
            .Column {
                flex-direction: column;
            }
            .Row > *{
                position: relative;
            }
            .Column > * {
                position: relative;
            }
            .Flow > * {
                position: relative;
            }
            .Row > *[no-view] {
                display: none;
            }
            .Column > *[no-view] {
                display: none;
            }
            .Flow > *[no-view] {
                display: none;
            }


            .Map > *{
                pointer-events: all;
            }

            *[invisible] {
                display: none;
            }
        </style>`)

        this.root = JQModules.QtQuick.Item.create()
        this.root.__complete()

        window.addEventListener('resize', ()=>{
            this.root.width = window.innerWidth
            this.root.height = window.innerHeight
        })

        window.addEventListener('load', ()=>{
            TextController.init()

            this.root.width = window.innerWidth
            this.root.height = window.innerHeight
            this.root.__connectDOM(document.body)
        })

        MouseController.init()
        KeyboardController.init()
    },

    ImageController: ImageController,
    MouseController: MouseController,
    KeyboardController: KeyboardController,
    TextController: TextController,
    MemoryController: MemoryController,

    objectsAwaitingUpdate: new Set(),
    updateLayers: [],
    initLayers: [],
    deleteObjects: [],

    focusTree: [],
    setFocusTree(tree){
        let unionTree = []
        while(this.focusTree.length || tree.length){
            let origin = this.focusTree.pop()
            let current = tree.pop()

            if(origin === current){
                if(current.__destroyed) continue
                unionTree.unshift(current)
            } else if(current){
                if(current.__destroyed) continue
                unionTree.unshift(current)
            } else if(origin){
                if(origin.__destroyed) continue
                unionTree.unshift(origin)
            }
        }
        
        this.root.__setFocusTree(unionTree)
        this.focusTree = unionTree
    },

    setCursor(cursorShape){
        this.root.__setDOMStyle({
            cursor: cursorShape
        })
    },

    beginUpdate: function(){
        this.updateLayers.push([])
        this.initLayers.push([])
    },

    deleteLater: function(obj){
        if(!obj || obj.__destroyed || obj.__destroying) return

        obj.__destroying = true

        if(this.updateLayers.length){
            if(this.deleteObjects.indexOf(obj) < 0) this.deleteObjects.push(obj)
        } else {
            obj.__destroy()
        }
    },

    initLater(obj){
        this.initLayers[this.initLayers.length-1].push(obj)
    },

    updateLater(obj){
        if(!obj || obj.__destroyed) return

        if(this.updateLayers.length){
            if(!this.objectsAwaitingUpdate.has(obj)){
                obj.__beginUpdate()
                this.objectsAwaitingUpdate.add(obj)
                this.updateLayers[this.updateLayers.length-1].push(obj)
            }
        } else {
            obj.__beginUpdate()
            obj.__endUpdate()
        }
    },

    endUpdate: function(){
        let layer = this.updateLayers.pop()
        let initLayer = this.initLayers.pop()

        if(initLayer){
            for(let obj of initLayer){
                obj.__init()
            }
        } 

        if(layer){
            for(let obj of layer){
                this.objectsAwaitingUpdate.delete(obj)
                if(!obj.__destroyed) obj.__endUpdate()
            }
        } 
        
        if(this.updateLayers.length === 0){
            let objects = this.deleteObjects.slice()
            this.deleteObjects = []
            for(let obj of objects){
                obj.__destroy()
            }
        }
        
    },
}

