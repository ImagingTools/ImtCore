const QtObject = require("./QtObject")
const Property = require("./Property")
const Var = require("./Var")
const Signal = require("./Signal")
const SDLSignal = require("./SDLSignal")

class SDLObject extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        jsData: { type: Var, value: null},
        jsDataChanged: {type:Signal, args:[]},
    })

    static handleTree = {
        get: (target, key)=>{
            let flag = global.queueFlag[global.queueFlag.length - 1]
            let link = Property.queueLink[Property.queueLink.length - 1]
            if(link && flag){
                if(!target.__connections) target.__connections = {}
                
                if(!link.target.__depends[link.name]) link.target.__depends[link.name] = []

                let found = false
                for(let connectionObj of link.target.__depends[link.name]){
                    if(connectionObj.name === key + 'Changed' && connectionObj.target === target){
                        found = true
                        break
                    }
                }

                if(!found){
                    let connectionObj = SDLSignal.get(target, key + 'Changed').connect(()=>{
                        if(!link.target[link.name+'__updating']){
                            link.target[link.name+'__updating'] = true
                            link.meta.type.set(link.target, link.name, link.func, link.meta)
                            delete link.target[link.name+'__updating']
                        }
                    })
            
                    link.target.__depends[link.name].push(connectionObj)
                }

            }
            return target[key.replaceAll('m_', '')]
          
        },
        set: (target, key, value)=>{
            let oldValue = target[key.replaceAll('m_', '')]
            let newValue = value

            if(oldValue !== newValue){
                target[key.replaceAll('m_', '')] = value
                SDLSignal.get(target, key + 'Changed')(oldValue, newValue)
            }
            

            return true
        }
    }

    static replacer(key, value){
        if(key === '__parent' || key === '__connections') return undefined

        return value
    }

    static updateTree(tree, parent){
        for(let key in tree){
            if(tree[key] && typeof tree[key] === 'object'){
                let self = tree[key]
                let proxy = new Proxy(self, SDLObject.handleTree)
                SDLObject.updateTree(self, proxy)
                tree[key] = proxy
                if(parent) self.__parent = parent
            }
        }
    }
    
    jsData = {}

    fromJSON(json){
        let tree = JSON.parse(json)
        let proxy = new Proxy(tree, SDLObject.handleTree)
        SDLObject.updateTree(tree, proxy)
        this.jsData = proxy
    }

    toJSON(){
        return JSON.stringify(this.jsData, SDLObject.replacer)
    }

    assign(branch){
        this.jsData = branch
    }
}



module.exports = SDLObject