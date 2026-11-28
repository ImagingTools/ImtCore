const { Qt } = require('../Qt/Qt')
const { setComponent } = require('../Core/color')

function aliasValue(value, kind){
    if(value !== undefined && value !== null) return value
    switch(kind){
        case 'color': return 'transparent'
        case 'font': return Qt.font({})
        case 'point': return Qt.point()
        case 'size': return Qt.size()
        case 'rect': return Qt.rect()
        case 'vector2d': return Qt.vector2d()
        case 'vector3d': return Qt.vector3d()
        case 'vector4d': return Qt.vector4d()
        case 'quaternion': return Qt.quaternion()
        case 'matrix4x4': return Qt.matrix4x4()
        default: throw new Error('Unsupported alias value type: ' + kind)
    }
}

function resolveAliasTarget(target, name){
    const aliases = target.constructor?.$aliases
    const resolve = aliases && Object.getOwnPropertyDescriptor(aliases, name)?.value
    return typeof resolve === 'function' ? resolve(target) : null
}

function withAliasField(value, field, next){
    if(value[field] === next) return value
    let color = typeof value === 'string' ? setComponent(value, field, next) : null
    if(color !== null) return color
    return Object.assign(Object.create(Object.getPrototypeOf(value)), value, { [field]: next })
}

function bindAliasField(target, id, field, kind, fn, scope, owner){
    // Read the current value after evaluating the expression, without tracking the property being written.
    return target.$bindProperty(id, (self, s) => {
        const next = fn.call(self, self, s)
        return withAliasField(aliasValue(target.$v[id], kind), field, next)
    }, scope, owner, field)
}

module.exports = { aliasValue, withAliasField, bindAliasField, resolveAliasTarget }
