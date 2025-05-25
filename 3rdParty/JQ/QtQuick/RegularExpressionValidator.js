const QtObject = require("../QtQml/QtObject")
const Var = require("../QtQml/Var")
const Signal = require("../QtQml/Signal")

class RegularExpressionValidator extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        regularExpression: { type: Var, value: undefined, signalName: 'regularExpressionChanged' },

        regularExpressionChanged: { type:Signal, slotName:'onRegularExpressionChanged', args:[] },
    })

    $makeRegExp(regExp){
        delete this.$matchRegex
        delete this.$partialMatchRegex

        if(regExp){
            this.$matchRegex = regExp
            this.$partialMatchRegex = this.$matchRegex.toPartialMatchRegex()
        }
    }

    $regularExpressionChanged(){
        this.$makeRegExp(this.getPropertyValue('regularExpression'))
    }

    $regExpChanged(){
        this.$makeRegExp(this.getPropertyValue('regExp'))
    }

    validate(str){
        if(this.$matchRegex){
            let result = this.$matchRegex.exec(str)
            return result ? result[0] === str ? true : false : false
        } else {
            return true
        }
    }

    hasPartialMatch(str){
        if(!str) return true

        if(this.$partialMatchRegex){
            let result = this.$partialMatchRegex.exec(str)
            return result ? result[0] === str ? true : false : false
        } else {
            return true
        }
    }
}



module.exports = RegularExpressionValidator