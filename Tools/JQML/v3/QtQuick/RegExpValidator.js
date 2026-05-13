const QtObject = require("../QtQml/QtObject")
const Var = require("../QtQml/Var")
const Signal = require("../QtQml/Signal")

class RegExpValidator extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        regExp: { type: Var, value: null},

        regExpChanged: { type:Signal, args:[] },
    })

    SLOT_regExpChanged(oldValue, newValue){
        delete this.__matchRegex
        delete this.__partialMatchRegex

        if(newValue){
            this.__matchRegex = newValue
            this.__partialMatchRegex = this.__matchRegex.toPartialMatchRegex()
        }
    }

    validate(str){
        if(this.__matchRegex){
            let result = this.__matchRegex.exec(str)
            return result ? result[0] === str ? true : false : false
        } else {
            return true
        }
    }

    hasPartialMatch(str){
        if(!str) return true

        if(this.__partialMatchRegex){
            let result = this.__partialMatchRegex.exec(str)
            return result ? result[0] === str ? true : false : false
        } else {
            return true
        }
    }
}



module.exports = RegExpValidator