class BaseObject {
    static create(){
        return new this()
    }

    static isAssignableFrom(cls){
        let prototype = this
        while(prototype){
            if(cls === prototype) return true

            prototype = prototype.__proto__
        }
        return false
    }

    static getDefaultValue(){
        return null
    }

    static cachedComponents = {}

    static cachedComponent(id, comp){
        if(this.cachedComponents[id]) return this.cachedComponents[id]

        this.cachedComponents[id] = comp
        return this.cachedComponents[id]
    }

    __destroy(){}

    destroy(){
        this.__destroying = true
        this.__destroy()
    }

}

module.exports = BaseObject