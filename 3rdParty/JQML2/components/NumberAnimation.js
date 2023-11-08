const { PropertyAnimation } = require('./PropertyAnimation')
const { QReal } = require('../utils/properties')

class NumberAnimation extends PropertyAnimation {

    static defaultProperties = {
        to: { type: QReal },
        from: { type: QReal },
    }


}

module.exports.NumberAnimation = NumberAnimation