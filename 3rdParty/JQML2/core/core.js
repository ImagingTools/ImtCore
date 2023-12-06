Function.prototype.setDependsSignal=function(signal){
    if(!this.depends) this.depends = []
    if(this.depends.indexOf(signal) < 0) this.depends.push(signal)
}
Function.prototype.removeDependsSignal=function(signal){
    let index = this.depends.indexOf(signal)
    if(index >= 0) this.depends.splice(index, 1)
    if(this.depends.length === 0) delete this.depends
}
Function.prototype.clearDependsSignal=function(){
    while(this.depends && this.depends.length){
        let signal = this.depends.pop()
        signal.disconnect(this)
    }
    delete this.depends
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
global.WebImage = Image
global._WebSocket = WebSocket
global.queueLink = []
global.UIDList = {}
global.Singletons = {}


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

const listControllers = require('../utils/controllers')
for(let prop in listControllers){
    global[prop] = new listControllers[prop]()
}

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

window.onload = ()=>{
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

    mainRoot.createProperty('location',QVar,location)
    mainRoot.createProperty('history',QVar,history)
    mainRoot.createProperty('language',QString,'location')
    mainRoot.createProperty('application',QString,'location')

    console.time('build')
    for(let name in SingletonClass){
        let obj = new SingletonClass[name]()
        Singletons[name] = obj
        if(obj.$id) Singletons[obj.$id] = obj
        obj.$complete()
    }
    let root = new (Function('return '+document.body.dataset.qml.replace('.qml', ''))())(mainRoot)
    for(let update of updateList.splice(0, updateList.length)){
        update()
    }
    mainRoot.$complete()
    console.timeEnd('build')
}
