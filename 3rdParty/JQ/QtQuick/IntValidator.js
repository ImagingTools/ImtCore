const QtObject = require("../QtQml/QtObject")
const String = require("../QtQml/String")
const Int = require("../QtQml/Int")
const Signal = require("../QtQml/Signal")

class IntValidator extends QtObject {
    static meta = Object.assign({}, QtObject.meta, {
        bottom: { type: Int, value: -2147483647, signalName: 'bottomChanged' },
        top: { type: Int, value: 2147483647, signalName: 'topChanged' },
        locale: { type: String, value: false, signalName: 'localeChanged' },

        bottomChanged: { type:Signal, slotName:'onBottomChanged', args:[] },
        topChanged: { type:Signal, slotName:'onTopChanged', args:[] },
        localeChanged: { type:Signal, slotName:'onLocaleChanged', args:[] },
    })

    validate(str) {
        if(!str) return false

        let value = Number(str)
        if(isNaN(value)) return false

        return (this.getPropertyValue('bottom') <= value && this.getPropertyValue('top') >= value)
    }

    hasPartialMatch(str){
        if(!str) return true

        if(this.getPropertyValue('bottom') < 0 || this.getPropertyValue('top') < 0){
            if(str === '-') return true
        }
        let value = Number(str)
        if(isNaN(value)) return false

        return (this.getPropertyValue('bottom') <= value && this.getPropertyValue('top') >= value) || (str.length - (this.getPropertyValue('top')-1).toString().length === 1)
    }
}



module.exports = IntValidator