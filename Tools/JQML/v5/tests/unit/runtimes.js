// The runtimes the unit tests run on: the sources (Runtime/) and the built runtime the bundles take (dist/core.js)
const fs = require('fs')
const path = require('path')
const vm = require('vm')

function sources(){
    return {
        engine: require('../../Runtime/Core/engine'),
        Item: require('../../Runtime/QtQuick/Item').Item,
        MouseArea: require('../../Runtime/QtQuick/MouseArea').MouseArea,
        TextInput: require('../../Runtime/QtQuick/TextInput').TextInput,
        TextEdit: require('../../Runtime/QtQuick/TextEdit').TextEdit,
        delivery: require('../../Runtime/QtQuick/PointerDelivery'),
    }
}

function built(){
    let context = { console, setTimeout, clearTimeout, queueMicrotask }
    context.window = context
    vm.createContext(context)
    vm.runInContext(fs.readFileSync(path.resolve(__dirname, '../../dist/core.js'), 'utf8'), context)
    let JQ = context.JQ
    return { engine: JQ.engine, Item: JQ.Item, MouseArea: JQ.MouseArea, TextInput: JQ.types.TextInput, TextEdit: JQ.types.TextEdit, delivery: JQ.mouse }
}

const runtimes = [['Runtime', sources], ['dist/core.js', built]]

module.exports = { runtimes }
