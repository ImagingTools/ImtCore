const BaseObject = require("../QtBase/BaseObject")
const Property = require("./Property")

class GroupProperty extends BaseObject {
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
                            this.__dataQml[key] = meta[key].type.create(this, meta[key])
                        }

                        return this.__dataQml[key].__get()
                    },
                    set:function(value){
                        if(this.__destroyed) return

                        if(!(key in this.__dataQml)) {
                            this.__dataQml[key] = meta[key].type.create(this, meta[key])
                        }

                        this.__dataQml[key].__reset(value)
                    },
                })
            } else if(meta[key].type.isAssignableFrom(GroupProperty)){
                Object.defineProperty(this.prototype, key, {
                    get:function(){
                        if(this.__destroyed) return

                        if(!(key in this.__dataQml)) {
                            this.__dataQml[key] = meta[key].type.create(this, meta[key])
                        }

                        return this.__dataQml[key].__get()
                    },
                    set:function(value){
                        if(this.__destroyed) return

                        if(!(key in this.__dataQml)) {
                            this.__dataQml[key] = meta[key].type.create(this, meta[key])
                        }

                        this.__dataQml[key].__assign(value)
                    },
                })
            } else {
                Object.defineProperty(this.prototype, key, {
                    get:function(){
                        if(this.__destroyed) return

                        if(!(key in this.__dataQml)) {
                            this.__dataQml[key] = meta[key].type.create(this, meta[key])
                        }

                        return this.__dataQml[key]
                    },
                })
            }
        }
    }

    static create(parent, meta){
        let obj = super.create(parent, meta)

        obj.__properties = parent.__properties
        obj.__parent = parent

        return obj
    }

    __dataQml = {}

    __getDataQml(key){
        this[key]
        return this.__dataQml[key]
    }

    __emitSignal(){
        if(this.__parent) {
            let signal = this.__parent[this.__signalName]
            if(signal) signal()
        }
    }

    __assign(obj){
        if(obj && typeof obj === 'object'){
            for(let name in obj){
                this[name] = obj[name]
            }
        } else {
            throw `Can not assign ${obj} to this group property`
        }
    }

    __destroy(){
        if(this.__destroyed || !this.__destroying) return
        this.__destroying = true

        super.__destroy()

        for(let key in this.__dataQml){
            if(this.__dataQml[key] instanceof Property || this.__dataQml[key] instanceof GroupProperty){
                this.__dataQml[key].destroy()
            }
        }

        for(let key in this){
            delete this[key]
        }

        this.__destroyed = true
    }

    blockSignals(flag){
        this.__blockedSignals = flag
    }

    signalsBlocked(){
        return this.__blockedSignals
    }
}

module.exports = GroupProperty