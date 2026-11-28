const { Qt } = require('../Qt/Qt')
const { setComponent } = require('../Core/color')

function aliasValue(value, kind){
    if(value !== undefined && value !== null) return value
    if(kind === 'color') return 'transparent'
    return kind === 'font' ? Qt.font({}) : Qt[kind]()
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

module.exports = { aliasValue, withAliasField, bindAliasField }
