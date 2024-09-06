const Property = require("./Property")

class SpecialProperty extends Property {
    __reset(value){
        this.__subscribersReset()
        if(this.__frozen) return
        this.__set(undefined, value)
    }
}

module.exports = SpecialProperty