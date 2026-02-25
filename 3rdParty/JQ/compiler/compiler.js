const fs = require('fs')
const os = require('os')
const path = require('path')
const parser = require('./parser')
const { SourceMapGenerator, SourceNode } = require('source-map-generator')
const UglifyJS = require("uglify-js")
const { Command, Option } = require('commander')

// for compatibility with web
global.window = {
    addEventListener: function () { }
    
}

const program = new Command()

program.addOption(new Option('-c, --config <path>', 'config file path'))
    .addOption(new Option('-o, --output <path>', 'output dir path'))
    .addOption(new Option('-e, --entry <path>', 'entry qml file path'))
    .addOption(new Option('-i, --icon <path>', 'icon file path for compile mode html'))
    .addOption(new Option('-n, --name <string>', 'application name').default(''))
    .addOption(new Option('-r, --root <string>', 'root path').default('/'))
    .addOption(new Option('-m, --mode <string>', 'compile mode').choices(['js', 'html']))

program.parse(process.argv)

const options = program.opts()


if(options.mode === 'html'){
    
    let icon = `<link rel="${options.name} icon" type="image" href="${options.icon}">`
    let html = `
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>${options.name}</title>
        ${'icon' in options ? icon : ''}
        <script src="${options.name}.js"></script>
    </head>
    <body>
        
    </body>
    </html>
    `

    fs.writeFileSync(options.output + `/${options.name}.html`, html)
    return
}


const BaseObject = require('../QtBase/BaseObject')
const Qt = require('../Qt/Qt')
const QtQml = require('../QtQml/QtQml')
const QtQuick = require('../QtQuick/QtQuick')
const Qt5Compat = require('../Qt5Compat/Qt5Compat')
const QtWebSockets = require('../QtWebSockets/QtWebSockets')
const QtPositioning = require('../QtPositioning/QtPositioning')
const QtLocation = require('../QtLocation/QtLocation')

const env = process.env

// const configFilePath = 'C:\\Users\\Artur\\Documents\\ImagingTools\\ItDevelopment\\ImtCore\\Qml\\web\\imtcore.json'//process.argv.slice(2)[0]
// const configFilePath = 'C:\\Users\\Artur\\Documents\\ImagingTools\\ItDevelopment\\Lisa\\Include\\lisaqml\\lisa.json'//process.argv.slice(2)[0]
// const configFilePath = 'C:\\Users\\Artur\\Documents\\ImagingTools\\ItDevelopment\\ImtCore\\3rdParty\\JQ\\tests\\agentino.json'//process.argv.slice(2)[0]
// const configFilePath = 'C:\\Users\\Artur\\Documents\\ImagingTools\\ItDevelopment\\ImtCore\\3rdParty\\JQ\\test\\jq.json'
// const configFilePath = 'C:\\Users\\Artur\\Documents\\ImagingTools\\ItDevelopment\\ImtCore\\3rdParty\\JQ\\tests\\Rosa\\rosa.json'

// options.config = "D:/IT/IotPlatform/Build/CMake/../../Include/iotqml/Qml/iot.json"
// options.name = "index"
// options.output = "D:/IotPlatform/Bin/web/Resources/"
// options.root = "/Iot/Views/"
// options.entry = "D:/IT/IotPlatform/Build/CMake/../../Include/iotqml/Qml/IotPlatformWeb.qml"
// options.mode = "js"

// options.config = "d:/TEMP/test.json"
// options.name = "index"
// options.output = "d:/TEMP"
// options.root = "/"
// options.entry = "d:/TEMP/main.qml"
// options.mode = "js"


const configFilePath = path.normalize(options.config.trim())
const configDirPath = configFilePath.split(/[\\\/]+/g).slice(0, -1).join('/')

function envFill(source) {
    let result = source
    for (let key in env) {
        result = result.replaceAll('${' + key + '}', env[key].replaceAll('\\', '\\\\').trim())
    }
    return result
}

function includeFiles(sourceFile) {
    if (sourceFile.includes)
        for (let filePath of sourceFile.includes) {
            let file = JSON.parse(envFill(fs.readFileSync(filePath, { encoding: 'utf8', flag: 'r' })))
            includeFiles(file)
            for (let dirPath of file.dirs) {
                sourceFile.dirs.unshift(dirPath)
            }
        }
}

const config = JSON.parse(envFill(fs.readFileSync(configFilePath, { encoding: 'utf8', flag: 'r' })))
includeFiles(config)

const BaseModules = {
    Qt,
    QtQml,
    QtQuick,
    Qt5Compat,
    QtWebSockets,
    QtPositioning,
    QtLocation,
}

const JQModules = {
    ...BaseModules
}

const SingleFiles = {

}

const Singletons = {

}

const Enums = {

}

class Instruction {
    defineProperties = []
    defineSignals = []
    defineMethods = []
    connectedSignals = []
    assignProperties = []
    objectProperties = []

    id = ''
    qmlFile = null
    className = ''
    extends = null
    parent = null

    children = []
    name = ''
    isRoot = false

    constructor(parent, className, _extends, meta, on, qmlFile, info, targetContext) {
        if (qmlFile.elementsCount === 0) this.isRoot = true
        this.name = '__el' + qmlFile.elementsCount++
        this.parent = parent
        this.className = className
        this.extends = typeof _extends === 'object' ? _extends.slice(1).join('.') : _extends
        this.qmlFile = qmlFile
        this.info = info
        this.targetContext = targetContext

        for (let m of meta) {
            if (m) this[m[0]](m)
        }
    }

    qmlelem(meta) {
        this.children.push(new Instruction(this, '', meta[1], meta[3], meta[2], this.qmlFile, meta.info, this))
    }
    qmlsignaldef(meta) {
        let args = []
        for (let arg of meta[2]) {
            args.push(`\`${arg.name}\``)
        }
        this.defineSignals.push({
            name: meta[1],
            slotName: 'on' + meta[1][0].toUpperCase() + meta[1].slice(1),
            args: args
        })
    }
    qmlmethod(meta) {
        this.defineMethods.push({
            name: meta[1],
            args: meta[2][2],
            source: meta[2]
        })
    }
    qmlaliasdef(meta) {
        this.defineProperties.push({
            name: meta[1],
            type: 'alias',
            signalName: meta[1] + 'Changed',
        })
        this.assignProperties.push({
            name: meta[1],
            type: 'alias',
            value: meta,
        })
        this.defineSignals.push({
            name: meta[1] + 'Changed',
            slotName: 'on' + meta[1][0].toUpperCase() + meta[1].slice(1) + 'Changed',
            args: []
        })
    }
    qmlpropdef(meta) {
        let type = {
            defaultValue: null,
            getDefaultValue(){
                return null
            }
        }
        try {
            let typeInfo = this.getTypeInfo(meta[3])
            type = typeInfo.type
        } catch {

        }

        if (meta[4] && meta[4][1][0] === 'qmlelem') {
            this.defineSignals.push({
                name: meta[2] + 'Changed',
                slotName: 'on' + meta[2][0].toUpperCase() + meta[2].slice(1) + 'Changed',
                args: []
            })

            this.defineProperties.push({
                name: meta[2],
                type: meta[3],
                value: type.defaultValue,
                signalName: meta[2] + 'Changed',
                info: meta.info,
            })

            this.assignProperties.push({
                name: meta[2],
                value: new Instruction(null, '', meta[4][1][1], meta[4][1][3], meta[4][1][2], this.qmlFile, meta[4][1].info, this),
            })
        } else {
            let defaultValue = type.getDefaultValue()

            if (meta[5]) {
                try {
                    defaultValue = eval(meta[5])
                    if (defaultValue && typeof defaultValue === 'object') {
                        this.assignProperties.push({
                            name: meta[2],
                            value: meta[4],
                        })
                    }
                } catch {
                    this.assignProperties.push({
                        name: meta[2],
                        value: meta[4],
                    })
                }
            }

            this.defineSignals.push({
                name: meta[2] + 'Changed',
                slotName: 'on' + meta[2][0].toUpperCase() + meta[2].slice(1) + 'Changed',
                args: []
            })

            this.defineProperties.push({
                name: meta[2],
                type: meta[3],
                value: defaultValue,
                signalName: meta[2] + 'Changed',
                info: meta.info,
            })
        }

    }
    qmlprop(meta) {
        if (meta[2][0] === "block" || meta[2][1][0] === "assign") {
            if (meta[1][0] === "dot") {
                let name = meta[1].slice(1)

                if (name[1].slice(0, 2) === 'on') {
                    this.connectedSignals.push({
                        slotName: `['${name.join('.')}']`,
                        args: [],
                        source: meta[2],
                    })
                } else {
                    if (meta[2][1][0] === 'qmlelem') {
                        this.assignProperties.push({
                            name: name.join('.'),
                            value: new Instruction(null, '', meta[2][1][1], meta[2][1][3], meta[2][1][2], this.qmlFile, meta[2][1].info, this),
                        })
                    } else {
                        this.assignProperties.push({
                            name: name.join('.'),
                            value: meta[2],
                        })
                    }
                }
            } else {
                let name = meta[1]

                if (name.slice(0, 2) === 'on') {
                    let signalName = name.slice(2)
                    signalName = signalName[0].toLowerCase() + signalName.slice(1)
                    this.connectedSignals.push({
                        slotName: name,
                        args: [], // Нужно отследить количество аргументов
                        source: meta[2],
                    })
                } else {
                    if (meta[2][1][0] === 'qmlelem') {
                        this.assignProperties.push({
                            name: name.join('.'),
                            value: new Instruction(null, '', meta[2][1][1], meta[2][1][3], meta[2][1][2], this.qmlFile, meta[2][1].info, this),
                        })
                    } else {
                        this.assignProperties.push({
                            name: name,
                            value: meta[2],
                        })
                    }
                }
            }
        } else if (meta[1][0] === "dot") {
            let name = meta[1].slice(1).join('.')
            this.assignProperties.push({
                name: name,
                value: meta[2],
            })
        } else {
            let name = meta[1]
            if (name.slice(0, 2) === 'on') {
                let signalName = name.slice(2)
                signalName = signalName[0].toLowerCase() + signalName.slice(1)
                this.connectedSignals.push({
                    slotName: name,
                    args: [], // Нужно отследить количество аргументов
                    source: meta[2],
                })
            } else {
                if (name === 'id') {
                    this.id = meta[2][1][1]
                    this.qmlFile.context[this.id] = this
                } else {
                    if (meta[2][1][0] === 'qmlelem') {
                        //console.log('=====', name)
                        this.assignProperties.push({
                            name: name,//.join('.'),
                            value: new Instruction(null, '', meta[2][1][1], meta[2][1][3], meta[2][1][2], this.qmlFile, meta[2][1].info, this),
                        })
                    } else {
                        // let command = ''
                        // if((name === 'latitude' || name === 'longitude') && !testProperty(name, instructions)){ // temp solution
                        //     command = 'create'
                        // }
                        this.assignProperties.push({
                            name: name,
                            value: meta[2],
                        })
                    }
                }
            }
        }
    }
    qmlobj(meta) {
        for (let m of meta[2]) {
            if(m[0] === 'qmlprop'){
                m[1] = `${meta[1]}.${m[1]}`
                this[m[0]](m)
            }
        }
    }
    qmlenumdef(meta) {
        Enums[meta[1]] = meta[2]
    }

    resolve(name, thisKey) {
        let res = this.resolveInner(name, thisKey)
        if (res) {
            return res
        } else {
            let recursive = (result = [], name, target) => {
                for (let key in target) {
                    result.push(key)
                    if (name === key) return result.join('.')

                    if ((typeof target[key] === 'object' && typeof target[key] !== 'function' && !(target[key] instanceof QmlFile || target[key] instanceof JSFile || target[key] === QtQml.Screen))) {
                        if (recursive(result, name, target[key])) {
                            // console.log(target[key][name])
                            if (target[key][name] instanceof JSFile || target[key][name] instanceof QmlFile) {
                                return {
                                    obj: target[key][name],
                                    source: result.join('.'),
                                }
                            } else {
                                let type = eval(result.join('.'))
                                // console.log(result.join('.'), name)
                                return {
                                    obj: type,
                                    source: result.join('.'),
                                }
                            }

                        }
                    }

                    result.pop()
                }
            }
            return recursive(['JQModules'], name, JQModules)
        }
    }

    resolveInner(name, thisKey) {
        for (let obj of this.defineProperties) {
            let typeInfo
            try {
                typeInfo = obj.type ? this.getTypeInfo(obj.type) : {}
            } catch (error) {
                throw `${this.qmlFile.fileName}:${obj.info.line + 1}:${obj.info.col + 1}: error: ${obj.type} is not founded`
            }
            if (name === obj.name) return {
                source: `${thisKey}.${name}`,
                type: typeInfo.type,
            }
        }
        for (let obj of this.defineSignals) {
            if (name === obj.name) return {
                source: `${thisKey}.${name}`,
            }
        }
        for (let obj of this.defineMethods) {
            if (name === obj.name) return {
                source: `${thisKey}.${name}`,
            }
        }

        let typeInfo = null
        try {
            typeInfo = this.getTypeInfo(this.extends)
        } catch { }

        if (typeInfo) {
            if (typeInfo.type instanceof QmlFile) {
                return typeInfo.type.instruction.resolve(name, thisKey)
            } else {
                let obj = new typeInfo.type()
                if (name in typeInfo.type.meta) {
                    return {
                        source: `${thisKey}.${name}`,
                        type: typeInfo.type.meta[name].typeTarget ? typeInfo.type.meta[name].typeTarget : typeInfo.type.meta[name].type,
                    }
                } else if (name in obj) {
                    return {
                        source: `${thisKey}.${name}`,
                    }
                }
            }
        }
    }

    prepare(tree, stat = { isCompute: false, thisKey: '__self', value: new SourceNode(), local: [] }, endSymbol = true) {
        if (tree) {
            switch (tree[0]) {
                case 'return': {
                    stat.isCompute = true
                    stat.value.add('return ')
                    this.prepare(tree[1], stat)
                    stat.value.add(';')
                    return stat
                }
                case 'num': {
                    stat.value.add(tree[1].toString())
                    return stat
                }
                case 'string': {
                    stat.value.add(`\`${tree[1].replaceAll('\u005C', '\u005C\u005C')}\``)
                    return stat
                }
                case 'assign': {
                    this.prepare(tree[2], stat)
                    if (tree[1] === true) {
                        stat.value.add(`=`)
                    } else {
                        if (tree[1]) stat.value.add(tree[1])
                        stat.value.add(`=`)
                    }
                    this.prepare(tree[3], stat)

                    if(!stat.isMissingEnd) stat.value.add(`;`)
                    
                    return stat
                }
                case 'name': {
                    for (let local of stat.local) {
                        if (local.indexOf(tree[1]) >= 0) {
                            stat.value.add(new SourceNode(tree.info.line+1,tree.info.col,this.qmlFile.fileName,tree[1]))
                            if (stat.wasDot) {
                                stat.dotObj = null
                            }
                            return stat
                        }
                    }

                    if (this.qmlFile.context[tree[1]]) {  // context !!!
                        stat.isCompute = true
                        // stat.value.add()
                        stat.value.add(new SourceNode(tree.info.line+1,tree.info.col,this.qmlFile.fileName,[`${stat.thisKey}.__${this.qmlFile.getContextName()}.`, tree[1]]))
                        stat.dotObj = this.qmlFile.context[tree[1]]

                        return stat
                    }

                    if (Singletons[tree[1]]) {
                        // this.qmlFile.dependencies.add(Singletons[tree[1]])
                        if (Singletons[tree[1]].moduleName) {
                            stat.value.add(new SourceNode(tree.info.line+1,tree.info.col,this.qmlFile.fileName,[`JQModules.${Singletons[tree[1]].moduleName}.`, tree[1]]))
                        } else {
                            stat.value.add(new SourceNode(tree.info.line+1,tree.info.col,this.qmlFile.fileName,tree[1]))
                        }

                        if (stat.wasDot) {
                            stat.dotObj = Singletons[tree[1]]
                        }
                        return stat
                    }

                    if (tree[1] === 'parent') {
                        if (stat.wasDot) {
                            if (!this.parent || this.parent.extends === 'Component') {
                                stat.dotObj = null
                            } else {
                                stat.dotObj = this.parent
                            }

                        }
                        stat.isCompute = true
                        stat.value.add(new SourceNode(tree.info.line+1,tree.info.col,this.qmlFile.fileName,[stat.thisKey, '.',tree[1]]))
                        return stat
                    } else if (tree[1] === 'Qt') {
                        stat.isCompute = true
                        stat.value.add(new SourceNode(tree.info.line+1,tree.info.col,this.qmlFile.fileName,'JQModules.Qt'))
                        return stat
                    } else if (tree[1] === 'QtPositioning') {
                        stat.isCompute = true
                        stat.value.add(new SourceNode(tree.info.line+1,tree.info.col,this.qmlFile.fileName,'JQModules.QtPositioning'))
                        return stat
                    }

                    try {
                        let obj = eval(tree[1])
                        stat.value.add(new SourceNode(tree.info.line+1,tree.info.col,this.qmlFile.fileName,tree[1]))
                        stat.dotObj = (typeof obj === 'object' || typeof obj === 'function') ? obj : null
                        return stat
                    } catch { }

                    let path = this.resolve(tree[1], stat.thisKey)

                    if (path) {
                        stat.isCompute = true
                        stat.dotObj = (typeof path.obj === 'object' || typeof path.obj === 'function') ? path.obj : null
                        stat.value.add(new SourceNode(tree.info.line+1,tree.info.col,this.qmlFile.fileName,path.source))
                    } else if (tree[1] === 'context') {
                        stat.value.add(new SourceNode(tree.info.line+1,tree.info.col,this.qmlFile.fileName,'JSContext'))
                    } else {
                        console.log(`${this.qmlFile.fileName}:${tree.info.line + 1}:${tree.info.col + 1}: warning: ${tree[1]} is not founded`)
                        stat.value.add(new SourceNode(tree.info.line+1,tree.info.col,this.qmlFile.fileName,[stat.thisKey, '.',tree[1]]))
                    }

                    return stat
                }
                case 'dot': {
                    stat.isCompute = true
                    stat.wasDot = true
                    this.prepare(tree[1], stat)
                    stat.value.add('.')
                    stat.value.add(new SourceNode(tree.info.line+1,tree.info.col,this.qmlFile.fileName,tree[2]))

                    if (stat.dotObj) {
                        let path = ''
                        if (stat.dotObj instanceof JSFile) {
                            if (stat.dotObj.meta.exports.indexOf(tree[2] >= 0)) {
                                path = {}
                            } else {
                                path = null
                            }
                            stat.dotObj = null
                        } else if (stat.dotObj instanceof QmlFile) {
                            path = stat.dotObj.instruction.resolve(tree[2])
                            if (path) stat.dotObj = (typeof path.obj === 'object' || typeof path.obj === 'function') ? path.obj : null
                        } else if (stat.dotObj instanceof Instruction) {
                            path = stat.dotObj.resolve(tree[2])
                            if (path) stat.dotObj = (typeof path.obj === 'object' || typeof path.obj === 'function') ? path.obj : null
                        } else if (typeof stat.dotObj === "function" && stat.dotObj.meta) {
                            let obj = new stat.dotObj()
                            if (tree[2] in obj || tree[2] in stat.dotObj) {
                                stat.dotObj = null
                                path = obj
                            } else if (tree[2] in stat.dotObj.meta) {
                                path = this.resolve(stat.dotObj.meta[tree[2]], stat.thisKey)
                                if (path) stat.dotObj = (typeof path.obj === 'object' || typeof path.obj === 'function') ? path.obj : null
                            }
                        } else if (tree[2] in stat.dotObj) {
                            path = stat.dotObj[tree[2]]
                            stat.dotObj = typeof stat.dotObj[tree[2]] === 'object' ? stat.dotObj[tree[2]] : null
                        } else {
                            path = null
                            stat.dotObj = null
                        }
                        if (path === undefined || path === null) {
                            console.log(`${this.qmlFile.fileName}:${tree.info.line + 1}:${tree.info.col + 1}: warning: ${tree[2]} is not founded`)
                        }
                    }
                    stat.wasDot = false
                    return stat
                }
                case 'call': {
                    this.prepare(tree[1], stat)
                    stat.value.add('(')
                    for (let i = 0; i < tree[2].length; i++) {
                        this.prepare(tree[2][i], stat)
                        if (i < tree[2].length - 1) stat.value.add(',')
                    }

                    if (tree[1][0] === 'dot' && tree[1][1][1] === 'Qt' && tree[1][2] === 'createComponent') {
                        // stat.value.add(',__currentModule')
                        stat.value.add(`,${this.qmlFile.moduleName ? "JQModules['" + this.qmlFile.moduleName + "']" : 'null'}`)
                    }

                    stat.value.add(')\n')

                    stat.dotObj = null

                    return stat
                }
                case 'conditional': {
                    stat.isCompute = true
                    stat.value.add('(')
                    this.prepare(tree[1], stat)
                    stat.value.add(' ? ')
                    this.prepare(tree[2], stat)
                    stat.value.add(' : ')
                    this.prepare(tree[3], stat)
                    stat.value.add(')')
                    return stat
                }
                case 'binary': {
                    stat.isCompute = true
                    stat.value.add(`(`)
                    this.prepare(tree[2], stat)
                    stat.value.add(` ${tree[1]} `)
                    this.prepare(tree[3], stat)
                    stat.value.add(`)`)
                    stat.value.add(`\n`)
                    return stat
                }
                case 'let': {
                    let name = tree[1][0][0]
                    let local = []
                    if (stat.local.length) {
                        local = stat.local[stat.local.length - 1]
                        local.push(name)
                    } else {
                        local.push(name)
                        stat.local.push(local)
                    }

                    stat.value.add(`let ${name}`)
                    if (tree[1][0][1]) {
                        stat.value.add(`=`)
                        this.prepare(tree[1][0][1], stat)
                    }
                    if(endSymbol) stat.value.add(`;`)

                    return stat
                }
                case 'var': {
                    let name = tree[1][0][0]
                    let local = []
                    if (stat.local.length) {
                        local = stat.local[stat.local.length - 1]
                        local.push(name)
                    } else {
                        local.push(name)
                        stat.local.push(local)
                    }

                    stat.value.add(`var ${name}`)
                    if (tree[1][0][1]) {
                        stat.value.add(`=`)
                        this.prepare(tree[1][0][1], stat)
                    }
                    if(endSymbol) stat.value.add(`;`)

                    return stat
                }
                case 'const': {
                    let name = tree[1][0][0]
                    let local = []
                    if (stat.local.length) {
                        local = stat.local[stat.local.length - 1]
                        local.push(name)
                    } else {
                        local.push(name)
                        stat.local.push(local)
                    }

                    stat.value.add(`const ${name}`)
                    if (tree[1][0][1]) {
                        stat.value.add(`=`)
                        this.prepare(tree[1][0][1], stat)
                    }
                    if(endSymbol) stat.value.add(`;`)

                    return stat
                }
                case 'unary-prefix': {
                    if (tree[1] === 'typeof' || tree[1] === 'delete') {
                        stat.value.add(tree[1] + ' ')
                    } else {
                        stat.value.add(tree[1])
                    }

                    this.prepare(tree[2], stat)

                    stat.value.add('\n')
                    return stat
                }
                case 'unary-postfix': {
                    this.prepare(tree[2], stat)
                    stat.value.add(tree[1])
                    stat.value.add('\n')
                    return stat
                }
                case 'if': {
                    stat.value.add('if(')
                    this.prepare(tree[1], stat)
                    stat.value.add('){')
                    this.prepare(tree[2], stat)
                    stat.value.add('}')
                    if (tree[3]) {
                        stat.value.add('else{')
                        this.prepare(tree[3], stat)
                        stat.value.add('}')
                    }

                    return stat
                }
                case 'while': {
                    stat.isCompute = true
                    stat.value.add(`while(`)
                    this.prepare(tree[1], stat)
                    stat.value.add(`){`)
                    this.prepare(tree[2], stat)
                    stat.value.add(`};`)
                    return stat
                }
                case 'for': {
                    stat.isCompute = true
                    stat.value.add(`for(`)
                    this.prepare(tree[1], stat)
                    this.prepare(tree[2], stat)
                    stat.value.add(`;`)
                    stat.isMissingEnd = true
                    this.prepare(tree[3], stat)
                    // if (stat.value[stat.value.length - 1] === ';') stat.value.pop()
                    stat.isMissingEnd = false
                    stat.value.add(`){`)
                    this.prepare(tree[4], stat)
                    stat.value.add(`};`)
                    return stat
                }
                case 'for-of': {
                    stat.isCompute = true
                    stat.value.add(`for(`)
                    this.prepare(tree[1], stat, false)
                    stat.value.add(` of `)
                    this.prepare(tree[3], stat)
                    stat.value.add(`){`)
                    this.prepare(tree[4], stat)
                    stat.value.add(`};`)
                    return stat
                }
                case 'for-in': {
                    stat.isCompute = true
                    stat.value.add(`for(`)
                    this.prepare(tree[1], stat, false)
                    stat.value.add(` in `)
                    this.prepare(tree[3], stat)
                    stat.value.add(`){`)
                    this.prepare(tree[4], stat)
                    stat.value.add(`};`)
                    return stat
                }
                case 'function': {
                    let local = []

                    stat.value.add(`function`)
                    if (tree[1]) {
                        if (stat.local.length) {
                            stat.local[stat.local.length - 1].push(tree[1])
                        } else {
                            stat.local.push([tree[1]])
                        }
                        stat.value.add(` ${tree[1]}`)
                    }

                    stat.local.push(local)

                    stat.value.add(`(`)
                    for (let i = 0; i < tree[2].length; i++) {
                        stat.value.add(tree[2][i])
                        local.push(tree[2][i])
                        if (i < tree[2].length - 1) stat.value.add(',')
                    }
                    stat.value.add(`){`)
                    this.prepare(tree[3], stat)
                    stat.value.add(`}`)

                    let index = stat.local.indexOf(local)
                    if (index >= 0) stat.local.splice(index, 1)

                    return stat
                }
                case 'defun': {
                    let local = []
                    stat.value.add([tree[1], '('])
                    for (let i = 0; i < tree[2].length; i++) {
                        stat.value.add(tree[2][i])
                        local.push(tree[2][i])
                        if (i < tree[2].length - 1) stat.value.add(',')
                    }

                    let ids = new Set()
                    let context = new Set()

                    for(let name of Object.keys(this.qmlFile.context)){
                        let found = false

                        if(tree[2].indexOf(name) >= 0){
                            found = true
                            break
                        }

                        for (let local of stat.local) {
                            if (local.indexOf(name) >= 0) {
                                found = true
                                break
                            }
                        }
                        if(!found){
                            ids.add(name)
                            context.add(`__self.__${this.qmlFile.getContextName()}.${name}`)
                        } 
                    }

                    stat.value.add(')')
                    stat.value.add(`{let __self=this;let [${Array.from(ids).join(',')}] = [${Array.from(context).join(',')}];try{JQApplication.beginUpdate();`)
                    stat.local.push(local)
                    this.prepare(tree[3], stat)
                    let index = stat.local.indexOf(local)
                    if (index >= 0) stat.local.splice(index, 1)
                    stat.value.add('}finally{JQApplication.endUpdate()}}')
                    return stat
                }
                case 'object': {

                    stat.value.add(`{`)
                    for (let d of tree[1]) {
                        stat.value.add(`${d[0]}:`)
                        this.prepare(d[1], stat)
                        stat.value.add(`,`)
                    }
                    stat.value.add(`}`)
                    return stat
                }
                case 'block': {
                    this.prepare(tree[1], stat)
                    return stat
                }
                case 'stat': {
                    this.prepare(tree[1], stat)
                    return stat
                }
                case 'new': {
                    stat.value.add(` new `)
                    this.prepare(tree[1], stat)
                    stat.value.add(`(`)
                    for (let i = 0; i < tree[2].length; i++) {
                        this.prepare(tree[2][i], stat)
                        if (i < tree[2].length - 1) stat.value.add(',')
                    }
                    stat.value.add(`)`)
                    return stat
                }
                case 'sub': {
                    this.prepare(tree[1], stat)
                    stat.value.add(`[`)
                    this.prepare(tree[2], stat)
                    stat.value.add(`]\n`)
                    return stat
                }
                case 'array': {
                    stat.value.add(`[`)
                    for (let i = 0; i < tree[1].length; i++) {
                        this.prepare(tree[1][i], stat)
                        if (i < tree[1].length - 1) stat.value.add(',')
                    }
                    stat.value.add(`]`)
                    return stat
                }
                case 'regexp': {
                    stat.value.add(`/${tree[1]}/${tree[2]}`)
                    return
                }
                case 'switch': {
                    stat.value.add(`switch(`)
                    this.prepare(tree[1], stat)
                    stat.value.add(`){`)
                    for (let i = 0; i < tree[2].length; i++) {
                        if (tree[2][i][0] === null) {
                            stat.value.add(`default: `)
                            this.prepare(tree[2][i][1], stat)
                        } else {
                            stat.value.add(`case `)
                            this.prepare(tree[2][i][0], stat)
                            stat.value.add(`:`)
                            this.prepare(tree[2][i][1], stat)
                            stat.value.add(`break;`)
                        }

                    }
                    stat.value.add(`}`)
                    return stat
                }
                case 'seq': {
                    stat.value.add(`(`)
                    for (let i = 1; i < tree.length; i++) {
                        this.prepare(tree[i], stat)
                        if (i < tree.length - 1) stat.value.add(',')
                    }
                    stat.value.add(`)`)
                    return stat
                }
                case 'try': {
                    stat.value.add(`try{`)
                    this.prepare(tree[1], stat)
                    if (tree[2][0]) {
                        let local = []
                        stat.ignore += tree[2][0]
                        stat.value.add(`}catch(${tree[2][0]}){`)
                        if (tree[2][0]) {
                            local.push(tree[2][0])
                            stat.local.push(local)
                        }
                        this.prepare(tree[2][1], stat)
                        let index = stat.local.indexOf(local)
                        if (index >= 0) stat.local.splice(index, 1)
                    } else {
                        stat.value.add(`}catch{`)
                        this.prepare(tree[2][1], stat)
                    }
                    stat.value.add(`}`)
                    if (tree[3] && tree[3].length) {
                        stat.value.add(`finally{`)
                        this.prepare(tree[3], stat)
                        stat.value.add(`}`)
                    }

                    return stat
                }
                case 'break': {
                    stat.value.add('break;')
                    return stat
                }
                case 'continue': {
                    stat.value.add('continue;')
                    return stat
                }
                case 'qmlaliasdef': {
                    stat.isCompute = true
                    let obj = null

                    if (this.qmlFile.context[tree.info[0].value]) {
                        stat.value.add(`${stat.thisKey}.__${this.qmlFile.getContextName()}.${tree.info[0].value}`)
                        obj = this.qmlFile.context[tree.info[0].value]
                    } else if (tree.info[0].value === 'parent') {
                        stat.value.add(stat.thisKey + '.' + tree.info[0].value)
                    }

                    if (obj) {
                        for (let i = 1; i < tree.info.length; i++) {
                            if (obj) {
                                let path = obj.resolve(tree.info[i].value, '')

                                if (path) {
                                    stat.value.add(path.source)

                                    obj = path.obj
                                } else {
                                    console.log(`${this.qmlFile.fileName}:${tree.info[i].line + 1}:${tree.info[i].col + 1}: warning: ${tree.info[i].value} is not founded in ${tree.info[i - 1].value}`)
                                }
                            } else {
                                stat.value.add('.' + tree.info[i].value)
                            }
                        }
                    } else {
                        console.log(`${this.qmlFile.fileName}:${tree.info[0].line + 1}:${tree.info[0].col + 1}: warning: ${tree.info[0].value} is not founded`)
                    }

                    return stat
                }
                default: {
                    for (let t of tree) {
                        this.prepare(t, stat)
                        stat.value.add('\n')
                    }
                    return stat
                }
            }
        }
    }

    getTypeInfo(type) {
        if (!type) type = this.extends

        let found = false
        let _type = type
        let _path = ''

        if (SingleFiles[type]) {
            found = true
            _path = type
            _type = SingleFiles[type]
        } else if (this.qmlFile.moduleName && JQModules[this.qmlFile.moduleName][type]) {
            found = true
            _path = `JQModules.${this.qmlFile.moduleName}.${type}`
            _type = JQModules[this.qmlFile.moduleName][type]
        } else if (JQModules.QtQml[type]) {
            found = true
            _path = `JQModules.QtQml.${type}`
            _type = JQModules.QtQml[type]
        } else if (JQModules.QtQml.Models[type]) {
            found = true
            _path = `JQModules.QtQml.Models.${type}`
            _type = JQModules.QtQml.Models[type]
        } else {
            for (let i = this.qmlFile.imports.length - 1; i >= 0; i--) {
                let path = this.qmlFile.imports[i].path
                let as = this.qmlFile.imports[i].as
                let version = this.qmlFile.imports[i].version
                if (as && type.indexOf(as) >= 0) {
                    _path = 'JQModules.' + type.replace(as, path)
                } else {
                    _path = 'JQModules.' + path + '.' + type
                }

                if (!found && version) {
                    try {
                        _type = eval(_path + '_v' + version)
                        if (_type) {
                            _path += '_v' + version
                            found = true
                            break
                        }
                    } catch { }
                }
                if (!found) {
                    let qmlFile = null
                    try {
                        qmlFile = eval(_path)
                    } catch { }

                    if (qmlFile) {
                        if (version !== undefined && qmlFile instanceof QmlFile) {
                            throw `${this.qmlFile.fileName}:${this.info.line + 1}:${this.info.col + 1}: error: ${type} version ${version} does not exists in ${path}`
                        } else {
                            _type = qmlFile
                            found = true
                            break
                        }
                    }
                }
            }
        }

        if (found) {
            let _typeBase = _type
            while (_typeBase instanceof QmlFile) {
                _typeBase = _typeBase.instruction.getTypeInfo(_typeBase.instruction.extends).type
            }
            return {
                path: _path,
                type: _type,
                typeBase: _typeBase,
            }
        } else {
            throw `${this.qmlFile.fileName}:${this.info.line + 1}:${this.info.col + 1}: error: ${type} is not founded`
        }
    }

    getSignalArgs(name) {
        for (let defineSignal of this.defineSignals) {
            if (defineSignal.name === name) {
                return defineSignal.args
            }
        }

        try {
            let typeInfo = this.getTypeInfo(this.extends)
            if (typeInfo.type instanceof QmlFile) {
                return typeInfo.type.instruction.getSignalArgs(name)
            } else {
                return typeInfo.type.meta[name].args ? typeInfo.type.meta[name].args : []
            }
        } catch {

        }

    }

    getMeta(isRoot = false) {
        let meta = []
        meta.push(`{`)

        let typeInfo = this.getTypeInfo(this.extends)

        if (this.defineProperties.length || this.defineSignals.length) {
            for (let defineProperty of this.defineProperties) {
                let _typeInfo
                try {
                    _typeInfo = this.getTypeInfo(defineProperty.type)

                    if (isRoot && _typeInfo.type instanceof QmlFile) {
                        this.qmlFile.dependencies.add(_typeInfo.type)
                    }
                } catch (error) {
                    throw `${this.qmlFile.fileName}:${defineProperty.info.line + 1}:${defineProperty.info.col + 1}: error: ${defineProperty.info.value} is not founded`
                }


                if (typeof defineProperty.value === 'object') {
                    if (_typeInfo.type instanceof QmlFile || _typeInfo.type.isAssignableFrom(QtQml.QObject)) {
                        if (typeInfo.typeBase.isAssignableFrom(JQModules.QtQml.SDLObject)) {
                            meta.push(`${defineProperty.name}:{type:JQModules.QtQml.SDLProperty, typeTarget:${_typeInfo.path}},`)
                        } else {
                            meta.push(`${defineProperty.name}:{type:JQModules.QtQml.variant, typeTarget:${_typeInfo.path}},`)
                        }
                    } else {
                        if (typeInfo.typeBase.isAssignableFrom(JQModules.QtQml.SDLObject)) {
                            meta.push(`${defineProperty.name}:{type:JQModules.QtQml.SDLProperty},`)
                        } else {
                            meta.push(`${defineProperty.name}:{type:${_typeInfo.path}},`)
                        }
                        
                    }
                    continue
                }

                if (typeof defineProperty.value === 'string') {
                    if (typeInfo.typeBase.isAssignableFrom(JQModules.QtQml.SDLObject)) {
                        meta.push(`${defineProperty.name}:{type:JQModules.QtQml.SDLProperty, value:'${defineProperty.value}'},`)
                    } else {
                        meta.push(`${defineProperty.name}:{type:${_typeInfo.path}, value:'${defineProperty.value}'},`)
                    }
                    
                } else {
                    if (_typeInfo.type instanceof QmlFile || _typeInfo.type.isAssignableFrom(QtQml.QObject)) {
                        if (typeInfo.typeBase.isAssignableFrom(JQModules.QtQml.SDLObject)) {
                            meta.push(`${defineProperty.name}:{type:JQModules.QtQml.SDLProperty, typeTarget:${_typeInfo.path}, value:${defineProperty.value}},`)
                        } else {
                            meta.push(`${defineProperty.name}:{type:JQModules.QtQml.variant, typeTarget:${_typeInfo.path}, value:${defineProperty.value}},`)
                        }
                        
                    } else {
                        if (typeInfo.typeBase.isAssignableFrom(JQModules.QtQml.SDLObject)) {
                            meta.push(`${defineProperty.name}:{type:JQModules.QtQml.SDLProperty, value:${defineProperty.value}},`)
                        } else {
                            meta.push(`${defineProperty.name}:{type:${_typeInfo.path}, value:${defineProperty.value}},`)
                        }
                        
                    }

                }

            }
            for (let defineSignal of this.defineSignals) {
                meta.push(`${defineSignal.name}:{type:JQModules.QtQml.Signal, args:[${defineSignal.args.join(',')}]},`)
            }
        }

        for (let defineMethod of this.defineMethods) {
            meta.push(`${defineMethod.name}:{type:JQModules.QtQml.Method},`)
        }

        meta.push('}')

        return meta.join('')
    }

    checkDefineProperty(name){
        for(let defineProperty of this.defineProperties){
            if(name === defineProperty.name) return true
        }

        return false
    }

    getProperties() {
        let code = new SourceNode()
        let lazyCode = new SourceNode()
        let aliasCode = new SourceNode()
        let classCode =  new SourceNode()

        for (let assignProperty of this.assignProperties) {
            let path = this.resolve(assignProperty.name.split('.')[0], this.name)
            if (!path) {
                console.log(`${this.qmlFile.fileName}:${assignProperty.value.info.line + 1}:${assignProperty.value.info.col - assignProperty.name.length - 1}: warning: ${assignProperty.name} is not founded`)
            }

            if (assignProperty.value instanceof Instruction) {
                if (!path) {
                    throw `${this.qmlFile.fileName}:${assignProperty.value.info.line + 1}:${assignProperty.value.info.col - assignProperty.name.length - 1}: error: ${assignProperty.name} is not founded`
                }

                let resultCode
                if(this.checkDefineProperty(assignProperty.name)){
                    resultCode = classCode
                } else {
                    resultCode = lazyCode
                }

                if (path.type === QtQml.Component && assignProperty.value.extends !== 'Component') {
                    resultCode.add(`let ${assignProperty.value.name}=JQModules.QtQml.Component.create(null,__context,`)
                    resultCode.add('\n')

                    let childTypeInfo = assignProperty.value.getTypeInfo()
                    let childMeta = assignProperty.value.getMeta()

                    // if (childTypeInfo.type instanceof QmlFile) {
                    //     this.qmlFile.dependencies.add(childTypeInfo.type)
                    // }

                    let properties = assignProperty.value.getProperties()

                    resultCode.add(`(class ${assignProperty.value.className} extends ${childTypeInfo.path} {
                        static meta = Object.assign({}, ${childTypeInfo.path}.meta, ${childMeta})
                        static create(parent,properties={},context={},isRoot=true){

                        let __context = JQContext.create(context)

                        let ${assignProperty.value.name} = super.create(parent,properties,context,false)
                        ${assignProperty.value.name}.__${this.qmlFile.getContextName()} = __context`)

                    resultCode.add('\n')
                    if (assignProperty.value.id) resultCode.add(`${assignProperty.value.name}.__${this.qmlFile.getContextName()}.${assignProperty.value.id}=${assignProperty.value.name}`)
                    resultCode.add('\n')
                    resultCode.add(properties.classCode)


                    resultCode.add(properties.aliasCode)
                    resultCode.add(properties.code)


                    // // children 
                    resultCode.add('\n')

                    for (let i = 0; i < assignProperty.value.children.length; i++) {
                        // resultCode.add(`let ${assignProperty.value.children[i].name}=`)
                        resultCode.add(assignProperty.value.children[i].toCode())
                        resultCode.add('\n')
                    }

                    // // children
                    resultCode.add('\n')
                    resultCode.add(properties.lazyCode)

                    resultCode.add('\n')
                    // resultCode.add(assignProperty.value.getConnectedSignals())
                    resultCode.add(`${assignProperty.value.name}.__${assignProperty.value.className}__${assignProperty.value.name}=true`)

                    resultCode.add('\n')

                    resultCode.add(`if(isRoot) {${assignProperty.value.name}.__updatePrimaryProperties();${assignProperty.value.name}.__updateProperties();${assignProperty.value.name}.__complete();${assignProperty.value.name}.__completeProperties()}`)

                    resultCode.add('\n')
                    resultCode.add(`return ${assignProperty.value.name}`)

                    resultCode.add('\n')

                    resultCode.add('}')

                    resultCode.add('\n')

                    resultCode.add('__dynamic=true')

                    resultCode.add('\n')

                    resultCode.add(assignProperty.value.getMethods())

                    resultCode.add('\n')
                    resultCode.add(assignProperty.value.getConnectedSignals())

                    resultCode.add('\n')

                    resultCode.add(`})`)

                    resultCode.add('\n')

                    resultCode.add(`)`)
                    resultCode.add('\n')


                    
                    if(this.checkDefineProperty(assignProperty.name)){
                        // lazyCode.add(`${this.name}.__self['${assignProperty.name}']=${assignProperty.value.name}`)
                        classCode.add(`${assignProperty.value.name}.__addLink()`)
                        classCode.add('\n')
                        classCode.add(`${this.name}.${assignProperty.name}=${assignProperty.value.name}`)
                        classCode.add('\n')
                        lazyCode.add(`${this.name}.__properties['${assignProperty.name}']='JQObject'`)
                        lazyCode.add('\n')
                    } else {
                        lazyCode.add(`${this.name}.__properties['${assignProperty.name}']=${assignProperty.value.name}`)
                        lazyCode.add('\n')
                    }
                    
                } else {
                    if(this.checkDefineProperty(assignProperty.name)){
                        // lazyCode.add(`${assignProperty.value.toCode()}`)
                        // lazyCode.add(`${this.name}.__self['${assignProperty.name}']=${assignProperty.value.name}`)
                        classCode.add('\n')
                        classCode.add(assignProperty.value.toCode())
                        classCode.add('\n')
                        // classCode.add(`${assignProperty.value.name}.__addLink()`)
                        classCode.add(`${this.name}.${assignProperty.name}=${assignProperty.value.name}`)
                        classCode.add('\n')
                        lazyCode.add(`${this.name}.__properties['${assignProperty.name}']='JQObject'`)
                        lazyCode.add('\n')
                    } else {    
                        lazyCode.add('\n')
                        lazyCode.add(assignProperty.value.toCode())
                        lazyCode.add('\n')
                        lazyCode.add(`${this.name}.__properties['${assignProperty.name}']=${assignProperty.value.name}`)
                        lazyCode.add('\n')
                    }
                    
                }

            } else {
                let names = assignProperty.name.split('.')

                let stat = this.prepare(assignProperty.value, { isCompute: false, thisKey: this.name, value: new SourceNode(), local: [] })
                if (stat.isCompute) {
                    if (assignProperty.type === 'alias') {
                        // console.log('alias')
                        // console.log(assignProperty.name, stat.value.toString())
                        // aliasCode.add(`JQModules.QtQml.alias.init(${this.name},'${assignProperty.name}',function(){return ${stat.value}},function(newVal){${stat.value}=newVal})`)
                        let aliasPath = stat.value.toString().split('.')
                        // aliasCode.add(`JQModules.QtQml.alias.init(${this.name},'${assignProperty.name}',${aliasPath.slice(0, aliasPath.length - 1).join('.')}, '${aliasPath[aliasPath.length-1]}')`)
                        aliasCode.add(`JQModules.QtQml.alias.init(${this.name},'${assignProperty.name}',()=>{return ${aliasPath.slice(0, aliasPath.length - 1).join('.')}}, '${aliasPath[aliasPath.length-1]}')`)
                        // code.add(`${this.name}.__getDataQml('${assignProperty.name}').__aliasInit(()=>{return ${stat.value}},(val)=>{${stat.value}=val},properties)`)
                        aliasCode.add('\n')
                    } else {
                        // lazyCode.add(`'${assignProperty.name}': function(){return ${stat.value}},`)
                        if(names.length > 1){
                            lazyCode.add(`${this.name}['${names[0]}'].__properties['${names[1]}']=function(){return ${stat.value}}`)
                            lazyCode.add('\n')
                            lazyCode.add(`${this.name}.__properties['${names[0]}']='JQGroup'`)
                            lazyCode.add('\n')
                        } else {
                            lazyCode.add(`${this.name}.__properties['${assignProperty.name}']=function(){return ${stat.value}}`)
                            lazyCode.add('\n')
                        }
                        
                    }
                } else {
                    if(names.length > 1){
                        code.add(`${this.name}['${names[0]}'].__properties['${names[1]}']=${stat.value}`)
                        code.add('\n')
                        code.add(`${this.name}['${names[0]}'].__updateProperties()`)
                        code.add('\n')
                    } else {
                        if(this.checkDefineProperty(assignProperty.name)){
                            classCode.add(`${this.name}.${assignProperty.name}=${stat.value}`)
                            classCode.add('\n')
                        } else {
                            // lazyCode.add(`${this.name}.__properties['${assignProperty.name}']=${stat.value}`)
                            lazyCode.add(`${this.name}.${assignProperty.name}=${stat.value}`)
                            lazyCode.add('\n')
                        }
                        
                    }
                    
                }
            }
            code.add('\n')
            lazyCode.add('\n')
            aliasCode.add('\n')
            classCode.add('\n')

        }

        return { code: code, lazyCode: lazyCode , aliasCode: aliasCode, classCode: classCode }
    }

    getConnectedSignals() {
        let code = new SourceNode()
        let typeInfo = this.getTypeInfo()

        for (let connectedSignal of this.connectedSignals) {
            if (typeInfo.typeBase.isAssignableFrom(JQModules.QtQuick.MouseArea) && connectedSignal.slotName === 'onPressed') {
                connectedSignal.slotName = 'onPressedChanged'
            }
            let names = connectedSignal.slotName.replaceAll("['", '').replaceAll("']", '').split('.')
            let signalName = ''
            if (names.length === 1) {
                signalName = names[0][2].toLowerCase() + names[0].slice(3)
            } else {
                signalName = names[0] + '.' + names[1][2].toLowerCase() + names[1].slice(3)
            }
            let args = this.getSignalArgs(signalName)
            connectedSignal.args = []

            let ids = new Set()
            let context = new Set()

            if (args){
                if(args.length){
                    for (let arg of args) {
                        connectedSignal.args.push(arg.replaceAll('`', ''))
                    }

                    for(let name of Object.keys(this.qmlFile.context)){
                        if(connectedSignal.args.indexOf(name) < 0){
                            ids.add(name)
                            context.add(`__self.__${this.qmlFile.getContextName()}.${name}`)
                        }
                    }
                } else {
                    for(let name of Object.keys(this.qmlFile.context)){
                        ids.add(name)
                        context.add(`__self.__${this.qmlFile.getContextName()}.${name}`)
                    }
                }
            } else {
                for(let name of Object.keys(this.qmlFile.context)){
                    ids.add(name)
                    context.add(`__self.__${this.qmlFile.getContextName()}.${name}`)
                }
            }
                

            
            

            if(signalName.indexOf('.') >= 0){
                code.add(`'SLOT_${signalName}'(${connectedSignal.args.join(',')}){
                    let __self = this
                    let [${Array.from(ids).join(',')}] = [${Array.from(context).join(',')}]
                    if(super['SLOT_${signalName}']) super['SLOT_${signalName}'](...arguments)
                    if(!this.__${this.className}__${this.name}) return
                    try{JQApplication.beginUpdate();`)
            } else {
                code.add(`SLOT_${signalName}(${connectedSignal.args.join(',')}){
                    let __self = this
                    let [${Array.from(ids).join(',')}] = [${Array.from(context).join(',')}]
                    if(super.SLOT_${signalName}) super.SLOT_${signalName}(...arguments)
                    if(!this.__${this.className}__${this.name}) return
                    try{JQApplication.beginUpdate();`)
            }
            

            let stat = this.prepare(connectedSignal.source, { isCompute: false, thisKey: '__self', value: new SourceNode(), local: [connectedSignal.args] })
            code.add(stat.value)
            code.add(`}finally{JQApplication.endUpdate()}}`)
            code.add('\n')
        }
        code.add('\n')
        return code
    }

    getMethods() {
        let code = new SourceNode()
        let connectionsInfo = {}
        let typeInfo = this.getTypeInfo()

        for (let defineMethod of this.defineMethods) {
            if (defineMethod.name.slice(0, 2) === 'on') {
                let signalName = defineMethod.name[2].toLowerCase() + defineMethod.name.slice(3)
                connectionsInfo[signalName] = defineMethod.name
            }
            let stat = this.prepare(defineMethod.source, { isCompute: false, thisKey: '__self', value: new SourceNode(), local: [] })
            code.add(new SourceNode(null, null, null, stat.value))
            code.add('\n')
        }

        if (typeInfo.typeBase.isAssignableFrom(JQModules.QtQml.Connections)) {
            code.add(`__connectionsInfo = ${JSON.stringify(connectionsInfo)}`)
        }

        code.add('\n')

        return code
    }

    toComponentCode() {
        let typeInfo = this.getTypeInfo()

        if (typeInfo.type === JQModules.QtQml.Component && this.children.length < 1) {
            throw `${this.qmlFile.fileName}:${this.info.line + 1}:${this.info.col + 1}: error: Cannot create empty component specification`
        }
        if (this.children.length > 1) {
            throw `${this.qmlFile.fileName}:${this.info.line + 1}:${this.info.col + 1}: error: Invalid component body specification`
        }

        let code = new SourceNode()


        code.add(`let ${this.name}=(__root.cachedComponents['${this.qmlFile.getName()}__${this.name}'] || __root.cachedComponent('${this.qmlFile.getName()}__${this.name}',class ${this.className} extends ${typeInfo.path} {
                static create(parent,context={},component=null){
                    let ${this.name} = super.create(parent,context,component)
                    ${this.name}.__${this.qmlFile.getContextName()} = context
                    ${this.id ? this.name+'.__'+this.qmlFile.getContextName()+'.'+this.id+'='+this.name : ''}
                    return ${this.name}
                }
        })).create(${this.parent ? this.parent.name : 'null'},${this.targetContext.name}.__${this.qmlFile.getContextName()},`)

        code.add('\n')
        // if (this.parent) {
        //     code.add(`${typeInfo.path}.create(${this.parent.name},`)
        // } else {
        //     code.add(`${typeInfo.path}.create(null,`)
        // }

        let childTypeInfo = this.children[0].getTypeInfo()
        let childMeta = this.children[0].getMeta()

        // if (childTypeInfo.type instanceof QmlFile) {
        //     this.qmlFile.dependencies.add(childTypeInfo.type)
        // }

        let properties = this.children[0].getProperties()

        code.add(`(class ${this.children[0].className} extends ${childTypeInfo.path} {
            static meta = Object.assign({}, ${childTypeInfo.path}.meta, ${childMeta})
            static create(parent,properties={},context={},isRoot=true){

            let __context = JQContext.create(context)

            let ${this.children[0].name} = super.create(parent,properties,context,false)

            ${this.children[0].name}.__${this.qmlFile.getContextName()} = __context`)
        
        code.add('\n')
        if (this.children[0].id) code.add(`${this.children[0].name}.__${this.qmlFile.getContextName()}.${this.children[0].id}=${this.children[0].name}`)
        code.add('\n')
        code.add(properties.classCode) 

 
        code.add(properties.aliasCode)
        code.add(properties.code)


        // // children 

        code.add('\n')

        for (let i = 0; i < this.children[0].children.length; i++) {
            // code.add(`let ${this.children[0].children[i].name}=`)
            code.add(this.children[0].children[i].toCode())
            code.add('\n')
        }

        // // children

        code.add('\n')

        code.add(properties.lazyCode)

        code.add('\n')

        // code.add(this.children[0].getConnectedSignals())
        code.add(`${this.children[0].name}.__${this.children[0].className}__${this.children[0].name}=true`)

        code.add('\n')
        code.add(`if(isRoot) {${this.children[0].name}.__updatePrimaryProperties();${this.children[0].name}.__updateProperties();${this.children[0].name}.__complete();${this.children[0].name}.__completeProperties()}`)

        code.add('\n')
        code.add(`return ${this.children[0].name}`)

        code.add('\n')

        code.add('}')

        code.add('\n')

        code.add('__dynamic=true')

        code.add('\n')

        code.add(this.children[0].getMethods())

        code.add('\n')
        code.add(this.children[0].getConnectedSignals())

        code.add('\n')

        code.add(`})`)

        code.add('\n')


        code.add(`)`)

        code.add('\n')

        // if (this.id) code.add(`${this.name}.__context.${this.id}=${this.name}`)

        return code
    }

    toCode() {
        let code = new SourceNode()
        let childrenCode = new SourceNode()
        let typeInfo = this.getTypeInfo()

        // if (typeInfo.type instanceof QmlFile) {
        //     this.qmlFile.dependencies.add(typeInfo.type)
        // }

        // if (typeInfo.typeBase.isAssignableFrom(JQModules.QtQml.Component)) return this.toComponentCode()
        if (typeInfo.type === JQModules.QtQml.Component) return this.toComponentCode()

        let meta = this.getMeta()

        let properties = this.getProperties()


        let id = ''

        if(typeof this.id === 'object') {
            let stat = this.prepare(this.id, { isCompute: false, thisKey: this.name, value: new SourceNode(), local: [] })
            id = stat.value.toString()   
        } else {
            id = this.id ? this.name+'.__' + this.qmlFile.getContextName() + '.'+this.id+'='+this.name : ''
        }

        code.add(`let ${this.name}=(__root.cachedComponents['${this.qmlFile.getName()}__${this.name}'] || __root.cachedComponent('${this.qmlFile.getName()}__${this.name}',class ${this.className} extends ${typeInfo.path} {
            static meta = Object.assign({}, ${typeInfo.path}.meta, ${meta})
            static create(parent,properties={},context={},isRoot=true){
                let ${this.name} = super.create(parent,properties,context,isRoot)
                ${this.name}.__${this.qmlFile.getContextName()} = context
                ${id}`)

        code.add('\n')

        code.add(properties.classCode)      
        code.add(properties.aliasCode)
        code.add(properties.code)

        code.add(`return ${this.name}}`)
        code.add(this.getMethods())
        code.add(this.getConnectedSignals())
        code.add(`})).create(${this.parent ? this.parent.name : 'null'},{JQAbstractModel:()=>{return ${this.targetContext.name}.JQAbstractModel}},${this.targetContext.name}.__${this.qmlFile.getContextName()},false)`)

        code.add('\n')
        
        for (let i = 0; i < this.children.length; i++) {
            // childrenCode.add(`let ${this.children[i].name}=`)
            code.add(this.children[i].toCode())
            code.add('\n')
        }

        code.add('\n')


        code.add(properties.lazyCode)

        code.add('\n')
        
        code.add(`${this.name}.__${this.className}__${this.name}=true`)

        code.add('\n')

        return code
    }
}

let singletonList = []

class QmlFile {
    imports = []
    context = {}
    singleton = false
    fileName = ''
    moduleName = ''
    instruction = null
    updateList = []
    version = 1.0
    dependencies = new Set()
    elementsCount = 0

    constructor(fileName, moduleName, version = 1) {
        this.fileName = fileName
        this.version = version
        this.moduleName = moduleName
        let data = fs.readFileSync(fileName, { encoding: 'utf8', flag: 'r' })

        parser.parse.nowParsingFile = fileName

        let meta = parser.parse(data)

        if (meta[3]) this.qmlpragma(meta[3])
        this.qmlimport(meta[1])

        let className = fileName.split(/[\/\\]+/g).pop().replace('.qml', '')
        if (this.singleton) Singletons[className] = this

        if (meta[2] && meta[2][0] === 'qmlelem') {
            this.instruction = new Instruction(null, className, meta[2][1], meta[2][3], meta[2][2], this, meta[2].info)
        } else if (meta[2] && meta[2][0] === 'qmlobj') {
            this.instruction = new Instruction(null, className, meta[2][1].slice(1).join('.'), meta[2][2], null, this, meta[2].info)
        }
    }

    getDefaultValue(){
        return null
    }

    qmlpragma(meta) {
        for (let pragma of meta) {
            if (pragma[0] === 'qmlpragma') {
                if (pragma[1] === 'Singleton') {
                    this.singleton = true

                }
            }
        }
    }

    qmlimport(meta) {
        for (let imp of meta) {
            if (imp[0] === 'qmlimport') {
                if (imp[1].indexOf('js') >= 0) {
                    // jsName = imp[1]
                    // jsAs = imp[3]
                    // let path = file.split('/')
                    // path = path.slice(0, path.length-1).join('/') + '/' + jsName

                    // if(fs.existsSync(path)){
                    //     let content = fs.readFileSync(path, encoding='utf-8')
                    //     let meta = parser.jsparse(content)

                    //     instructions.js[jsName] = meta.source.replaceAll(/(?<=[^\\])[`]/g, '\\`') + (jsAs ? `\r\nvar ${jsAs} = {${meta.exports.join(',')}}\r\n` : '')
                    // } else {
                    //     console.log(`WARNING ${path} does not exist`)
                    // }

                } else {
                    this.imports.push({
                        path: imp[1],
                        version: imp[2],
                        as: imp[3],
                    })
                }
            }
        }
    }

    getName(){
        return this.fileName.replaceAll('.qml', '').split(/[\/\\]+/).pop()
    }

    getContextName(){
        return this.fileName.replaceAll('.qml', '').split(/[\/\\]+/).pop() + 'Context'
    }

    toCode() {
        let code = new SourceNode()
        let typeInfo = this.instruction.getTypeInfo()
        let meta = this.instruction.getMeta(true)

        if (typeInfo.type instanceof QmlFile) {
            this.dependencies.add(typeInfo.type)
        }

        if (this.singleton) {
            if (this.moduleName) {
                code.add(`JQModules.${this.moduleName}.${this.instruction.className} = (class extends ${typeInfo.path} {\n`)
            } else {
                code.add(`const ${this.instruction.className} = (class extends ${typeInfo.path} {\n`)
            }

            code.add(`static singleton = true\n`)
        } else {
            code.add(`class ${this.instruction.className} extends ${typeInfo.path} {\n`)
        }

        let properties = this.instruction.getProperties()

        code.add(`static cachedComponents = {}

            static meta = Object.assign({}, ${typeInfo.path}.meta, ${meta})

            __removeObjectName(){removeObjectName('${this.instruction.className}')}
            __addObjectName(){addObjectName('${this.instruction.className}')}

            static create(parent,properties={},context={},isRoot=true){

            let __root = this
            let __context = JQContext.create()

            let ${this.instruction.name} = super.create(parent,properties,context,false)

            ${this.instruction.name}.__${this.getContextName()} = __context`)
        code.add('\n')
        if (this.instruction.id) code.add(`${this.instruction.name}.__${this.getContextName()}.${this.instruction.id}=${this.instruction.name}`)
        code.add('\n')
        code.add(properties.classCode)


        code.add(properties.aliasCode)


        code.add(properties.code)


        // children
        code.add('\n')

        for (let i = 0; i < this.instruction.children.length; i++) {
            // code.add(`let ${this.instruction.children[i].name}=`)
            code.add(this.instruction.children[i].toCode())
            code.add('\n')
        }

        // children
        code.add('\n')

        code.add(properties.lazyCode)

        code.add('\n')

        // code.add(this.instruction.getConnectedSignals())
        code.add(`${this.instruction.name}.__${this.instruction.className}__${this.instruction.name}=true`)

        code.add('\n')

        code.add(`if(isRoot) {${this.instruction.name}.__updatePrimaryProperties();${this.instruction.name}.__updateProperties();${this.instruction.name}.__complete();${this.instruction.name}.__completeProperties()}`)

        code.add('\n')
        code.add(`return ${this.instruction.name}`)
        code.add('\n')

        code.add('}')

        code.add('\n')

        code.add(this.instruction.getMethods())

        code.add('\n')
        code.add(this.instruction.getConnectedSignals())

        code.add('\n')

        if (this.singleton) {
            code.add(`}).create()`)
        } else {
            code.add(`}`)
        }

        code.add('\n')

        return code
    }
}


class JSFile {
    fileName = ''
    moduleName = ''
    version = 1
    compiled = null

    constructor(fileName, moduleName, version = 1) {
        this.fileName = fileName
        this.version = version
        this.moduleName = moduleName
        let data = fs.readFileSync(fileName, { encoding: 'utf8', flag: 'r' })

        parser.parse.nowParsingFile = fileName

        this.meta = parser.jsparse(data)
    }

    toCode() {
        return `function(){${this.meta.source}; return {${this.meta.exports.join(',')}}}()`
    }
}

let fullCode = new SourceNode()
let compiledFiles = []

let mainData = fs.readFileSync(path.resolve(__dirname, '../dist/main.js'), { encoding: 'utf8', flag: 'r' })
fullCode.add(mainData)

if (config.dirs.length) console.log(`JQ: preparation of third party modules`)

let counter = {

}

for (let dirPath of config.dirs) {
    let absolutePath = path.resolve(configDirPath, dirPath)
    let moduleName = ''
    let lines = fs.readFileSync(absolutePath + '/qmldir', { encoding: 'utf8', flag: 'r' }).replaceAll('\r', '').replaceAll(/[ ]+/g, ' ').split('\n')
    let count = 0
    for (let line of lines) {
        let params = line.trim().split(' ')
        if (params.length === 2) {
            let [type, name] = params
            if (type === 'module') {
                moduleName = name

                console.log(`    > ${name}`)

                if (JQModules[name]) continue

                JQModules[name] = {}
            }
        } else if (params.length === 3) {
            count += 1
            let [className, version, filePath] = params

            version = parseFloat(version)
            let absoluteFilePath = path.resolve(absolutePath, filePath)
            let extension = filePath.split('.').pop()
            let file = null

            if (extension === 'qml') {
                file = new QmlFile(absoluteFilePath, moduleName, version)
            } else if (extension === 'js') {
                file = new JSFile(absoluteFilePath, moduleName, version)
            }

            if (JQModules[moduleName][className]) {
                if (JQModules[moduleName][className].version < file.version) JQModules[moduleName][className] = file
            } else {
                JQModules[moduleName][className] = file
            }
            JQModules[moduleName][className + '_v' + version] = file
        } else if (params.length === 4) {
            count += 1
            let [singleton, className, version, filePath] = params

            if (filePath.split('.').pop() !== 'qml') continue

            version = parseFloat(version)
            let absoluteFilePath = path.resolve(absolutePath, filePath)

            let qmlFile = new QmlFile(absoluteFilePath, moduleName, version)

            if (!qmlFile.singleton && singleton) {
                throw `${absoluteFilePath}:1:1: error: pragma Singleton is not defined`
            }

            if (JQModules[moduleName][className]) {
                if (JQModules[moduleName][className].version < qmlFile.version) JQModules[moduleName][className] = qmlFile
            } else {
                JQModules[moduleName][className] = qmlFile
            }
            JQModules[moduleName][className + '_v' + version] = qmlFile
        }
    }

    counter[moduleName] = count
}

console.log(`JQ: preparation of single files`)
for (let fileName of getFiles(options.entry.replaceAll(/.\w+\.qml/g, ''))) {
    let qmlFile = new QmlFile(fileName)
    SingleFiles[fileName.split(/[\/\\]+/g).pop().replace('.qml', '')] = qmlFile
}

console.log(`JQ: compilation of single files`)


for (let className in Singletons) {
    console.log(`    > ${className}.qml (Singleton)`)
    compiledFiles.push({
        file: Singletons[className],
        code: Singletons[className].toCode(),
    })
}

for (let className in SingleFiles) {
    if (SingleFiles[className].singleton) {
        continue
    }
    console.log(`    > ${className}.qml`)
    compiledFiles.push({
        file: SingleFiles[className],
        code: SingleFiles[className].toCode(),
        path: className,
    })
}

if (config.dirs.length) console.log(`JQ: compilation of third party modules`)
for (let moduleName in JQModules) {
    if (!BaseModules[moduleName]) {
        console.log(`    > ${moduleName} (${counter[moduleName] + ' files'})`)
        fullCode.add(`JQModules.${moduleName}={},`)
    }
}

for (let moduleName in JQModules) {
    let queue = {}
    for (let className in JQModules[moduleName]) {
        let file = JQModules[moduleName][className]
        if (file instanceof QmlFile || file instanceof JSFile) {
            if (file.singleton) {
                continue
            }
            if (JQModules[moduleName][className + '_v' + file.version]) {
                queue[className] = file
                continue
            }

            let code = new SourceNode()
            code.add(`JQModules.${moduleName}.${className}=`)
            code.add(file.toCode())

            compiledFiles.push({
                file: file,
                code: code,
                path: `JQModules.${moduleName}.${className}`,
            })
        }
    }
    for (let className in queue) {
        let file = queue[className]
        fullCode.add(`Object.defineProperty(JQModules.${moduleName},'${className}',{get:()=>{return JQModules.${moduleName}.${className}_v${file.version}}})`)
    }
}

function getFiles(dir, _files) {
    _files = _files || []
    let files = fs.readdirSync(dir)
    for (var i in files) {
        let name = dir + '/' + files[i]
        if (!(fs.statSync(name).isDirectory())) {
            if (path.extname(name) === '.qml') _files.push(name.replaceAll(/\\+/g, '/'))
        }
    }
    return _files
}



while (compiledFiles.length) {
    let compiledFile = compiledFiles.shift()
    if(compiledFile.file instanceof QmlFile && compiledFile.file.singleton){
        singletonList.push(compiledFile)
        continue
    }

    if (compiledFile.file instanceof QmlFile && compiledFile.file.dependencies.size) {
        let found = false
        for (let i = 0; i < compiledFiles.length; i++) {
            if (compiledFile.file.dependencies.has(compiledFiles[i].file)) {
                compiledFiles.splice(i + 1, 0, compiledFile)
                found = true
                break
            }
        }
        if (!found) {
            fullCode.add(compiledFile.code)
        }
    } else {
        fullCode.add(compiledFile.code)
    }
    
}

while (singletonList.length) {
    let compiledFile = singletonList.shift()

    if (compiledFile.file instanceof QmlFile && compiledFile.file.dependencies.size) {
        let found = false
        for (let i = 0; i < singletonList.length; i++) {
            if (compiledFile.file.dependencies.has(singletonList[i].file)) {
                singletonList.splice(i + 1, 0, compiledFile)
                found = true
                break
            }
        }
        if (!found) {
            fullCode.add(compiledFile.code)
        }
    } else {
        fullCode.add(compiledFile.code)
    }
    
}


if(options.mode === 'js'){
    if (options.entry) {
        fullCode.add(`window.addEventListener('load', ()=>{console.time('build');JQApplication.rootPath='${options.root}';${options.entry.split(/[\/\\]+/g).pop().replace('.qml', '')}.create(JQApplication.root);console.timeEnd('build')})`)
    }

    if (options.output) {
        let output = options.output + `/${options.name}.js`

        fullCode.add(`//# sourceMappingURL=${output + '.map'}`)
        let result = fullCode.join('\n').toStringWithSourceMap({ file: output })

        fs.writeFileSync(output, result.code)

        fs.writeFileSync(output+'.map', result.map.toString())



        // let resultCode = UglifyJS.minify(result.code, {compress: {}}).code
        // fs.writeFileSync(output, resultCode)

        // fs.writeFileSync(path.resolve(configDirPath, config.output)+'.map', result.map.toString())
    }
}

