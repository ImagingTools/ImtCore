const fs = require('fs')
const os = require('os')
const path = require('path')
const parser = require('./parser')

const BaseObject = require('../QtBase/BaseObject')
const Qt = require('../Qt/Qt')
const QtQml = require('../QtQml/QtQml')
const QtQuick = require('../QtQuick/QtQuick')
const Qt5Compat = require('../Qt5Compat/Qt5Compat')
const QtWebSockets = require('../QtWebSockets/QtWebSockets')

const configFilePath = process.argv.slice(2)[0]
const configDirPath = configFilePath.split(/[\\\/]+/g).slice(0, -1).join('/')
const config = JSON.parse(fs.readFileSync(configFilePath, {encoding:'utf8', flag:'r'}))

const BaseModules = {
    Qt,
    QtQml,
    QtQuick,
    Qt5Compat,
    QtWebSockets,
}

const JQModules = {
    ...BaseModules
}

const SingleFiles = {

}

const Singletons = {

}

class Instruction {
    defineProperties = []
    defineSignals = []
    defineMethods = []
    connectedSignals = []
    assignProperties = []

    id = ''
    qmlFile = null
    className = ''
    extends = null
    parent = null

    children = []

    constructor(parent, className, _extends, meta, on, qmlFile, info){
        this.parent = parent
        this.className = className
        this.extends = typeof _extends === 'object' ? _extends.slice(1).join('.') : _extends
        this.qmlFile = qmlFile
        this.info = info

        for(let m of meta){
            if(m) this[m[0]](m)
        }
    }

    qmlelem(meta){
        this.children.push(new Instruction(this, '', meta[1], meta[3], meta[2], this.qmlFile, meta.info)) 
    }
    qmlsignaldef(meta){
        let args = []
        for(let arg of meta[2]){
            args.push(`\`${arg.name}\``)
        }
        this.defineSignals.push({
            name: meta[1],
            slotName: 'on'+meta[1][0].toUpperCase()+meta[1].slice(1),
            args: args
        })
    }
    qmlmethod(meta){
        this.defineMethods.push({
            name: meta[1],
            args: meta[2][2],
            source: meta[2]
        })
    }
    qmlaliasdef(meta){
        this.defineProperties.push({
            name: meta[1],
            type: 'alias',
        })
        this.assignProperties.push({
            name: meta[1],
            value: meta,//meta[meta.length - 1] === '.' ? meta.slice(2, meta.length - 1) : meta.slice(2),
        })
    }
    qmlpropdef(meta){
        let type = {}
        try {
            let typeInfo = this.getTypeInfo(meta[3])
            type = typeInfo.type
        } catch {
            
        }
        
        if(meta[4] && meta[4][1][0] === 'qmlelem'){
            this.defineSignals.push({
                name: meta[2]+'Changed',
                slotName: 'on'+ meta[2][0].toUpperCase() + meta[2].slice(1) +'Changed',
                args: []
            }) 

            this.defineProperties.push({
                name: meta[2],
                type: meta[3],
                value: type.defaultValue,
            }) 

            this.assignProperties.push({
                name: meta[2],
                value: new Instruction(null, '', meta[4][1][1], meta[4][1][3], meta[4][1][2], this.qmlFile, meta[4][1].info),
            })
        } else {
            let defaultValue = type.defaultValue
            
            if(meta[5]){
                try {
                    defaultValue = eval(meta[5])
                } catch {
                    this.assignProperties.push({
                        name: meta[2],
                        value: meta[4],
                    })
                }
            }
            
            this.defineSignals.push({
                name: meta[2]+'Changed',
                slotName: 'on'+ meta[2][0].toUpperCase() + meta[2].slice(1) +'Changed',
                args: []
            })

            this.defineProperties.push({
                name: meta[2],
                type: meta[3],
                value: defaultValue, 
                signalName: meta[2]+'Changed',
            }) 

            
        }
        
    }
    qmlprop(meta){
        if(meta[2][0] === "block" || meta[2][1][0] === "assign"){
            if(meta[1][0] === "dot"){
                let name = meta[1].slice(1)
    
                if(name[1].slice(0, 2) === 'on'){
                    this.connectedSignals.push({
                        slotName: `['${name.join('.')}']`,
                        args: [],
                        source: meta[2],
                    })
                } else {
                    if(meta[2][1][0] === 'qmlelem'){
                        this.assignProperties.push({
                            name: name.join('.'),
                            value: new Instruction(null, '', meta[2][1][1], meta[2][1][3], meta[2][1][2], this.qmlFile, meta[2][1].info),
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
    
                if(name.slice(0, 2) === 'on'){
                    let signalName = name.slice(2)
                    signalName = signalName[0].toLowerCase() + signalName.slice(1)
                    this.connectedSignals.push({
                        slotName: name,
                        args: [], // Нужно отследить количество аргументов
                        source: meta[2],
                    })
                } else {
                    if(meta[2][1][0] === 'qmlelem'){
                        this.assignProperties.push({
                            name: name.join('.'),
                            value: new Instruction(null, '', meta[2][1][1], meta[2][1][3], meta[2][1][2], this.qmlFile, meta[2][1].info),
                        })
                    } else {
                        this.assignProperties.push({
                            name: name,
                            value: meta[2],
                        })
                    }
                }
            } 
        } else if (meta[1][0] === "dot"){
            let name = meta[1].slice(1).join('.')
            this.assignProperties.push({
                name: name,
                value: meta[2],
            })
        } else {
            let name = meta[1]
            if(name.slice(0, 2) === 'on'){
                let signalName = name.slice(2)
                signalName = signalName[0].toLowerCase() + signalName.slice(1)
                this.connectedSignals.push({
                    slotName: name,
                    args: [], // Нужно отследить количество аргументов
                    source: meta[2],
                })
            } else {
                if(name === 'id'){
                    this.id = meta[2][1][1]
                    this.qmlFile.context[this.id]= this
                } else {
                    if(meta[2][1][0] === 'qmlelem'){
                        //console.log('=====', name)
                        this.assignProperties.push({
                            name: name,//.join('.'),
                            value: new Instruction(null, '', meta[2][1][1], meta[2][1][3], meta[2][1][2], this.qmlFile, meta[2][1].info),
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
    qmlobj(meta){
        this.children.push(new Instruction(this, '', meta[1].slice(1).join('.'), meta[2], null, this.qmlFile, meta.info)) 
    }
    qmlenumdef(meta){
        //Enums[meta[1]] = meta[2]
    }

    resolve(name, thisKey){
        let res = this.resolveInner(name, thisKey)
        if(res) {
            return res
        } else {
            let recursive = (result = [], name, target)=>{
                for(let key in target){
                    result.push(key)
                    if(name === key) return result.join('.')

                    if((typeof target[key] === 'object' && typeof target[key] !== 'function' && !(target[key] instanceof QmlFile || target[key] instanceof JSFile))){
                        if(recursive(result, name, target[key])) {
                            // console.log(target[key][name])
                            if(target[key][name] instanceof JSFile || target[key][name] instanceof QmlFile){
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
    
    resolveInner(name, thisKey){
        for(let obj of this.assignProperties){
            let typeInfo = obj.type ? this.getTypeInfo(obj.type) : {}
            if(name === obj.name) return {
                // obj: obj.value instanceof Instruction ? obj.value : typeInfo.type, // ====
                source: `${thisKey}.${name}`,
            }
        }
        for(let obj of this.defineProperties){
            let typeInfo = obj.type ? this.getTypeInfo(obj.type) : {}
            if(name === obj.name) return {
                // obj: obj.value instanceof Instruction ? obj.value : typeInfo.type, // ====
                source: `${thisKey}.${name}`,
            }
        }
        for(let obj of this.defineSignals){
            if(name === obj.name) return {
                // obj: obj.source, // ====
                source: `${thisKey}.${name}`,
            }
        }
        for(let obj of this.defineMethods){
            if(name === obj.name) return {
                // obj: obj.source, // ====
                source: `${thisKey}.${name}`,
            }
        }

        let typeInfo = null
        try {
            typeInfo = this.getTypeInfo(this.extends)
        } catch {}

        if(typeInfo){
            if(typeInfo.type instanceof QmlFile) {
                return typeInfo.type.instruction.resolve(name)
            } else {
                let obj = new typeInfo.type()
                if(name in typeInfo.type.meta || name in obj){
                    return {
                        // obj: typeInfo.type, // ====
                        source: `${thisKey}.${name}`,
                    }
                }
            }
        }
    }

    prepare(tree, stat = {isCompute:false, thisKey: 'self', value: '', local: []}){ 
        if(tree){
            switch(tree[0]){
                case 'return': {
                    stat.isCompute = true
                    stat.value += 'return '
                    this.prepare(tree[1], stat)
                    stat.value += ';'
                    return stat
                }
                case 'num': {
                    stat.value += tree[1]
                    return stat
                }
                case 'string': {
                    stat.value += `\`${tree[1].replaceAll('\u005C', '\u005C\u005C')}\``
                    return stat
                }
                case 'assign': {
                    this.prepare(tree[2], stat)
                    stat.value += `=`
                    this.prepare(tree[3], stat)
                    stat.value += `;`
                    return stat
                }
                case 'name': {
                    for(let local of stat.local){
                        if(local.indexOf(tree[1]) >= 0){
                            stat.value += tree[1]
                            if(stat.wasDot){
                                stat.dotObj = null
                            }
                            return stat
                        }
                    }

                    if(this.qmlFile.context[tree[1]]) {  // context !!!
                        stat.isCompute = true
                        stat.value += `__context['${tree[1]}']`
                        stat.dotObj = this.qmlFile.context[tree[1]]
                        
                        return stat
                    }

                    if(Singletons[tree[1]]) {
                        stat.value += tree[1]
                        if(stat.wasDot){
                            stat.dotObj = Singletons[tree[1]]
                        }
                        return stat
                    }

                    if(tree[1] === 'parent') {
                        if(stat.wasDot){
                            if(!this.parent || this.parent.extends === 'Component'){
                                stat.dotObj = null
                            } else {
                                stat.dotObj = this.parent
                            }
                            
                        }
                        stat.isCompute = true
                        stat.value += stat.thisKey + '.' +tree[1]
                        return stat
                    }

                    try {               
                        let obj = eval(tree[1])
                        stat.value += tree[1]
                        stat.dotObj = obj
                        return stat
                    } catch {}

                    let path = this.resolve(tree[1], stat.thisKey)

                    if(path){
                        stat.dotObj = path.obj
                        stat.value += path.source
                    } else {
                        console.log(`${this.qmlFile.fileName}:${tree.info.line+1}:${tree.info.col+1}: warning: ${tree[1]} is not founded`)
                        stat.value += stat.thisKey + '.' +tree[1]
                    }
                    
                    return stat
                }
                case 'dot': {
                    stat.isCompute = true
                    stat.wasDot = true
                    this.prepare(tree[1], stat)
                    stat.value += '.' + tree[2] 

                    if(stat.dotObj){
                        let path = ''
                        if(stat.dotObj instanceof QmlFile){
                            path = stat.dotObj.instruction.resolve(tree[2])
                            if(path) stat.dotObj = path.obj
                        } else if(stat.dotObj instanceof Instruction){
                            path = stat.dotObj.resolve(tree[2])
                            if(path) stat.dotObj = path.obj
                        } else if(typeof stat.dotObj === "function" && stat.dotObj.meta){
                            let obj = new stat.dotObj()
                            if(tree[2] in obj || tree[2] in stat.dotObj){
                                stat.dotObj = null
                                path = obj
                            } else if(tree[2] in stat.dotObj.meta){
                                path = this.resolve(stat.dotObj.meta[tree[2]], stat.thisKey)
                                if(path) stat.dotObj = path.obj
                            }
                            // if(tree[2] in stat.dotObj) stat.dotObj = path.obj
                        } else if(tree[2] in stat.dotObj){
                            path = stat.dotObj[tree[2]]
                            stat.dotObj = typeof stat.dotObj[tree[2]] === 'object' ? stat.dotObj[tree[2]] : null
                        } else {
                            path = null
                            stat.dotObj = null
                        }
                        if(!path){
                            console.log(`${this.qmlFile.fileName}:${tree.info.line+1}:${tree.info.col+1}: warning: ${tree[2]} is not founded`)
                        }
                    }
                    stat.wasDot = false
                    return stat
                }
                case 'call': {
                    this.prepare(tree[1], stat)
                    stat.value += '('
                    for(let i = 0; i < tree[2].length; i++){
                        this.prepare(tree[2][i], stat)
                        if(i < tree[2].length-1) stat.value += ','
                    }
                    stat.value += ')\n'

                    stat.dotObj = null
                    
                    return stat
                }
                case 'conditional': {
                    stat.isCompute = true
                    stat.value += '('
                    this.prepare(tree[1], stat)
                    stat.value += ' ? '
                    this.prepare(tree[2], stat)
                    stat.value += ' : '
                    this.prepare(tree[3], stat)
                    stat.value += ')'
                    return stat
                }
                case 'binary': {
                    stat.isCompute = true
                    stat.value += `(`
                    this.prepare(tree[2], stat)
                    stat.value += ` ${tree[1]} `
                    this.prepare(tree[3], stat)
                    stat.value += `)`
                    stat.value += `\n`
                    return stat
                }
                case 'let': {
                    let name = tree[1][0][0]
                    let local = []
                    if(stat.local.length){
                        local = stat.local[stat.local.length-1]
                        local.push(name)
                    } else {
                        local.push(name)
                        stat.local.push(local)
                    }

                    stat.value += `let ${name}`
                    if(tree[1][0][1]){
                        stat.value += `=`
                        this.prepare(tree[1][0][1], stat)
                    }
                    stat.value += `;`

                    return stat
                }
                case 'var': {
                    let name = tree[1][0][0]
                    let local = []
                    if(stat.local.length){
                        local = stat.local[stat.local.length-1]
                        local.push(name)
                    } else {
                        local.push(name)
                        stat.local.push(local)
                    }

                    stat.value += `var ${name}`
                    if(tree[1][0][1]){
                        stat.value += `=`
                        this.prepare(tree[1][0][1], stat)
                    }
                    stat.value += `;`
                    
                    return stat
                }
                case 'const': {
                    let name = tree[1][0][0]
                    let local = []
                    if(stat.local.length){
                        local = stat.local[stat.local.length-1]
                        local.push(name)
                    } else {
                        local.push(name)
                        stat.local.push(local)
                    }

                    stat.value += `const ${name}`
                    if(tree[1][0][1]){
                        stat.value += `=`
                        this.prepare(tree[1][0][1], stat)
                    }
                    stat.value += `;`

                    return stat
                }
                case 'unary-prefix': {
                    if(tree[1] === 'typeof' || tree[1] === 'delete'){
                        stat.value += tree[1] + ' '
                    } else{
                        stat.value += tree[1]
                    }
                    
                    this.prepare(tree[2], stat)  
        
                    stat.value += '\n'
                    return stat
                }
                case 'unary-postfix': {
                    this.prepare(tree[2], stat)
                    stat.value += tree[1]
                    stat.value += '\n'
                    return stat
                }
                case 'if': {
                    stat.value += 'if('
                    this.prepare(tree[1], stat)
                    stat.value += '){'
                    this.prepare(tree[2], stat)
                    stat.value += '}'
                    if(tree[3]){
                        stat.value += 'else{'
                        this.prepare(tree[3], stat)
                        stat.value += '}'
                    }
                    
                    return stat
                }
                case 'while': {
                    stat.isCompute = true
                    stat.value += `while(`
                    this.prepare(tree[1], stat)
                    stat.value += `){`
                    this.prepare(tree[2], stat)
                    stat.value += `};`
                    return stat
                }
                case 'for': {
                    stat.isCompute = true
                    stat.value += `for(`
                    this.prepare(tree[1], stat)
                    this.prepare(tree[2], stat)
                    stat.value += `;`
                    this.prepare(tree[3], stat)
                    if(stat.value[stat.value.length-1] === ';') stat.value.pop()
                    stat.value += `){`
                    this.prepare(tree[4], stat)
                    stat.value += `};`
                    return stat
                }
                case 'for-of': {
                    stat.isCompute = true
                    stat.value += `for(`
                    this.prepare(tree[1], stat)
                    stat.value = stat.value.slice(0, stat.value.length - 1)
                    stat.value += ` of `
                    this.prepare(tree[3], stat)
                    stat.value += `){`
                    this.prepare(tree[4], stat)
                    stat.value += `};`
                    return stat
                }
                case 'for-in': {
                    stat.isCompute = true
                    stat.value += `for(`
                    this.prepare(tree[1], stat)
                    stat.value = stat.value.slice(0, stat.value.length - 1)
                    stat.value += ` in `
                    this.prepare(tree[3], stat)
                    stat.value += `){`
                    this.prepare(tree[4], stat)
                    stat.value += `};`
                    return stat
                }
                case 'function': {
                    let local = []

                    stat.value += `function`
                    if(tree[1]){
                        if(stat.local.length){
                            stat.local[stat.local.length-1].push(tree[1])
                        } else {
                            stat.local.push([tree[1]])
                        }
                        stat.value += ` ${tree[1]}`
                    }

                    stat.local.push(local)

                    stat.value += `(`
                    for(let i = 0; i < tree[2].length; i++){
                        stat.value += tree[2][i]
                        local.push(tree[2][i])
                        if(i < tree[2].length-1) stat.value += ','
                    }
                    stat.value += `){`
                    this.prepare(tree[3], stat)
                    stat.value += `}`

                    let index = stat.local.indexOf(local)
                    if(index >= 0) stat.local.splice(index, 1)

                    return stat
                }
                case 'defun': {
                    let local = []
                    stat.value += tree[1] + '('
                    for(let i = 0; i < tree[2].length; i++){
                        stat.value += tree[2][i]
                        local.push(tree[2][i])
                        if(i < tree[2].length - 1) stat.value += ','
                    }
                    
                    stat.value += ')'
                    stat.value += '{'
                    stat.local.push(local)
                    this.prepare(tree[3], stat)
                    let index = stat.local.indexOf(local)
                    if(index >= 0) stat.local.splice(index, 1)
                    stat.value += '}'
                    return stat
                }
                case 'object': {

                    stat.value += `{`
                    for(let d of tree[1]){
                        stat.value += `${d[0]}:`
                        this.prepare(d[1], stat)
                        stat.value += `,`
                    }
                    stat.value += `}`
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
                    stat.value += ` new `
                    this.prepare(tree[1], stat)
                    stat.value += `(`
                    for(let i = 0; i < tree[2].length; i++){
                        this.prepare(tree[2][i], stat)
                        if(i < tree[2].length-1) stat.value += ','
                    }
                    stat.value += `)`
                    return stat
                }
                case 'sub': {
                    this.prepare(tree[1], stat)
                    stat.value += `[`
                    this.prepare(tree[2], stat)
                    stat.value += `]\n`
                    return stat
                }
                case 'array': {
                    stat.value += `[`
                    for(let i = 0; i < tree[1].length; i++){
                        this.prepare(tree[1][i], stat)
                        if(i < tree[1].length-1) stat.value += ','
                    }
                    stat.value += `]`
                    return stat
                }
                case 'regexp': {
                    stat.value += `/${tree[1]}/${tree[2]}`
                    return
                }
                case 'switch': {
                    stat.value += `switch(`
                    this.prepare(tree[1], stat)
                    stat.value += `){`
                    for(let i = 0; i < tree[2].length; i++){
                        if(tree[2][i][0] === null){
                            stat.value += `default: `
                            this.prepare(tree[2][i][1], stat)
                        } else {
                            stat.value += `case `
                            this.prepare(tree[2][i][0], stat)
                            stat.value += `:`
                            this.prepare(tree[2][i][1], stat)
                            
                        }
                        
                    }
                    stat.value += `}`
                    return stat
                }
                case 'seq': {
                    stat.value += `(`
                    for(let i = 1; i < tree.length; i++){
                        this.prepare(tree[i], stat)
                        if(i < tree.length-1) stat.value += ','
                    }
                    stat.value += `)`
                    return stat
                }
                case 'try': {
                    stat.value += `try{`
                    this.prepare(tree[1], stat)
                    if(tree[2][0]){
                        stat.ignore += tree[2][0]
                        stat.value += `}catch(${tree[2][0]}){`
                        this.prepare(tree[2][1], stat)
                    } else {
                        stat.value += `}catch{`
                        this.prepare(tree[2][1], stat)
                    }
                    stat.value += `}`
                    if(tree[3] && tree[3].length){
                        stat.value += `finally{`
                        this.prepare(tree[3], stat)
                        stat.value += `}`
                    }
        
                    break
                }
                case 'break': {
                    return 'break;'
                }
                case 'continue': {
                    return 'continue;'
                }
                case 'qmlaliasdef': {
                    stat.isCompute = true
                    let obj = null
                    let path = ''
                    for(let p of tree.info){
                        if(!p.value) continue

                        if(obj){
                            stat.value += '.'
                            let isExists = obj.resolve(p.value)
                            if(!isExists){
                                console.log(`${this.qmlFile.fileName}:${p.line+1}:${p.col+1}: warning: ${p.value} is not founded in ${path}`)
                            }
        
                            stat.value += p.value
                            path += '.' + p.value
                        } else {
                            path += p.value
                            if(this.qmlFile.context[p.value]) {  // context !!!
                                stat.value += `__context['${p.value}']`
                                obj = this.qmlFile.context[p.value]
                                continue
                            }
                            if(p.value === 'parent') {
                                stat.value += stat.thisKey + '.' +p.value
                                continue
                            }
        
                            let isExists = this.resolve(p.value, stat.thisKey)
                            if(!isExists){
                                console.log(`${this.qmlFile.fileName}:${p.line+1}:${p.col+1}: warning: ${p.value} is not founded`)
                            }
        
                            stat.value += stat.thisKey + '.' + p.value 
                        }
                        
                    }
                    return stat
                }
                default: {
                    // console.log('======', this.qmlFile.filename, Object.keys(tree))
                    for(let t of tree){
                        this.prepare(t, stat)
                        stat.value += '\n'
                    }
                    return stat

                    console.log('default'); return ''
                    
                    if(stat.type !== 'QAlias'){
                        for(let t of tree){
                            this.prepare(t, compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
                        }
                    } else {
                        stat.compute = true
                        if(tree.length === 1){
                            if(compiledFile.context[tree[0]]){
                                stat.value.push(`inCtx.get('${tree[0]}')`)
                            } else {
                                stat.value.push(`${currentInstructions.name}.getStatement('${tree[0]}')`)
                            }
                        } else if(tree.length === 2){
                            if(compiledFile.context[tree[0]]){
                                if(tree[1]){
                                    let path = tree[1].split('.')
                                    for(let i = 0; i < path.length; i++){
                                        path[i] = `getStatement('${path[i]}')`
                                    }
                                    stat.value.push(`inCtx.get('${tree[0]}').${path.join('.')}`)
                                } else {
                                    stat.value.push(`inCtx.get('${tree[0]}')`)
                                }
                                
                            } else {
                                if(tree[1]){
                                    let path = tree[1].split('.')
                                    for(let i = 0; i < path.length; i++){
                                        path[i] = `getStatement('${path[i]}')`
                                    }
                                    stat.value.push(`${currentInstructions.name}.getStatement('${tree[0]}').${path.join('.')}`)
                                } else {
                                    stat.value.push(`${currentInstructions.name}.getStatement('${tree[0]}')`)
                                }
                                
                            }
                        }
                    }
                    
                    break
                }
            }
        }
    }

    getTypeInfo(type){
        let found = false
        let _type = type
        let _path = ''

        if(SingleFiles[type]){
            found = true
            _path = type
            _type = SingleFiles[type]
        } else if(this.qmlFile.moduleName && JQModules[this.qmlFile.moduleName][type]) {
            found = true
            _path = `JQModules.${this.qmlFile.moduleName}.${type}`
            _type = JQModules[this.qmlFile.moduleName][type]
        } else if(JQModules.QtQml[type]) {
            found = true
            _path = `JQModules.QtQml.${type}`
            _type = JQModules.QtQml[type]
        } else if(JQModules.QtQml.Models[type]) {
            found = true
            _path = `JQModules.QtQml.Models.${type}`
            _type = JQModules.QtQml.Models[type]
        } else {
            for(let i = this.qmlFile.imports.length - 1; i >= 0; i--){
                let path = this.qmlFile.imports[i].path
                let as = this.qmlFile.imports[i].as
                let version = this.qmlFile.imports[i].version
                if(as && type.indexOf(as) >= 0){
                    _path = 'JQModules.' + type.replace(as, path)
                } else {
                    _path = 'JQModules.' + path + '.' + type
                }

                if(!found && version){
                    try {
                        _type = eval(_path+'_v'+version)
                        if(_type){
                            _path += '_v'+version
                            found = true
                            break
                        }
                    } catch {}
                }
                if(!found){
                    let qmlFile = null
                    try {
                        qmlFile = eval(_path)
                    } catch {}

                    if(qmlFile){
                        if(qmlFile instanceof QmlFile){
                            throw `${this.qmlFile.fileName}:${this.info.line+1}:${this.info.col+1}: error: ${type} version ${version} does not exists in ${path}`
                        } else {
                            _type = qmlFile
                            found = true
                            break
                        }
                    }
                }
            }
        }

        if(found){
            return {
                path: _path,
                type: _type
            }
        } else {
            throw `${this.qmlFile.fileName}:${this.info.line+1}:${this.info.col+1}: error: ${type} is not founded`
        }
    }

    getSignalArgs(name){
        for(let defineSignal of this.defineSignals){
            if(defineSignal.name === name){
                return defineSignal.args
            }
        }

        try {
            let typeInfo = this.getTypeInfo(this.extends)
            if(typeInfo.type instanceof QmlFile){
                return typeInfo.type.instruction.getSignalArgs(name)
            } else {
                return typeInfo.type.meta[name].args ? typeInfo.type.meta[name].args : []
            }
        } catch {
            
        }
        
    }

    toCode(isRoot = false){
        let code = []
        let typeInfo = this.getTypeInfo(this.extends)
        let typeBase = typeInfo.type
        while(typeBase instanceof QmlFile){
            typeBase = typeBase.instruction.getTypeInfo(typeBase.instruction.extends).typeBase
        }

        if(this.qmlFile.singleton && isRoot){
            code.push(`const ${this.className} = (class extends ${typeInfo.path} {`)
        } else {
            code.push(`class ${this.className} extends ${typeInfo.path} {`)
        }
        
        if(this.defineProperties.length || this.defineSignals.length){
            code.push(`static meta = Object.assign({}, ${typeInfo.path}.meta, {`)
            for(let defineProperty of this.defineProperties){
                let _typeInfo = this.getTypeInfo(defineProperty.type)

                if(typeof defineProperty.value === 'object'){
                    code.push(`${defineProperty.name}:{type:${_typeInfo.path}, signalName:'${defineProperty.signalName}'},`)
                    continue
                }

                if(typeof defineProperty.value === 'string'){
                    code.push(`${defineProperty.name}:{type:${_typeInfo.path}, value:'${defineProperty.value}', signalName:'${defineProperty.signalName}'},`)
                } else {
                    code.push(`${defineProperty.name}:{type:${_typeInfo.path}, value:${defineProperty.value}, signalName:'${defineProperty.signalName}'},`)
                }
                
            }
            for(let defineSignal of this.defineSignals){
                code.push(`${defineSignal.name}:{type:JQModules.QtQml.Signal, slotName:'${defineSignal.slotName}', args:[${defineSignal.args.join(',')}]},`)
            }
            code.push('})')
        }

        code.push('static create(...args){')
        code.push('let self = super.create(...args)')
        if(isRoot) {
            code.push(`let __context={}`) // context !!!
            code.push(`let __updateList=[]`) // property update !!!
        }
        if(this.id) code.push(`__context['${this.id}']=self`)  // context !!!

        for(let assignProperty of this.assignProperties){
            if(assignProperty.value instanceof Instruction) {
                code.push(`self.${assignProperty.name}=(${assignProperty.value.toCode(false)}).create()`)
            } else {
                let stat = this.prepare(assignProperty.value, {isCompute:false, thisKey: 'self', value:'', local:[]})
                if(stat.isCompute){
                    code.push(`self.${assignProperty.name} = JQModules.Qt.binding(()=>{return ${stat.value}},__updateList)`)
                    // code.push(`__updateList.push(self.${assignProperty.name})`) // property update !!!
                } else {
                    code.push(`self.${assignProperty.name}=${stat.value}`)
                }
            }
            
            
        }
        for(let i = 0; i < this.children.length; i++){
            if(this.extends === 'Component'){
                code.push(`self.component=` + this.children[i].toCode())
            } else {
                if(typeBase === JQModules.QtQuick.Flickable){
                    code.push(`let child${i}=(` + this.children[i].toCode() + ').create(self.contentItem)')
                } else {
                    code.push(`let child${i}=(` + this.children[i].toCode() + ').create(self)')
                }
                
            }
            
        }
        if(isRoot) {
            code.push(`for(let property of __updateList){property.__update()}`) // property update !!!
            code.push(`self.__complete()`)
        }

        code.push('return self')
        code.push('}')

        let connectionsInfo = {}
        for(let defineMethod of this.defineMethods){
            if(defineMethod.name.slice(0, 2) === 'on'){
                let signalName = defineMethod.name[2].toLowerCase() + defineMethod.name.slice(3)
                connectionsInfo[signalName] = defineMethod.name
            }
            let stat = this.prepare(defineMethod.source, {isCompute:false, thisKey: 'this', value:'', local:[]})
            code.push(stat.value)
        }

        if(typeBase === JQModules.QtQml.Connections){
            code.push(`__connectionsInfo = ${JSON.stringify(connectionsInfo)}`)
        }
        

        for(let connectedSignal of this.connectedSignals){
            let names = connectedSignal.slotName.replaceAll("['", '').replaceAll("']", '').split('.')
            let signalName = ''
            if(names.length === 1){
                signalName = names[0][2].toLowerCase() + names[0].slice(3)
            } else {
                signalName = names[0] + '.' + names[1][2].toLowerCase() + names[1].slice(3)
            }
            // signalName = connectedSignal.slotName[2].toLowerCase() + connectedSignal.slotName.slice(3)
        
            let args = this.getSignalArgs(signalName)

            connectedSignal.args = []

            if(args)
            for(let arg of args){
                connectedSignal.args.push(arg.replaceAll('`', ''))
            }


            code.push(`${connectedSignal.slotName}(${connectedSignal.args.join(',')}){`)
            
            let stat = this.prepare(connectedSignal.source, {isCompute:false, thisKey: 'this', value:'', local:[connectedSignal.args]}) 
            code.push(stat.value)
            code.push(`}`)
        }

        if(this.qmlFile.singleton && isRoot){
            code.push(`}).create()`)
        } else {
            code.push(`}`)
        }
        
        return code.join('\n')
    }
}

class QmlFile {
    imports = []
    context = {}
    singleton = false
    fileName = ''
    moduleName = ''
    instruction = null
    updateList = []
    version = 1

    constructor(fileName, moduleName, version = 1){
        this.fileName = fileName
        this.version = version
        this.moduleName = moduleName
        let data = fs.readFileSync(fileName, {encoding:'utf8', flag:'r'})

        parser.parse.nowParsingFile = fileName

        let meta = parser.parse(data)

        if(meta[3]) this.qmlpragma(meta[3])
        this.qmlimport(meta[1])

        let className = fileName.split(/[\/\\]+/g).pop().replace('.qml', '')
        if(this.singleton) Singletons[className] = this

        if(meta[2] && meta[2][0] === 'qmlelem'){
            this.instruction = new Instruction(null, className, meta[2][1], meta[2][3], meta[2][2], this, meta[2].info)
        } else if(meta[2] && meta[2][0] === 'qmlobj'){
            this.instruction = new Instruction(null, className, meta[2][1].slice(1).join('.'), meta[2][2], null, this, meta[2].info)
        }
    }

    qmlpragma(meta){
        for(let pragma of meta){
            if(pragma[0] === 'qmlpragma'){
                if(pragma[1] === 'Singleton') {
                    this.singleton = true
                    
                }
            }
        }
    }

    qmlimport(meta){
        for(let imp of meta){
            if(imp[0] === 'qmlimport'){
                if(imp[1].indexOf('js') >= 0){
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

    toCode(){
        return this.instruction.toCode(true)
    }
}


class JSFile {
    fileName = ''
    moduleName = ''
    version = 1
    compiled = null

    constructor(fileName, moduleName, version = 1){
        this.fileName = fileName
        this.version = version
        this.moduleName = moduleName
        let data = fs.readFileSync(fileName, {encoding:'utf8', flag:'r'})

        parser.parse.nowParsingFile = fileName

        this.meta = parser.jsparse(data)
    }

    toCode(){
        return `function(){${this.meta.source}; return {${this.meta.exports.join(',')}}}()`
    }
}

let fullCode = []

if(config.dirs.length) console.log(`JQ: preparation of third party modules`)

let counter = {

}

for(let dirPath of config.dirs){
    let absolutePath = path.resolve(configDirPath, dirPath)
    let moduleName = ''
    let lines = fs.readFileSync(absolutePath + '/qmldir', {encoding:'utf8', flag:'r'}).replaceAll('\r', '').split('\n')
    let count = 0
    for(let line of lines){
        let params = line.split(' ')
        if(params.length === 2){
            let [type, name] = params
            if(type === 'module'){
                moduleName = name

                if(JQModules[name]) throw `module ${name} already exists`
                console.log(`    > ${name}`)

                JQModules[name] = {}
            }
        } else if(params.length === 3){
            count += 1
            let [className, version, filePath] = params

            version = parseFloat(version)
            let absoluteFilePath = path.resolve(absolutePath, filePath)
            let extension = filePath.split('.').pop()
            let file = null

            if(extension === 'qml'){
                file = new QmlFile(absoluteFilePath, moduleName, version)
            } else if(extension === 'js'){
                file = new JSFile(absoluteFilePath, moduleName, version)
            }
            
            if(JQModules[moduleName][className]) {
                if(JQModules[moduleName][className].version < file.version) JQModules[moduleName][className] = file
            } else {
                JQModules[moduleName][className] = file
            }
            JQModules[moduleName][className+'_v'+version] = file
        }  else if(params.length === 4){
            count += 1
            let [singleton, className, version, filePath] = params

            if(filePath.split('.').pop() !== 'qml') continue

            version = parseFloat(version)
            let absoluteFilePath = path.resolve(absolutePath, filePath)
            
            let qmlFile = new QmlFile(absoluteFilePath, moduleName, version)
            
            if(!qmlFile.singleton && singleton){
                throw `${absoluteFilePath}:1:1: error: pragma Singleton is not defined`
            }

            if(JQModules[moduleName][className]) {
                if(JQModules[moduleName][className].version < qmlFile.version) JQModules[moduleName][className] = qmlFile
            } else {
                JQModules[moduleName][className] = qmlFile
            }
            JQModules[moduleName][className+'_v'+version] = qmlFile
        }
    }

    counter[moduleName] = count
}

console.log(`JQ: preparation of single files`)
for(let fileName of getFiles(path.resolve(configDirPath, config.base))){
    let qmlFile = new QmlFile(fileName)
    SingleFiles[fileName.split(/[\/\\]+/g).pop().replace('.qml', '')] = qmlFile
}

if(config.dirs.length) console.log(`JQ: compilation of third party modules`)
for(let moduleName in JQModules){
    if(!BaseModules[moduleName]){
        console.log(`    > ${moduleName} (${counter[moduleName] + ' files'})`)
        fullCode.push(`JQModules.${moduleName}={},`)
    }
}

for(let moduleName in JQModules){
    let queue = {}
    for(let className in JQModules[moduleName]){
        let file = JQModules[moduleName][className]
        if(file instanceof QmlFile || file instanceof JSFile){
            if(file.singleton) {
                continue
            }
            if(JQModules[moduleName][className+'_v'+file.version]) {
                queue[className] = file
                continue
            }
            fullCode.push(`JQModules.${moduleName}.${className}=${file.toCode()}`)
        }
    }
    for(let className in queue){
        let file = queue[className]
        fullCode.push(`JQModules.${moduleName}.${className}=JQModules.${moduleName}.${className}_v${file.version}`)
    }
}

function getFiles (dir, _files){
    _files = _files || []
    let files = fs.readdirSync(dir)
    for (var i in files){
        let name = dir + '/' + files[i]
        if (!(fs.statSync(name).isDirectory())){
            if(path.extname(name) === '.qml') _files.push(name.replaceAll(/\\+/g, '/'))
        }
    }
    return _files
}

console.log(`JQ: compilation of single files`)

for(let className in SingleFiles){
    if(SingleFiles[className].singleton) {
        continue
    }
    console.log(`    > ${className}.qml`)
    // console.log(`        > ${SingleFiles[className].fileName}(5)`)
    fullCode.push(SingleFiles[className].toCode())
}

for(let className in Singletons){
    console.log(`    > ${className}.qml (Singleton)`)
    fullCode.push(Singletons[className].toCode())
}

if(config.entry){
    fullCode.push(`${config.entry.replaceAll('.qml', '')}.create(JQApplication.root)`)
}

if(config.output) fs.writeFileSync(path.resolve(configDirPath, config.output), fullCode.join('\n'))
