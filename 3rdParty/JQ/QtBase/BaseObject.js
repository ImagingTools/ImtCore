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

    __destroy(){}

    destroy(){
        this.__destroying = true
        this.__destroy()
    }

}

module.exports = BaseObject