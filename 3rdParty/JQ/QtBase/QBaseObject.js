const BaseObject = require("./BaseObject")
const Property = require("../QtQml/Property")
const GroupProperty = require("../QtQml/GroupProperty")

class QBaseObject extends BaseObject {
    static singleton = false
    static uid = 0

    static meta = {}

    static initialize(){
        let meta = this.meta

        for(let key in meta){
            if(key in this.prototype) continue

            if(meta[key].type.isAssignableFrom(Property)){
                Object.defineProperty(this.prototype, key, {
                    get:function(){
                        if(this.__destroyed) return

                        if(!(key in this.__dataQml)) {
                            this.__dataQml[key] = this.constructor.meta[key].type.create(this, this.constructor.meta[key])
                        }

                        return this.__dataQml[key].__get()
                    },
                    set:function(value){
                        if(this.__destroyed) return

                        if(!(key in this.__dataQml)) {
                            this.__dataQml[key] = this.constructor.meta[key].type.create(this, this.constructor.meta[key])
                        }

                        this.__dataQml[key].__reset(value)
                    },
                })
            } else if(meta[key].type.isAssignableFrom(GroupProperty)){
                Object.defineProperty(this.prototype, key, {
                    get:function(){
                        if(this.__destroyed) return

                        if(!(key in this.__dataQml)) {
                            this.__dataQml[key] = this.constructor.meta[key].type.create(this, this.constructor.meta[key])
                        }

                        return this.__dataQml[key]
                    },
                    set:function(value){
                        if(this.__destroyed) return

                        if(!(key in this.__dataQml)) {
                            this.__dataQml[key] = this.constructor.meta[key].type.create(this, this.constructor.meta[key])
                        }

                        this.__dataQml[key].__assign(value)
                    },
                })
            } else {
                Object.defineProperty(this.prototype, key, {
                    get:function(){
                        if(this.__destroyed) return

                        if(!(key in this.__dataQml)) {
                            this.__dataQml[key] = this.constructor.meta[key].type.create(this, this.constructor.meta[key])
                        }

                        return this.__dataQml[key]
                    },
                })
            }
        }
    }

    static create(parent=null, model=null, meta={}, properties=[], isRoot=true){
        if(this.singleton) this.initialize()
        
        let obj = super.create(parent, model, meta, properties, isRoot)
        
        for(let key in meta){
            if(meta[key].type.isAssignableFrom(Property)){
                Object.defineProperty(obj, key, {
                    get:function(){
                        if(obj.__destroyed) return

                        if(!(key in obj.__dataQml)) {
                            obj.__dataQml[key] = meta[key].type.create(obj, meta[key])
                        }

                        return obj.__dataQml[key].__get()
                    },
                    set:function(value){
                        if(obj.__destroyed) return

                        if(!(key in obj.__dataQml)) {
                            obj.__dataQml[key] = meta[key].type.create(obj, meta[key])
                        }

                        obj.__dataQml[key].__reset(value)
                    },
                })
            } else if(meta[key].type.isAssignableFrom(GroupProperty)){
                Object.defineProperty(obj, key, {
                    get:function(){
                        if(obj.__destroyed) return

                        if(!(key in obj.__dataQml)) {
                            obj.__dataQml[key] = meta[key].type.create(obj, meta[key])
                        }

                        return obj.__dataQml[key]
                    },
                    set:function(value){
                        if(obj.__destroyed) return

                        if(!(key in obj.__dataQml)) {
                            obj.__dataQml[key] = meta[key].type.create(obj, meta[key])
                        }

                        obj.__dataQml[key].__assign(value)
                    },
                })
            } else {
                Object.defineProperty(obj, key, {
                    get:function(){
                        if(obj.__destroyed) return

                        if(!(key in obj.__dataQml)) {
                            obj.__dataQml[key] = meta[key].type.create(obj, meta[key])
                        }

                        return obj.__dataQml[key]
                    },
                })
            }
        }

        obj.__properties = properties
        obj.__uid = this.uid++

        return obj
    }

    __dataQml = {}
    

    __getDataQml(key){
        if(this.__destroyed) return

        if(!(key in this.__dataQml)) {
            this.__dataQml[key] = this.constructor.meta[key].type.create(this, this.constructor.meta[key])
        }

        return this.__dataQml[key]
    }

    __beginUpdate(){

    }

    __endUpdate(){
        
    }

    __resolve(){
        return this
    }

    __destroy(){
        
    }

    blockSignals(flag){
        this.__blockedSignals = flag
    }

    signalsBlocked(){
        return this.__blockedSignals
    }

    deleteLater(){
        this.destroy()
    }

    destroy(){
        JQApplication.deleteLater(this)
    }    
}

module.exports = QBaseObject