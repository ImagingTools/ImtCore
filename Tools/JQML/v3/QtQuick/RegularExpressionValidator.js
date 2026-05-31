const QtObject = require("../QtQml/QtObject")
const Var = require("../QtQml/Var")
const Signal = require("../QtQml/Signal")

class RegularExpressionValidator extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        regularExpression: { type: Var, value: undefined},

        regularExpressionChanged: { type:Signal, args:[] },
    })

    SLOT_regularExpressionChanged(oldValue, newValue){
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



module.exports = RegularExpressionValidator