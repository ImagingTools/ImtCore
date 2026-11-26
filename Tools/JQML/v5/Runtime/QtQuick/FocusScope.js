const { defineType } = require('../Core/type')
const { Item } = require('./Item')

class FocusScope extends Item {}
defineType(FocusScope)
FocusScope.prototype.$isFocusScope = true

module.exports = { FocusScope }
