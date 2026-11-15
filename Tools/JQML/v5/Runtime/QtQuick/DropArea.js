const { defineType } = require('../Core/type')
const { Item } = require('./Item')

// ---- DropArea: properties and signals; there is no drag and drop yet ---------------------------

class DropArea extends Item {}
defineType(DropArea, {
    props: { keys: 'var', containsDrag: { kind: 'bool', readonly: true }, drag: 'var' },
    signals: ['entered(drag)', 'exited', 'positionChanged(drag)', 'dropped(drop)'],
})

module.exports = { DropArea }
