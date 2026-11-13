const { defineType } = require('../Core/type')
const { PropertyAnimation } = require('./PropertyAnimation')

class NumberAnimation extends PropertyAnimation {
    $interpolatorKind(){ return 'real' }
}
defineType(NumberAnimation)

module.exports = { NumberAnimation }
