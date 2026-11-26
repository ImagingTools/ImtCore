const { defineType } = require('../Core/type')
const { PropertyAnimation } = require('./PropertyAnimation')

class ColorAnimation extends PropertyAnimation {
    $interpolatorKind(){ return 'color' }
}
defineType(ColorAnimation)

module.exports = { ColorAnimation }
