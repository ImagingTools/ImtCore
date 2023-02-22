const fs = require('fs')
const path = require('path')
const parser = require('./parser')
const crypto = require('crypto')
const esprima = require('./esprima')

const QML = [
    'Component',
    'Shortcut',
    'QtObject',
    'Item',
    'FocusScope',
    'Canvas',
    'MouseArea',
    'Rectangle',
    'Loader',
    'ListElement',
    'ListModel',
    'DropShadow',
    'Repeater',
    'Image',
    'Column',
    'Row',
    'Flickable',
    'ListView',
    'GridView',
    'Text',
    'Timer',
    'Animation',
    'PropertyAnimation',
    'PauseAnimation',
    'NumberAnimation',
    'FontLoader',
    'TextInput',
    'TextEdit',
    'IntValidator',
    'DoubleValidator',
    'RegularExpressionValidator',
    'RegExpValidator',
    'Href',
    'FileDialog',
    'Gradient',
    'GradientStop',
]

let source = process.argv[2]
let destination = process.argv[3]

function getFiles (dir, _files){
    _files = _files || []
    let files = fs.readdirSync(dir)
    for (var i in files){
        let name = dir + '/' + files[i]
        if (fs.statSync(name).isDirectory()){
            getFiles(name, _files)
        } else {
            if(path.extname(name) === '.qml') _files.push(name.replaceAll(/\\+/g, '/'))
        }
    }
    return _files
}

if(!source) source = `C:\\Users\\Артур\\Documents\\projects\\2023\\TEST\\web\\web\\src`
// if(!source) source = `C:\\Users\\Артур\\Documents\\projects\\2023\\РТС\\web\\web\\src`
// if(!source) source = `C:\\projects\\ImagingTools\\ItDevelopment\\Lisa\\Bin\\web\\src`
if(!destination) destination = source

if(!fs.existsSync(source + '/cache/')) fs.mkdirSync(source + '/cache/');

let files = getFiles(source)

let IDList = new Set()

function fillSignalParamsFromDefaultClass(name, params, className){
    if(className === 'Repeater') {
        if(name === 'itemAdded') params.push('item')
        if(name === 'itemRemoved') params.push('item')
    }
    if(className === 'ListModel') { 
        if(name === 'dataChanged') {
            params.push('topLeft')
            params.push('bottomRight')
            params.push('keyRoles')
        }

    }
}

function proxyJS(sourceOrig, currentName, instruction, ignoreList = []){
    let source = sourceOrig
    ignoreList.push('of')

    const ignore = new Set(ignoreList)
    const script = esprima.parseScript(source, { tokens: true, range: true }, (node)=>{
        if(node.type === 'VariableDeclarator' || node.type === 'FunctionExpression'){

            if(node.id){
                ignore.add(node.id.name)
            }
            if(node.init && node.init.params){
                for(let param of node.init.params){
                    ignore.add(param.name)
                }
            }
            if(node.params){
                for(let param of node.params){
                    ignore.add(param.name)
                }
            }
        }
        
    })
    const tokens = script.tokens
    const declarationsAll = script.body.filter(x => x.type === 'VariableDeclaration')
    
    const markers = tokens.sort((a, b) => { return b.range[0] - a.range[0] })
    
    for(let declarationAll of declarationsAll){
        for(declaration of declarationAll.declarations){
            if(declaration.type === 'VariableDeclarator' && declaration.id.type === 'Identifier'){
                ignore.add(declaration.id.name)
            }
        }
    }
    for(let i = 0; i < markers.length; i++){
        if(markers[i].type === 'Identifier'){
            try {
                if(eval(markers[i].value)) continue
            } catch (error) {
                
            }
            
            if(ignore.has(markers[i].value)) continue
            if(i + 1 < markers.length) {
                if(markers[i+1].type === 'Punctuator' && markers[i+1].value === '.') continue
                if(markers[i+1].type === 'Keyword' && (markers[i+1].value === 'var' || markers[i+1].value === 'let')) {
                    ignore.add(markers[i].value)
                    continue
                }
            }
            if(i - 1 > 0) {
                if(markers[i-1].type === 'Punctuator' && markers[i-1].value === ':' && markers[i+1].type === 'Punctuator' && (markers[i+1].value === ',' || markers[i+1].value === '{' )) continue
                // if(markers[i+1].type === 'Keyword' && (markers[i+1].value === 'var' || markers[i+1].value === 'let')) continue
            }

            let id = `this.$P0.${markers[i].value}`
            if(instruction.properties[markers[i].value] || instruction.propertiesNew[markers[i].value] || instruction.propertiesLazy[markers[i].value] ||
                instruction.propertiesLazyNew[markers[i].value] || instruction.propertiesQML[markers[i].value] || instruction.propertiesQMLNew[markers[i].value] || 
                instruction.propertiesAlias[markers[i].value] || instruction.propertiesSpecial[markers[i].value]) {
                    id = `this.${markers[i].value}`
                }
            if(instruction.propertiesAlias[markers[i].value] && currentName === markers[i].value) {
                    id = `this.$PI.${markers[i].value}`
                }
            if(instruction.id.has(`\`${markers[i].value}\``)) id = `this`
            if(markers[i].value === 'XMLHttpRequest') id = 'XMLHttpRequest'
            //const id = `this.$P.${markers[i].value}`
            const start = markers[i].range[0]
            const end = markers[i].range[1]
            source = source.slice(0, start) + id + source.slice(end)
        }
    }
    return source
    // console.log(source)
}

function getBaseStructure(){
    return {
        class: '',
        Singleton: false,
        _qmlName: '',
        id: new Set(),
        properties: {},
        propertiesAlias: {},
        propertiesSpecial: {},
        propertiesLazy: {},
        propertiesQML: {},
        propertiesNew: {},
        propertiesLazyNew: {},
        propertiesQMLNew: {},
        js: {},
        qml: [],
        methods: {},
        connectionSignals: [],
        defineSignals: {},
        children: [],
    }
}
function qmlpragma(m, instructions, file){
    for(let p of m){
        if(p[0] === 'qmlpragma'){
            if(p[1] === 'Singleton') {
                
                // instructions.SingletonName = name
                instructions.Singleton = true
            }
        }
    }
}
function qmlimport(m, instructions, file){
    for(let imp of m){
        if(imp[0] === 'qmlimport'){
            if(imp[1].indexOf('js') >= 0){
                jsName = imp[1]
                jsAs = imp[3]
                let path = file.split('/')
                path = path.slice(0, path.length-1).join('/') + '/' + jsName

                if(fs.existsSync(path)){
                    let content = fs.readFileSync(path, encoding='utf-8')
                    let meta = parser.jsparse(content)
                    
                    instructions.js[jsName] = meta.source.replaceAll(/(?<=[^\\])[`]/g, '\\`') + (jsAs ? `\r\nvar ${jsAs} = {${meta.exports.join(',')}}\r\n` : '')
                } else {
                    console.log(`WARNING ${path} does not exist`)
                }
                
            } else {
                qmlPath = imp[1]
                qmlAs = imp[3]
                instructions.qml.push({
                    path: qmlPath,
                    as: qmlAs,
                })
            }
        }
    }
}
let ignoreSingletons = new Set()
function qmlelem(m, instructions, file){
    let cls = m[1]
    let childInstructions = getBaseStructure()
    // if(QML.indexOf(m[1]) < 0){
    //     if(m[1][0] === 'dot'){
    //         let name = m[1][2]
    //         let as = m[1][1]
    //         for(let qml of instructions.qml){
    //             let childFile = [source,qml.path,name + '.qml'].join('/') 
    //             if(qml.as === as && fs.existsSync(childFile)){
    //                 let data = fs.readFileSync(childFile, {encoding:'utf8', flag:'r'})

    //                 data = data.replaceAll(/((?<![:\/])\/\/(.*?)\n)|(\/\*(.*?)\*\/)/gms, '\n')
    //                 parser.parse.nowParsingFile = file.replaceAll(/\\+/g, '/')
    //                 let meta = parser.parse(data)
                    
    //                 qmlimport(meta[1], childInstructions, childFile)
    //                 preCompile(meta[2][1], meta[2][3], meta[2][2], childInstructions, childFile)
    //                 cls = meta[2][1]
    //             }
    //         }
    //     } else {
    //         let name = m[1]
    //         let childFile = [source,name + '.qml'].join('/') 
    //         if(fs.existsSync(childFile)){
    //             let data = fs.readFileSync(childFile, {encoding:'utf8', flag:'r'})

    //             data = data.replaceAll(/((?<![:\/])\/\/(.*?)\n)|(\/\*(.*?)\*\/)/gms, '\n')
    //             parser.parse.nowParsingFile = file.replaceAll(/\\+/g, '/')
    //             let meta = parser.parse(data)
                
    //             if(meta[3]) {
    //                 qmlpragma(meta[3], childInstructions, childFile)
    //                 let name = childFile.split('/').pop().replaceAll('.qml', '')
    //                 if(childInstructions.Singleton === true) ignoreSingletons.add(name)
    //             }
    //             qmlimport(meta[1], childInstructions, childFile)
    //             preCompile(meta[2][1], meta[2][3], meta[2][2], childInstructions, childFile)
    //             cls = meta[2][1]
    //         } else {
    //             for(let qml of instructions.qml){
    //                 let childFile = [source,qml.path,name + '.qml'].join('/') 
    //                 if(fs.existsSync(childFile)){
    //                     let data = fs.readFileSync(childFile, {encoding:'utf8', flag:'r'})
    
    //                     data = data.replaceAll(/((?<![:\/])\/\/(.*?)\n)|(\/\*(.*?)\*\/)/gms, '\n')
    //                     parser.parse.nowParsingFile = file.replaceAll(/\\+/g, '/')
    //                     let meta = parser.parse(data)
                        
    //                     if(meta[3]) {
    //                         qmlpragma(meta[3], childInstructions, childFile)
    //                         let name = childFile.split('/').pop().replaceAll('.qml', '')
    //                         if(childInstructions.Singleton === true) ignoreSingletons.add(name)
    //                     }
    //                     qmlimport(meta[1], childInstructions, childFile)
    //                     preCompile(meta[2][1], meta[2][3], meta[2][2], childInstructions, childFile)
    //                     cls = meta[2][1]
    //                 }
    //             }
    //         }
    //     }
        
    // }
    preCompile(cls, m[3], m[2], childInstructions, file) 
    if(instructions.class === 'Component'){
        instructions.propertiesSpecial.createObject = childInstructions
    } else {
        instructions.children.push(childInstructions)
    }
    
    
}
function qmlsignaldef(m, instructions){
    let params = []
    for(let p of m[2]){
        params.push(`\`${p.name}\``)
    }
    let name = m[1]
    instructions.defineSignals[name] = params//.join(',')
}
function qmlmethod(m, instructions){
    let name = m[1]
    let params = m[2][2]//.join(',')
    let src = m[3].trimStart().trimEnd()
    let lpos = src.indexOf('{')
    let rpos = src.lastIndexOf('}')
    if(lpos >= 0 && rpos >= 0) src = src.slice(lpos+1, rpos)
    
    instructions.methods[name] = {
        params: params,
        source: src,
        sourceFull: `let $compileTemp=function(${params.join(',')}){${src}}`,
    }
}
function qmlaliasdef(m, instructions){
    let name = m[1]
    let val = m.slice(2).join('.')
    if(val[val.length-1] === '.') val = val.slice(0, val.length-1)
    instructions.propertiesAlias[name] = val
}
function qmlpropdef(m, instructions, file){
    try {
        let name = m[1]
        let type = m[2]
        if(type === 'Component'){
            parser.parse.nowParsingFile = file.replaceAll(/\\+/g, '/')
            let _meta = parser.parse(m[4].replaceAll('};', '}'))
            if(!_meta[2]) throw 1
            let propertyInstructions = getBaseStructure()
            preCompile(_meta[2][1], _meta[2][3], _meta[2][2], propertyInstructions) 
            instructions.propertiesSpecial[name] = propertyInstructions
        } else {
            parser.parse.nowParsingFile = file.replaceAll(/\\+/g, '/')
            let _meta = parser.parse(m[4].replaceAll('};', '}'))
            if(!_meta[2]) throw 1
            let propertyInstructions = getBaseStructure()
            preCompile(_meta[2][1], _meta[2][3], _meta[2][2], propertyInstructions) 
            instructions.propertiesQMLNew[name] = propertyInstructions
        }
        
    } catch(error) {
        let name = m[1]
        let type = m[2]
        let val = m[4] ? m[4].trimStart().trimEnd() : undefined
        if(val && val[val.length-1] === ';') val = val.slice(0, val.length-1)

        try {
            let cval = eval(val.replaceAll('\n','\\\n'))

            if(type === 'string' && cval === undefined) cval = "''"

            if(typeof cval === 'string') {
                instructions.propertiesNew[name] = val.replaceAll('\n','\\\n')
            } else {
                instructions.propertiesNew[name] = val
            }
        } catch (error) {
            if(val === undefined){
                if(type === 'string') val = "''"
                instructions.propertiesNew[name] = val
            } else {
                instructions.propertiesLazyNew[name] = val
            }
            
        }
    }
}
function qmlprop(m, instructions, file){
    if(m[2][0] === "block" || m[2][1][0] === "assign"){
        if(m[1][0] === "dot"){
            let signalCall = m[1][2]
            let signalName = signalCall.slice(2).split('')
            signalName[0] = signalName[0].toLowerCase()
            signalName = m[1][1] + '.' + signalName.join('')
            let src = m[3].trimStart().trimEnd()
            let lpos = src.indexOf('{')
            let rpos = src.lastIndexOf('}')
            if(lpos >= 0 && rpos >= 0) src = src.slice(lpos+1, rpos)
            instructions.connectionSignals.push({
                name: signalName,
                source: src,
                sourceFull: `let $compileTemp=function(){with(this.$s['${signalName}'].context){${src}}}`,
            })
        } else {
            let signalCall = m[1]
            let signalName = signalCall.slice(2).split('')
            signalName[0] = signalName[0].toLowerCase()
            signalName = signalName.join('')
            let src = m[3].trimStart().trimEnd()
            let lpos = src.indexOf('{')
            let rpos = src.lastIndexOf('}')
            if(lpos >= 0 && rpos >= 0) src = src.slice(lpos+1, rpos)
            instructions.connectionSignals.push({
                name: signalName,
                source: src,
                sourceFull: `let $compileTemp=function(){with(this.$s['${signalName}'].context){${src}}}`,
            })
        } 
    } else if (m[1][0] === "dot"){
        let name = m[1].slice(1).join('.')
        let val = m[3].trimStart().trimEnd()
        if(val[val.length-1] === '.') val = val.slice(0, val.length-1)
        if(val[val.length-1] === ';') val = val.slice(0, val.length-1)
        
        try {
            let cval = eval(val.replaceAll('\n','\\\n'))
            if(typeof cval === 'string') {
                if(name === 'anchors.margins'){
                    instructions.properties['anchors.leftMargin'] = val.replaceAll('\n','\\\n')
                    instructions.properties['anchors.rightMargin'] = val.replaceAll('\n','\\\n')
                    instructions.properties['anchors.topMargin'] = val.replaceAll('\n','\\\n')
                    instructions.properties['anchors.bottomMargin'] = val.replaceAll('\n','\\\n')
                } else {
                    instructions.properties[name] = val.replaceAll('\n','\\\n')
                }
                
            } else {
                if(name === 'anchors.margins'){
                    instructions.properties['anchors.leftMargin'] = val
                    instructions.properties['anchors.rightMargin'] = val
                    instructions.properties['anchors.topMargin'] = val
                    instructions.properties['anchors.bottomMargin'] = val
                } else {
                    instructions.properties[name] = val
                }
            }
        } catch (error) {
            if(name === 'anchors.margins'){
                instructions.propertiesLazy['anchors.leftMargin'] = val
                instructions.propertiesLazy['anchors.rightMargin'] = val
                instructions.propertiesLazy['anchors.topMargin'] = val
                instructions.propertiesLazy['anchors.bottomMargin'] = val
            } else {
                instructions.propertiesLazy[name] = val
            }
            
        }
    } else if(m[2][1][0] === "binary" || m[2][1][0] === "dot" || m[2][1][0] === "conditional"){
        let name = m[1]
        let val = m[3].trimStart().trimEnd()
        
        try {
            let cval = eval(val)
            if(typeof cval === 'string') {
                instructions.properties[name] = val
            } else {
                if((cval === undefined && val.indexOf('undefined') < 0) || isNaN(cval)){
                    instructions.propertiesLazy[name] = val
                } else {
                    instructions.properties[name] = val
                }
                
            }
        } catch (error) {
            instructions.propertiesLazy[name] = val
        }
    } else if(m[1] === "delegate"){
        try{
            parser.parse.nowParsingFile = file.replaceAll(/\\+/g, '/')
            let _meta = parser.parse(m[3])
            let delegateInstructions = getBaseStructure()
            preCompile(_meta[2][1], _meta[2][3], _meta[2][2], delegateInstructions, file) 
            instructions.propertiesSpecial.delegate = delegateInstructions
        } catch {
            instructions.propertiesLazy[m[1]] = m[3]
        }
        
    } else if(m[1] === "validator"){
        try {
            parser.parse.nowParsingFile = file.replaceAll(/\\+/g, '/')
            let _meta = parser.parse(m[3])
            let validatorInstructions = getBaseStructure()
            preCompile(_meta[2][1], _meta[2][3], _meta[2][2], validatorInstructions, file) 
            instructions.propertiesQML.validator = validatorInstructions
        } catch {
            instructions.propertiesLazy[m[1]] = m[3]
        }
    } else if(m[1] === "gradient"){
        try {
            parser.parse.nowParsingFile = file.replaceAll(/\\+/g, '/')
            let _meta = parser.parse(m[3])
            let gradientInstructions = getBaseStructure()
            preCompile(_meta[2][1], _meta[2][3], _meta[2][2], gradientInstructions, file) 
            instructions.propertiesQML.gradient = gradientInstructions
        } catch {
            instructions.propertiesLazy[m[1]] = m[3]
        }
    } else {
        try {
            let name = m[1]
            parser.parse.nowParsingFile = file.replaceAll(/\\+/g, '/')
            let _meta = parser.parse(m[3])
            if(!_meta[2]) throw 1
            let propertyInstructions = getBaseStructure()
            preCompile(_meta[2][1], _meta[2][3], _meta[2][2], propertyInstructions, file) 
            if(name === 'sourceComponent' && instructions.class === 'Loader'){
                instructions.propertiesSpecial[name] = propertyInstructions
            } else {
                instructions.propertiesQML[name] = propertyInstructions
            }
            
        } catch {
            let name = m[1]
            let val = m[3].trimStart().trimEnd()
            if(val[val.length-1] === ';') val = val.slice(0, val.length-1)

            if(name.slice(0, 2) === 'on'){
                let signalCall = name
                let signalName = signalCall.slice(2).split('')
                signalName[0] = signalName[0].toLowerCase()
                signalName = signalName.join('')
                let src = val
                instructions.connectionSignals.push({
                    name: signalName,
                    source: src,
                    sourceFull: `let $compileTemp=function(){with(this.$s['${signalName}'].context){${src}}}`,
                })
            } else {
                if(name === 'id'){
                    instructions[name].add(`\`${val}\``)
                    // instructions.id= val
                    IDList.add(val)
                    compiledFiles[file].IDList.add(val)
                } else {
                    try {
                        let cval = eval(val.replaceAll('\n','\\\n'))
                        if(typeof cval === 'string') {
                            instructions.properties[name] = val.replaceAll('\n','\\\n')
                        } else {
                            instructions.properties[name] = val
                        }
                    } catch (error) {
                        instructions.propertiesLazy[name] = val
                    }
                }
                
            }

            
        }
    }
}
function preCompile(cls, meta, on, instructions, file){
    if(file){
        let qmlName = file.split('/').pop()
        instructions._qmlName = qmlName
    }  
    

    instructions.class = cls[0] === 'dot' ? cls.slice(1).pop() : cls
    if(on) {
        instructions.properties.properties = on
    }
    for(let m of meta){
        if(m){
            if(m[0] === "qmlprop"){
                qmlprop(m, instructions, file)
            }
            if(m[0] === "qmlpropdef"){
                qmlpropdef(m, instructions, file)
            }
            if(m[0] === "qmlsignaldef"){
                qmlsignaldef(m, instructions)
            }
            if(m[0] === "qmlaliasdef"){
                qmlaliasdef(m, instructions)
            }
            if(m[0] === "qmlmethod"){
                qmlmethod(m, instructions)
            }
            if(m[0] === "qmlelem"){
                qmlelem(m, instructions, file)
            }
        }
    }
}

let compiledFiles = {}

for(let file of files){
    let data = fs.readFileSync(file, {encoding:'utf8', flag:'r'})

    let currentHash = crypto.createHash('md5').update(data).digest("hex")
    let fileHash = source + '/cache/' + crypto.createHash('md5').update(file).digest("hex")
    let fileSave = file.replaceAll('.qml', '.js')

    compiledFiles[file] = {
        'instructions': {},
        'code': '',
        'cache': false,
        'IDList': new Set()
    }

    // if(fs.existsSync(fileHash)){
    //     let savedHash = fs.readFileSync(fileHash, {encoding:'utf8', flag:'r'}).split('\n')

    //     if(currentHash === savedHash[0]){
    //         let savedData = fs.readFileSync(fileSave, {encoding:'utf8', flag:'r'})

    //         compiledFiles[file].IDList = new Set(savedData[1].split(','))
    //         for(let id of compiledFiles[file].IDList){
    //             IDList.add(id)
    //         }
    //         compiledFiles[file].code = savedData
    //         compiledFiles[file].cache = true
    //         let instructions = getBaseStructure()
    
    //         data = data.replaceAll(/((?<![:\/])\/\/(.*?)\n)|(\/\*(.*?)\*\/)/gms, '\n')
            
    //         parser.parse.nowParsingFile = file.replaceAll(/\\+/g, '/')
    //         let meta = parser.parse(data)
    //         if(meta[3]) qmlpragma(meta[3], instructions, file)
    //         compiledFiles[file].instructions = instructions
    //         continue
    //     } else {
    //         compiledFiles[file].currentHash = currentHash
    //         // fs.writeFile(fileHash, currentHash, function(error){
    //         //     if(error) throw error
    //         // })
    //     }
    // } else {
    //     compiledFiles[file].currentHash = currentHash
    //     // fs.writeFile(fileHash, currentHash, function(error){
    //     //     if(error) throw error
    //     // })
    // }

    
    let instructions = getBaseStructure()
    
    data = data.replaceAll(/((?<![:\/])\/\/(.*?)\n)|(\/\*(.*?)\*\/)/gms, '\n')
    
    parser.parse.nowParsingFile = file.replaceAll(/\\+/g, '/')
    let meta = parser.parse(data)
    if(meta[3]) qmlpragma(meta[3], instructions, file)
    qmlimport(meta[1], instructions, file)

    if(meta[2][0] === 'qmlelem'){
        preCompile(meta[2][1], meta[2][3], meta[2][2], instructions, file)
        compiledFiles[file].instructions = instructions
    }
    // console.log(instructions)
}


function proxyReplace(instructions){
    
    for(let name in instructions.propertiesLazy){
        instructions.propertiesLazy[name] = proxyJS(instructions.propertiesLazy[name], name, instructions)
    }
    for(let name in instructions.propertiesLazyNew){
        instructions.propertiesLazyNew[name] = proxyJS(instructions.propertiesLazyNew[name], name, instructions)
    }
    for(let name in instructions.propertiesAlias){
        instructions.propertiesAlias[name] = proxyJS(instructions.propertiesAlias[name], name, instructions)
    }
    
    for(let name in instructions.methods){
        // let temp = eval(instructions.methods[name].sourceFull)
        // fillMethodParamsFromDefaultClass(signalParams, instructions.class)
        instructions.methods[name].sourceFull = proxyJS(instructions.methods[name].sourceFull, name, instructions, instructions.methods[name].params).replaceAll('let $compileTemp=', '')
    }
    for(let signal of instructions.connectionSignals){
        let signalParams = []
        if(QML.indexOf(instructions.class) < 0)
        for(let path in compiledFiles){
            if(path.indexOf(instructions.class) >= 0){
                if(compiledFiles[path].instructions.defineSignals[signal.name]){
                    signalParams.push(...compiledFiles[path].instructions.defineSignals[signal.name])
                }
            }
        }
        if(instructions.defineSignals[signal.name]){
            signalParams.push(...instructions.defineSignals[signal.name])
        }
        for(let i = 0; i < signalParams.length; i++){
            signalParams[i] = signalParams[i].replaceAll('`', '')
        }
        fillSignalParamsFromDefaultClass(signal.name, signalParams, instructions.class)
        signal.sourceFull = proxyJS(signal.sourceFull, signal.name, instructions, signalParams).replaceAll('let $compileTemp=', '')
    }
    for(let name in instructions.propertiesQML){
        proxyReplace(instructions.propertiesQML[name])
    }
    for(let name in instructions.propertiesQMLNew){
        proxyReplace(instructions.propertiesQMLNew[name])
    }
    for(let name in instructions.propertiesSpecial){
        proxyReplace(instructions.propertiesSpecial[name])
    }
    for(let child of instructions.children){
        proxyReplace(child)
    }
}


for(file in compiledFiles){
    let name = file.split('/').pop().replaceAll('.qml', '')
    if(compiledFiles[file].instructions.Singleton === true){
        compiledFiles[file].instructions.id.add(`\`${name}\``)
        // compiledFiles[file].instructions.id = name
        IDList.add(name)
        compiledFiles[file].IDList.add(name)
    }

    if(compiledFiles[file].cache === true) continue
    // anchorsReplace(compiledFiles[file].instructions)

    
    proxyReplace(compiledFiles[file].instructions)
    
    // ProxyReplace(compiledFiles[file].instructions)
    // PropertyReplace(compiledFiles[file].instructions)
}

function compile(instructions, code, curr = '$root', prev = ''){
    for(let file in instructions.js){
        code.push(`${instructions.js[file]}`)
    }
    if(!prev){
        code.push(`let ${curr}=Core.cC(\`${instructions.class}\`, $args)`)

    } else {
        code.push(`let ${curr}=Core.cC(\`${instructions.class}\`, {parent: ${prev}})`)
    }

    if(instructions.class === 'Column' || instructions.class === 'Row' || instructions.class === 'Image' || instructions.class === 'Repeater' || instructions.class === 'Text'){
        if(instructions.properties.width) code.push(`${curr}.$widthAuto=false`)
        if(instructions.properties.height) code.push(`${curr}.$heightAuto=false`)
        if(instructions.propertiesLazy.width) code.push(`${curr}.$widthAuto=false`)
        if(instructions.propertiesLazy.height) code.push(`${curr}.$heightAuto=false`)

        if(instructions.propertiesLazy['anchors.fill']) {
            code.push(`${curr}.$widthAuto=false`)
            code.push(`${curr}.$heightAuto=false`)
        }
        if(instructions.propertiesLazy['anchors.left'] && instructions.propertiesLazy['anchors.right']) {
            code.push(`${curr}.$widthAuto=false`)
        }
        if(instructions.propertiesLazy['anchors.top'] && instructions.propertiesLazy['anchors.bottom']) {
            code.push(`${curr}.$heightAuto=false`)
        }
        
    }
    if(instructions.class === 'Image'){
        if(instructions.propertiesLazy['sourceSize.width']) {
            code.push(`${curr}.$sizeWidthAuto=false`)
        }
        if(instructions.propertiesLazy['sourceSize.height']) {
            code.push(`${curr}.$sizeHeightAuto=false`)
        }
    }
    if(instructions.class === 'Flickable' || instructions.class === 'ListView' || instructions.class === 'GridView' || instructions.class === 'Text' || instructions.class === 'TextInput' || instructions.class === 'TextEdit'){
        if(instructions.properties.contentWidth || instructions.propertiesLazy.contentWidth) code.push(`${curr}.$contentWidthAuto=false`)
        if(instructions.properties.contentHeight || instructions.propertiesLazy.contentHeight) code.push(`${curr}.$contentHeightAuto=false`)
    }
    
    
    if(instructions.class === 'Loader'){
        let xOverride = false
        let yOverride = false
        let widthOverride = false
        let heightOverride = false
        let clipOverride = false

        if(instructions.properties.x || instructions.propertiesLazy.x) xOverride = true
        if(instructions.properties.y || instructions.propertiesLazy.y) yOverride = true
        if(instructions.properties.width || instructions.propertiesLazy.width) widthOverride = true
        if(instructions.properties.height || instructions.propertiesLazy.height) heightOverride = true
        if(instructions.properties.clip || instructions.propertiesLazy.clip) clipOverride = true

        if(instructions.propertiesLazy['anchors.fill']) {
            xOverride = true
            yOverride = true
            widthOverride = true
            heightOverride = true
        }
        if(instructions.propertiesLazy['anchors.centerIn']) {
            xOverride = true
            yOverride = true
        }
        if(instructions.propertiesLazy['anchors.left'] && instructions.propertiesLazy['anchors.right']) {
            xOverride = true
            widthOverride = true
        }
        if(instructions.propertiesLazy['anchors.top'] && instructions.propertiesLazy['anchors.bottom']) {
            yOverride = true
            heightOverride = true
        }

        if(!xOverride) code.push(`${curr}.$xOverride=false`)
        if(!yOverride) code.push(`${curr}.$yOverride=false`)
        if(!widthOverride) code.push(`${curr}.$widthOverride=false`)
        if(!heightOverride) code.push(`${curr}.$heightOverride=false`)
        if(!clipOverride) code.push(`${curr}.$clipOverride=false`)
        // let overrideItemProperties = []
        // for(let prop in instructions.properties){
        //     overrideItemProperties.push(`\`${prop}\``)
        // }
        // for(let prop in instructions.propertiesNew){
        //     overrideItemProperties.push(`\`${prop}\``)
        // }
        // for(let prop in instructions.propertiesLazy){
        //     overrideItemProperties.push(`\`${prop}\``)
        // }
        // for(let prop in instructions.propertiesLazyNew){
        //     overrideItemProperties.push(`\`${prop}\``)
        // }

        // code.push(`${curr}.$overrideItemProperties=[${overrideItemProperties}]`)
    }
    
    code.push(`${curr}._qmlName='${instructions._qmlName}'`)

    if(instructions.id.size > 0) code.push(`${curr}.$sID(${Array.from(instructions.id).join(',')})`)
    // if(instructions.id) code.push(`${curr}.$sID('${instructions.id}')`)

    
    for(let prop in instructions.propertiesLazy){
        let val = instructions.propertiesLazy[prop]
        code.push(`${curr}.$sP(\`${prop}\`, function(){return ${val}}.bind(${curr}))`)
    }
    for(let prop in instructions.propertiesNew){
        let val = instructions.propertiesNew[prop]
        code.push(`${curr}.$cP(\`${prop}\`,${val})`)
    }
    for(let prop in instructions.propertiesLazyNew){
        let val = instructions.propertiesLazyNew[prop]
        code.push(`${curr}.$cP(\`${prop}\`,function(){return ${val}}.bind(${curr}))`)
    }
    for(let prop in instructions.propertiesAlias){
        let val = instructions.propertiesAlias[prop]
        code.push(`${curr}.$cA(\`${prop}\`,function(){return ${val}}.bind(${curr}),function(nVal){${val}=nVal}.bind(${curr}))`)
    }
    for(let prop in instructions.propertiesSpecial){
        let codeNew = []
        codeNew.push(`function($args){`)
        // codeNew.push(`let $LVL = Core.LVL++`)
        compile(instructions.propertiesSpecial[prop], codeNew)
        // codeNew.push(`$root.$tryComplete()`)
        codeNew.push(`return $root`)
        codeNew.push(`}`)
        let val = codeNew.join('\n')
        code.push(`${curr}.${prop}=${val}`)
    }

    for(let prop in instructions.propertiesQML){
        let codeNew = []
        codeNew.push(`function($args){`)
        // codeNew.push(`let $LVL = Core.LVL++`)
        compile(instructions.propertiesQML[prop], codeNew)
        // codeNew.push(`$root.$tryComplete()`)
        codeNew.push(`return $root`)
        codeNew.push(`}`)
        let val = codeNew.join('\n')
        code.push(`${curr}.${prop}=${val}({parent: ${curr}})`)
    }
    for(let prop in instructions.propertiesQMLNew){
        let codeNew = []
        codeNew.push(`function($args){`)
        // codeNew.push(`let $LVL = Core.LVL++`)
        compile(instructions.propertiesQMLNew[prop], codeNew)
        // codeNew.push(`$root.$tryComplete()`)
        codeNew.push(`return $root`)
        codeNew.push(`}`)
        let val = codeNew.join('\n')
        code.push(`${curr}.$cP(\`${prop}\`,${val}({parent: ${curr}}))`)
    }

    for(let name in instructions.methods){
        //code.push(`${curr}.${name}=function(${instructions.methods[name].params.join(',')}){let $args = {${instructions.methods[name].params.join(',')}};with(this)with($args)with(QML){${instructions.methods[name].source}}}.bind(${curr})`)
        code.push(`${curr}.${name}=${instructions.methods[name].sourceFull}.bind(${curr})`)
    }
    for(let name in instructions.defineSignals){
        if(instructions.defineSignals[name].length){
            code.push(`${curr}.$cS(\`${name}\`,${instructions.defineSignals[name].join(',')})`)
        } else {
            code.push(`${curr}.$cS(\`${name}\`)`)
        }
        
    }
    for(let signal of instructions.connectionSignals){
        //code.push(`${curr}.$s['${signal.name}'].connect(function(){with(this)with(QML)with(this.$s['${signal.name}'].context){${signal.source}}}.bind(${curr}))`)
        code.push(`${curr}.$s['${signal.name}'].connect(${signal.sourceFull}.bind(${curr}))`)
    }

    for(let prop in instructions.properties){
        let val = instructions.properties[prop]
        if(instructions.class === 'ListElement'){
            code.push(`${curr}.$cP(\`${prop}\`,${val})`)
        } else {
            code.push(`${curr}.${prop}=${val}`)
        }
        
    }

    let step = 0
    for(let child of instructions.children){
        compile(child, code, curr+'c'+step, curr)
        step++
    }

    
}

let jqml = []
let jqmlExports = []
let Singletons = []
for(file in compiledFiles){
    let name = file.split('/').pop().replaceAll('.qml', '')
    let instructions = compiledFiles[file].instructions
    if(instructions.Singleton === true && !ignoreSingletons.has(name)){
        Singletons.push(name)
    } else {
        jqmlExports.push(name)
    }

    if(compiledFiles[file].cache === true) {
        
    } else {
        let code = []
        code.push(`function ${name}($args){`)
        compile(instructions, code)
        code.push(`return $root`)
        code.push(`}`)

        compiledFiles[file].code = code.join('\n')
        fs.writeFile(file.replaceAll('.qml', '.js'), compiledFiles[file].code, function(error){
            if(error) throw error
        })

        let tempIDList = []
        for(let id of compiledFiles[file].IDList){
            tempIDList.push(id)
        }

        // fs.writeFile(source + '/cache/' + crypto.createHash('md5').update(file).digest("hex"), compiledFiles[file].currentHash /*+ '\n' + tempIDList.join(',')*/, function(error){
        //     if(error) throw error
        // })
    }
    
    
    jqml.push(compiledFiles[file].code)
    
}



let fullCode = (jqml.join('\n') + `\nCore.exports = {${jqmlExports.join(',')}}` + `\nCore.Singletons = {${Singletons.join(',')}}`).split('\n')
let i = 0
while(i < fullCode.length){
    fullCode[i] = fullCode[i].trimStart()
    if(fullCode[i]){
        i++
    } else {
        fullCode.splice(i, 1)
    }
}
fullCode = fullCode.join('\n')

fs.writeFile([destination, 'jqml.full.js'].join('/'), fullCode, function(error){
    if(error) throw error
})
