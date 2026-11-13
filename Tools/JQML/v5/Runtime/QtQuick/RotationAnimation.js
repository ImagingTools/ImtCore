const { defineType } = require('../Core/type')
const { NumberAnimation } = require('./NumberAnimation')

// RotationAnimation: numerical direction only
class RotationAnimation extends NumberAnimation {}
defineType(RotationAnimation, { props: { direction: 'int' } })
Object.assign(RotationAnimation, { Numerical: 0, Clockwise: 1, Counterclockwise: 2, Shortest: 3 })

module.exports = { RotationAnimation }
