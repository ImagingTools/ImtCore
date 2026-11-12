const { defineType } = require('../Core/type')
const { NumberAnimation } = require('./NumberAnimation')

// Animators run on the render thread in Qt; here they animate their fixed property
function animator(name, property){
    let Class = { [name]: class extends NumberAnimation {} }[name]
    defineType(Class)
    Class.prototype.$properties = function(){ return [property] }
    return Class
}
const XAnimator = animator('XAnimator', 'x')
const YAnimator = animator('YAnimator', 'y')
const ScaleAnimator = animator('ScaleAnimator', 'scale')
const RotationAnimator = animator('RotationAnimator', 'rotation')
const OpacityAnimator = animator('OpacityAnimator', 'opacity')

module.exports = { XAnimator, YAnimator, ScaleAnimator, RotationAnimator, OpacityAnimator }
