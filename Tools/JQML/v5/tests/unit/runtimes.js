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
        DropArea: require('../../Runtime/index').types.DropArea,
        types: require('../../Runtime/index').types,
        keys: require('../../Runtime/QtQuick/KeyDelivery'),
        keyEvent: require('../../Runtime/QtQuick/KeyEvent'),
        shortcut: require('../../Runtime/QtQuick/Shortcut'),
        flickable: require('../../Runtime/QtQuick/Flickable'),
        translations: require('../../Runtime/Core/Translations'),
        textLayout: require('../../Runtime/QtQuick/TextLayout'),
        Qt: require('../../Runtime/index').Qt,
        jq: require('../../Runtime/index'),
    }
}

function built(){
    let context = { console, setTimeout, clearTimeout, queueMicrotask }
    context.window = context
    // the tests give the clipboard as globalThis.navigator
    Object.defineProperty(context, 'navigator', { get: () => globalThis.navigator, configurable: true })
    // the tests give the browser's Image loader as globalThis.Image
    Object.defineProperty(context, 'Image', { get: () => globalThis.Image, configurable: true })
    vm.createContext(context)
    vm.runInContext(fs.readFileSync(path.resolve(__dirname, '../../dist/core.js'), 'utf8'), context)
    let JQ = context.JQ
    return { engine: JQ.engine, Item: JQ.Item, MouseArea: JQ.MouseArea, TextInput: JQ.types.TextInput, TextEdit: JQ.types.TextEdit, delivery: JQ.mouse, DropArea: JQ.types.DropArea, types: JQ.types, keys: JQ.keys, keyEvent: JQ.keyEvent, shortcut: JQ.shortcut, flickable: JQ.flickable, translations: JQ.translations, textLayout: JQ.textLayout, Qt: JQ.Qt, jq: JQ }
}

const runtimes = [['Runtime', sources], ['dist/core.js', built]]

module.exports = { runtimes }
