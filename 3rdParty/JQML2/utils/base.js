const { QSignal } = require('./signal')

class ComplexObject {
    $properties = {}
    $signals = {}
    
    get(){
        return this
    }

    createProperty(name, type, value){
        this.$properties[name] = new type(value)
        // this.$properties[name].debug = true
        // this.$properties[name].id = this.$id
        Object.defineProperty(this, name, {
            get: function(){
                let property = this.getProperty(name)
                let caller = global.queueLink[global.queueLink.length-1]
                if(caller) caller.subscribe(property)
                return property.get()
            },
            set: function(newVal){
                this.getProperty(name).reset(newVal)
            }
        })
        Object.defineProperty(this, name+'Changed', {
            get: function(){
                return this.getProperty(name).getNotify()
            },
        })
    }

    createVariantProperty(name, type, value){
        this.$properties[name] = new QVariant(value, type)
        Object.defineProperty(this, name, {
            get: function(){
                let property = this.getProperty(name)
                let caller = global.queueLink[global.queueLink.length-1]
                if(caller) caller.subscribe(property)
                return property.get()
            },
            set: function(newVal){
                this.getProperty(name).reset(newVal)
            }
        })
        Object.defineProperty(this, name+'Changed', {
            get: function(){
                return this.getProperty(name).getNotify()
            },
        })
    }

    createSignal(name, ...args){
        this.$signals[name] = new QSignal(args)
        Object.defineProperty(this, name, {
            get: function(){
                return this.getSignal(name)
            }
        })
    }

    getStatement(name){
        if(this.$properties[name]){
            return this.$properties[name]
        } else if(this.existDefaultProperty(name)){
            return this.getProperty(name)
        } else if(this.$signals[name]){
            return this.$signals[name]
        } else if(this.existDefaultSignal(name)){
            return this.getSignal(name)
        }
    }

    getProperty(name){
        if(this.$properties[name]){
            return this.$properties[name]
        } else {
            return this.createDefaultProperty(name)
        }
    }

    getPropertyValue(name){
        if(this.$properties[name]){
            return this.$properties[name].get()
        } else {
            return this.getPropertyDefaultValue(name)
        }
    }

    getPropertyDefaultValue(name){
        let current = this.__proto__.constructor
        while(current.defaultProperties){
            if(current.defaultProperties[name]) return current.defaultProperties[name].value
            current = current.__proto__
        }
        
    }

    existDefaultProperty(name){
        let current = this.__proto__.constructor
        while(current.defaultProperties){
            if(current.defaultProperties[name]) return true
            current = current.__proto__
        }
        
    }

    createDefaultProperty(name){
        let current = this.__proto__.constructor
        while(current.defaultProperties){
            if(current.defaultProperties[name]) {
                this.$properties[name] = new current.defaultProperties[name].type(current.defaultProperties[name].value)
                if(this[current.defaultProperties[name].changed]) {
                    this.$properties[name].getNotify().connect(this, this[current.defaultProperties[name].changed])
                }
                return this.$properties[name]
            }
            current = current.__proto__
        }
    }

    getSignal(name){
        if(this.$signals[name]){
            return this.$signals[name]
        } else {
            return this.createDefaultSignal(name)
        }
    }

    existDefaultSignal(name){
        let current = this.__proto__.constructor
        while(current.defaultSignals){
            if(current.defaultSignals[name]) return true
            current = current.__proto__
        }
        
    }

    createDefaultSignal(name){
        let current = this.__proto__.constructor
        while(current.defaultSignals){
            if(current.defaultSignals[name]) {
                this.$signals[name] = new QSignal(current.defaultSignals[name].params)
                return this.$signals[name]
            }
            current = current.__proto__
        }
    }
}

module.exports.ComplexObject = ComplexObject