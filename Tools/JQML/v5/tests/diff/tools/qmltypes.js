// Reads the plugins.qmltypes of a Qt module: its components (C++ name, the QML names it exports, its prototype, its
// properties, signals and methods). The diff cases of the controls list the API of each type from it.
//   node tests/diff/tools/qmltypes.js <qml dir of Qt> <module dir> [QmlName...]
const fs = require('fs')
const path = require('path')

function parse(file){
    let text = fs.readFileSync(file, 'utf8')
    let components = new Map()
    for(let block of text.split(/\n    Component \{/).slice(1)){
        let name = (/\bname: "([^"]+)"/.exec(block) || [])[1]
        if(!name) continue
        let prototype = (/\bprototype: "([^"]+)"/.exec(block) || [])[1] || null
        let exports = [...block.matchAll(/"([\w.]+)\/(\w+) [\d.]+"/g)].map(m => m[2])
        let props = []
        for(let m of block.matchAll(/\n        Property \{([^}]*)\}/g)){
            let body = m[1]
            props.push({
                name: (/name: "(\w+)"/.exec(body) || [])[1],
                type: (/type: "([^"]+)"/.exec(body) || [])[1],
                readonly: /isReadonly: true/.test(body),
                pointer: /isPointer: true/.test(body),
                list: /isList: true/.test(body),
            })
        }
        let signals = [...block.matchAll(/\n        Signal \{\s*name: "(\w+)"/g)].map(m => m[1])
        let methods = [...block.matchAll(/\n        Method \{\s*name: "(\w+)"/g)].map(m => m[1])
        components.set(name, { name, prototype, exports, props, signals, methods })
    }
    return components
}

// the properties of a QML type and of its prototypes up to (without) stop, the base class first
function api(components, qmlName, stop = 'QQuickItem'){
    let component = [...components.values()].find(c => c.exports.includes(qmlName))
    let chain = []
    for(let c = component; c && c.name !== stop; c = components.get(c.prototype)) chain.unshift(c)
    return {
        props: chain.flatMap(c => c.props),
        signals: chain.flatMap(c => c.signals),
        methods: chain.flatMap(c => c.methods),
    }
}

module.exports = { parse, api }

if(require.main === module){
    let [qml, module, ...names] = process.argv.slice(2)
    let components = parse(path.join(qml, module, 'plugins.qmltypes'))
    for(let name of names) console.log(name, JSON.stringify(api(components, name)))
}
