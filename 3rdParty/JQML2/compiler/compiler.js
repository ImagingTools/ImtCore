const fs = require('fs')
const path = require('path')
const parser = require('./parser')
const { listComponents } = require('../components/list')
const { Qt } = require('../utils/Qt')
const components = {}
for(let componentName of listComponents){
    const component = require(`../components/${componentName}`)[componentName]
    components[componentName] = component
}
const listProperties = require('../utils/properties')

const source = process.argv[2] || '../test/qml'// 'C:\\projects\\ImagingTools\\ItDevelopment\\Lisa\\Bin\\web\\src' // 
if(!source) throw 'error: source not specified'

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

const fileList = getFiles(path.resolve(__dirname, source))
const classList = []
for(let i = 0; i < fileList.length; i++){
    fileList[i] = fileList[i].replaceAll('/', '/').replaceAll('\\', '/')
    classList.push(path.relative(path.resolve(__dirname, source), fileList[i]).replaceAll('/','_').replaceAll('\\','_').replaceAll('.qml',''))
}
const compiledFiles = {}
// console.log('fileList: ', fileList)

UID = 0
function getBaseStructure(){
    UID++
    return {
        extends: undefined,
        className: '',
        // extendsClassName: '',
        extends: null,
        Singleton: false,
        fileName: '',
        // qmlFileName: '',
        // getQmlFileName: function(){
        //     let qmlName = this.fileName.split('/').pop()
        //     return qmlName
        // },
        getClassName: function(){
            return this.className
        },
        getExtendsClassName: function(){
            return this.extends ? this.extends.instructions.getClassName() : ''
        },
        id: '',
        properties: [],
        propertiesNames: [],
        js: {},
        qml: [],
        methods: {},
        connectionSignals: [],
        defineSignals: {},
        children: [],
        parent: null,
        name: `el${UID}`,
        UID: UID
    }
}

function qmlpragma(meta, instructions){
    for(let pragma of meta){
        if(pragma[0] === 'qmlpragma'){
            if(pragma[1] === 'Singleton') {
                instructions.Singleton = true
            }
        }
    }
}
function qmlimport(meta, compiledFile){
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
                compiledFile.imports.push({
                    path: imp[1],
                    as: imp[3],
                })
            }
        }
    }
}
// let ignoreSingletons = new Set()
function qmlelem(meta, instructions){
    let childInstructions = getBaseStructure()
    instructions.children.push(childInstructions)
    childInstructions.parent = instructions
    childInstructions.fileName = instructions.fileName
    childInstructions.compiledFile = instructions.compiledFile
   
    preCompile(meta[1], meta[3], meta[2], childInstructions) 


    // if(instructions.class === 'Component'){
    //     instructions.propertiesSpecial.createObject = childInstructions
    // } else {
    //     instructions.children.push(childInstructions)
    // }
    
    
}
function qmlsignaldef(meta, instructions){
    let params = []
    for(let p of meta[2]){
        params.push(`\`${p.name}\``)
    }
    instructions.defineSignals[meta[1]] = params//.join(',')
}
function qmlmethod(meta, instructions){
    instructions.methods[meta[1]] = {
        params: meta[1],
        val: meta[2],
    }
}
function qmlaliasdef(meta, instructions){
    instructions.propertiesNames.push(meta[1])
    instructions.properties.push({
        name: meta[1],
        type: 'QAlias',
        val: meta[meta.length - 1] === '.' ? meta.slice(2, meta.length - 1) : meta.slice(2),
        command: 'create',
    })
}
function qmlpropdef(meta, instructions){
    instructions.propertiesNames.push(meta[1])
    if(meta[3] && meta[3][1][0] === 'qmlelem'){
        let childInstructions = getBaseStructure()
        childInstructions.parent = instructions
        childInstructions.fileName = instructions.fileName
        childInstructions.compiledFile = instructions.compiledFile
    
        preCompile(meta[3][1][1], meta[3][1][3], meta[3][1][2], childInstructions) 

        let type = meta[2][0].toUpperCase()+meta[2].slice(1)

        instructions.properties.push({
            val: childInstructions,
            name: meta[1],
            type: listProperties['Q'+type] ? 'Q'+type : components[type] ? type : 'null', //temp
            command: 'create',
            isElement: true
        })
    } else {
        let type = meta[2][0].toUpperCase()+meta[2].slice(1)
        
        instructions.properties.push({
            name: meta[1],
            type: listProperties['Q'+type] ? 'Q'+type : components[type] ? type : 'null', //temp
            val: meta[3],
            command: 'create'
        })
    }
    
}
function qmlprop(meta, instructions){
    if(meta[2][0] === "block" || meta[2][1][0] === "assign"){
        if(meta[1][0] === "dot"){
            let name = meta[1].slice(1)

            if(name[1].slice(0, 2) === 'on'){
                let signalName = name[1].slice(2)
                signalName = signalName[0].toLowerCase() + signalName.slice(1)
                instructions.connectionSignals.push({
                    name: `${name[0]}.${signalName}`,
                    val: meta[2],
                })
            } else {
                instructions.propertiesNames.push(name.join('.'))
                if(meta[2][1][0] === 'qmlelem'){
                    let childInstructions = getBaseStructure()
                    childInstructions.parent = instructions
                    childInstructions.fileName = instructions.fileName
                
                    preCompile(meta[2][1][1], meta[2][1][3], meta[2][1][2], childInstructions) 

                    instructions.properties.push({
                        name: name,
                        val: childInstructions,
                        isElement: true
                    })
                }
                instructions.properties.push({
                    name: name.join('.'),
                    val: meta[2]
                })
            }
             
            
        } else {
            let name = meta[1]

            if(name.slice(0, 2) === 'on'){
                let signalName = name.slice(2)
                signalName = signalName[0].toLowerCase() + signalName.slice(1)
                instructions.connectionSignals.push({
                    name: signalName,
                    val: meta[2],
                })
            } else {
                instructions.propertiesNames.push(name)

                if(meta[2][1][0] === 'qmlelem'){
                    let childInstructions = getBaseStructure()
                    childInstructions.parent = instructions
                    childInstructions.fileName = instructions.fileName
                    childInstructions.compiledFile = instructions.compiledFile
                
                    preCompile(meta[2][1][1], meta[2][1][3], meta[2][1][2], childInstructions) 

                    instructions.properties.push({
                        name: name,
                        val: childInstructions,
                        isElement: true
                    })
                }

                instructions.properties.push({
                    name: name,
                    val: meta[2]
                })
            }
        } 
    } else if (meta[1][0] === "dot"){
        let name = meta[1].slice(1).join('.')
        instructions.propertiesNames.push(name)
        instructions.properties.push({
            name: name,
            val: meta[2]
        })
    } else {
        let name = meta[1]
        if(name.slice(0, 2) === 'on'){
            let signalName = name.slice(2)
            signalName = signalName[0].toLowerCase() + signalName.slice(1)
            instructions.connectionSignals.push({
                name: signalName,
                val: meta[2],
            })
        } else {
            if(name === 'id'){
                instructions.id = meta[2][1][1]
                compiledFiles[instructions.fileName].context[instructions.id] = instructions
            } else {
                instructions.propertiesNames.push(name)

                if(meta[2][1][0] === 'qmlelem'){
                    let childInstructions = getBaseStructure()
                    childInstructions.parent = instructions
                    childInstructions.fileName = instructions.fileName
                    childInstructions.compiledFile = instructions.compiledFile
                
                    preCompile(meta[2][1][1], meta[2][1][3], meta[2][1][2], childInstructions) 

                    instructions.properties.push({
                        name: name,
                        val: childInstructions,
                        isElement: true
                    })
                } else {
                    instructions.properties.push({
                        name: name,
                        val: meta[2]
                    })
                }

                
            }
        }
        
        
    }
}
function qmlobj(m, instructions, file){
    let cls = 'QtObject'
    let childInstructions = getBaseStructure()
   
    preCompile(cls, m[2], null, childInstructions, file) 
    instructions.propertiesQML[m[1]] = childInstructions
    
}
function preCompile(className, meta, on, instructions){
    className = className[0] === 'dot' ? className.slice(1).pop() : className
    if(listComponents.indexOf(className) >= 0){
        instructions.className = className
    } else {
        // let compiledFile = compiledFiles[instructions.fileName]
        // path.relative(path.resolve(__dirname, source), fileName).replaceAll('/','_').replaceAll('\\','_').replaceAll('.qml','')
        let pathName = []
        if(instructions.compiledFile.namespace) pathName.push(instructions.compiledFile.namespace)
        pathName.push(className)
        if(classList.indexOf(pathName.join('_')) >= 0){
            instructions.className = pathName.join('_')
        } else {
            let find = false
            for(let importObj of instructions.compiledFile.imports){
                pathName = []
                pathName.push(importObj.path)
                let fileName = instructions.fileName.replaceAll('/','_').replaceAll('\\','_').split('_')
                fileName.pop()
                fileName = fileName.join('/')

                let _pathName = path.relative(path.resolve(__dirname, source), path.resolve(fileName, pathName.join('/'))).replaceAll('/','_').replaceAll('\\','_')
                if(classList.indexOf(_pathName + '_' + className) >= 0){
                    instructions.className = _pathName + '_' + className
                    find = true
                    break
                } else {
                    // let test1 = path.resolve(__dirname, source)
                    // let test2 = pathName.join('/')
                    // let test3 = path.resolve(path.resolve(__dirname, source), pathName.join('/'))
                    _pathName = path.relative(path.resolve(__dirname, source), path.resolve(path.resolve(__dirname, source), pathName.join('/'))).replaceAll('/','_').replaceAll('\\','_')
                    if(classList.indexOf(_pathName + '_' + className) >= 0){
                        instructions.className = _pathName + '_' + className
                        find = true
                        break
                    }
                }
            }
            if(!find){
                console.log(`Warning: class ${className} not found into file ${instructions.fileName}`)
                for(let _className of classList){
                    if(_className === className || _className.split('_').pop() === className){
                        instructions.className = _className
                        find = true
                        break
                    }
                }
                if(!find) throw `Error: class ${className} not found into file ${instructions.fileName}`
            } 
            // instructions.className = ''
        }
        // instructions.className = path.relative(path.resolve(__dirname, source), instructions.fileName).replaceAll('/','_').replaceAll('\\','_').replaceAll('.qml','')
    }
    

    // if(on) {
    //     instructions.properties.properties = on
    // }
    for(let m of meta){
        if(m){
            if(m[0] === "qmlprop"){
                qmlprop(m, instructions)
            }
            if(m[0] === "qmlpropdef"){
                qmlpropdef(m, instructions)
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
                qmlelem(m, instructions)
            }
            // if(m[0] === "qmlobj"){
            //     qmlobj(m, instructions)
            // }
        }
    }
}

//preCompile

for(let fileName of fileList){
    UID = 0
    let data = fs.readFileSync(fileName, {encoding:'utf8', flag:'r'}).replaceAll(/((?<![:\/])\/\/(.*?)\n)|(\/\*(.*?)\*\/)/gms, '\n')
    let instructions = getBaseStructure()

    parser.parse.nowParsingFile = fileName
    let meta = parser.parse(data)
    
    let namespace = path.relative(path.resolve(__dirname, source), fileName).replaceAll('/', '_').replaceAll('\\', '_').split('_')
    namespace.pop()
    namespace = namespace.join('_')
    // let test3 = test2.replaceAll(/[_]{1}\w+.qml/g, '').replaceAll(/\w+.qml/g, '')
   
    compiledFiles[fileName] = {
        fileName: fileName,
        context: {},
        instructions: instructions,
        namespace: namespace,
        imports: [],
    }
    instructions.fileName = fileName
    instructions.compiledFile = compiledFiles[fileName]

    if(meta[3]) qmlpragma(meta[3], instructions)
    qmlimport(meta[1], compiledFiles[fileName])

    if(meta[2][0] === 'qmlelem'){
        preCompile(meta[2][1], meta[2][3], meta[2][2], instructions)
    }
}

function testName(name, currentInstructions){
    if(currentInstructions.getClassName() === 'MouseArea' && (name === 'mouse' || name === 'wheel')) return true

    let inExtends = false
    if(currentInstructions.extends) inExtends = testName(name, currentInstructions.extends.instructions)
    if(inExtends) return true
    
    let component = components[currentInstructions.getClassName()]

    if(component){
        if(name in component.prototype) return true
        while(component.defaultProperties){
        
            if(name in component.defaultProperties){
                return true
            }
            component = component.__proto__
        }
    }
    
    for(let prop of currentInstructions.properties){
        if(prop.name === name) return true
    }
    if(name in currentInstructions.defineSignals) return true
    if(name in currentInstructions.methods) return true
    return false
}

function testProperty(name, currentInstructions){
    name = name.replaceAll('Changed','')

    let inExtends = false
    if(currentInstructions.extends) inExtends = testProperty(name, currentInstructions.extends.instructions)
    if(inExtends) return true
    
    let component = components[currentInstructions.getClassName()]
    if(component)
    while(component.defaultProperties){
        if(name in component.defaultProperties){
            return true
        }
        component = component.__proto__
    }
    for(let prop of currentInstructions.properties){
        if(prop.name === name) return true
    }
    return false
}

function getSignalParams(name, currentInstructions){
    let component = components[currentInstructions.getClassName()]
    if(component)
    while(component.defaultSignals){
        if(name in component.defaultSignals){
            return component.defaultSignals[name].params
        }
        component = component.__proto__
    }
    if(name in currentInstructions.defineSignals) return currentInstructions.defineSignals[name]
    if(currentInstructions.extends) return getSignalParams(name, currentInstructions.extends.instructions)
    return []
}

function prepare(tree, compiledFile, currentInstructions, stat = null, propValue = false, assign = false, prevCommand = '', currentObj = {}){ 
    if(tree)
    switch(tree[0]){
        case 'return': {
            stat.return = true
            stat.value.push(`return `)
            prepare(tree[1], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push(`;`)
            break
        }
        case 'num': {
            stat.value.push(tree[1])
            break
        }
        case 'string': {
            stat.value.push(`\`${tree[1]}\``)
            break
        }
        case 'assign': {
            stat.compute = true
            prepare(tree[2], compiledFile, currentInstructions, stat, propValue, true, prevCommand, currentObj)
            if(stat.value[stat.value.length-1].indexOf('getStatement') >= 0){
                stat.value.push(`.reset(`)
                prepare(tree[3], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
                stat.value.push(`)`)
            } else {
                if(tree[1] === true) {
                    stat.value.push(`=`)
                } else {
                    if(tree[1]) stat.value.push(tree[1])
                    stat.value.push(`=`)
                }
                
                prepare(tree[3], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            }
            stat.value.push(`;`)
            break
        }
        case 'name': {
            if(tree[1] === 'parent') {
                stat.compute = true
                stat.value.push(`${currentInstructions.name}.parent`)
            } else if(tree[1] in compiledFile.context){
                stat.compute = true
                stat.value.push(`inCtx.get('${tree[1]}')`)
                currentObj.name = `inCtx.${tree[1]}`
            } else {
                if(stat.ignore.indexOf(tree[1]) >= 0 || stat.params.indexOf(tree[1]) >= 0 || tree[1] === 'true' || tree[1] === 'false' || tree[1] === 'this'){
                    stat.value.push(tree[1])
                } else if(testName(tree[1], currentInstructions)){
                    stat.compute = true
                    stat.value.push(`${currentInstructions.name}.${tree[1]}`)
                } else {
                    try {
                        if(!components[tree[1]]) eval(tree[1])
                        stat.value.push(tree[1])
                    } catch (error) {
                        if(tree[1] === 'XMLHttpRequest'){
                            stat.value.push(`XMLHttpRequest`)
                        } else if(tree[1] in Qt){
                            stat.compute = true
                            stat.value.push(`Qt.${tree[1]}`)
                        } else {
                            stat.compute = true
                            stat.value.push(`inCtx.get('${tree[1]}')`)
                        }
                    }
                }
                
            }
            break
        }
        case 'dot': {
            stat.compute = true
            prepare(tree[1], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push('.')
            stat.value.push(tree[2])
            break
        }
        case 'call': {
            stat.compute = true
            prepare(tree[1], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push('(')
            for(let i = 0; i < tree[2].length; i++){
                prepare(tree[2][i], compiledFile, currentInstructions, stat, propValue, assign, '', currentObj)
                if(i < tree[2].length-1) stat.value.push(',')
            }
            stat.value.push(')\n')
            break
        }
        case 'conditional': {
            stat.compute = true
            prepare(tree[1], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push(' ? ')
            prepare(tree[2], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push(' : ')
            prepare(tree[3], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            break
        }
        case 'binary': {
            stat.compute = true
            stat.value.push(`(`)
            prepare(tree[2], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push(` ${tree[1]} `)
            prepare(tree[3], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push(`)`)
            stat.value.push(`\n`)
            break
        }
        case 'let': {
            stat.compute = true
            stat.ignore.push(tree[1][0][0])
            stat.value.push(`let ${tree[1][0][0]}`)
            if(tree[1][0][1]){
                stat.value.push(`=`)
                prepare(tree[1][0][1], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            }
            stat.value.push(`;`)
            break
        }
        case 'var': {
            stat.compute = true
            stat.ignore.push(tree[1][0][0])
            stat.value.push(`var ${tree[1][0][0]}`)
            if(tree[1][0][1]){
                stat.value.push(`=`)
                prepare(tree[1][0][1], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            }
            stat.value.push(`;`)
            break
        }
        case 'unary-prefix': {
            if(tree[1] === 'typeof'){
                stat.value.push(`${tree[1]} `)
            } else {
                stat.value.push(tree[1])
            }
            
            prepare(tree[2], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)  

            stat.value.push('\n')
            break
        }
        case 'unary-postfix': {
            prepare(tree[2], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push(tree[1])

            stat.value.push('\n')
            break
        }
        case 'if': {
            stat.value.push('if(')
            prepare(tree[1], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push(')')
            stat.value.push('{')
            prepare(tree[2], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push('}')
            if(tree[3]){
                stat.value.push('else{')
                prepare(tree[3], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
                stat.value.push('}')
            }
            
            break
        }
        case 'while': {
            stat.compute = true
            stat.value.push(`while(`)
            prepare(tree[1], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push(`)`)
            stat.value.push(`{`)
            prepare(tree[2], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push(`};`)
            break
        }
        case 'for': {
            stat.compute = true
            stat.value.push(`for(`)
            prepare(tree[1], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            // stat.value.push(`;`)
            prepare(tree[2], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push(`;`)
            prepare(tree[3], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push(`){`)
            prepare(tree[4], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push(`};`)
            break
        }
        case 'for-of': {
            stat.compute = true
            stat.value.push(`for(`)
            prepare(tree[1], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value[stat.value.length-1] = stat.value[stat.value.length-1].replaceAll(';', '')
            stat.value.push(` of `)
            prepare(tree[3], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push(`){`)
            prepare(tree[4], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push(`};`)
            break
        }
        case 'for-in': {
            stat.compute = true
            stat.value.push(`for(`)
            prepare(tree[1], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value[stat.value.length-1] = stat.value[stat.value.length-1].replaceAll(';', '')
            // prepare(tree[2], compiledFile, currentInstructions, stat)
            stat.value.push(` in `)
            prepare(tree[3], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push(`){`)
            prepare(tree[4], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push(`};`)
            break
        }
        case 'function': {
            stat.compute = true
            stat.value.push(`function`)
            if(tree[1]){
                stat.ignore.push(tree[1])
                stat.value.push(` ${tree[1]}`)
            }
            stat.value.push(`(`)
            // for(let arg of tree[2]){
            //     prepare(arg, compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            // }
            for(let i = 0; i < tree[2].length; i++){
                stat.value.push(tree[2][i])
                stat.ignore.push(tree[2][i])
                if(i < tree[2].length-1) stat.value.push(',')
            }
            stat.value.push(`)`)
            stat.value.push(`{`)
            prepare(tree[3], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push(`}`)
            break
        }
        case 'defun': {
            stat.value.push(`function(`)
            for(let i = 0; i < tree[2].length; i++){
                stat.value.push(tree[2][i])
                stat.ignore.push(tree[2][i])
                if(i < tree[2].length-1) stat.value.push(',')
            }
            
            stat.value.push(`)`)
            stat.value.push(`{`)
            prepare(tree[3], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push(`}`)
            break
        }
        case 'object': {
            stat.value.push(`{`)
            for(let d of tree[1]){
                stat.value.push(`${d[0]}:`)
                prepare(d[1], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
                stat.value.push(`,`)
            }
            stat.value.push(`}`)
            break
        }
        case 'block': {
            stat.compute = true
            prepare(tree[1], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            break
        }
        case 'stat': {
            prepare(tree[1], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, tree[1][0] === 'qmlelem' ? currentInstructions : currentObj)
            break
        }
        case 'new': {
            stat.value.push(` new `)
            prepare(tree[1], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push(`(`)
            for(let i = 0; i < tree[2].length; i++){
                prepare(tree[2][i], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
                if(i < tree[2].length-1) stat.value.push(',')
            }
            stat.value.push(`)`)
            break
        }
        case 'sub': {
            prepare(tree[1], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push(`[`)
            prepare(tree[2], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push(`]\n`)
            break
        }
        case 'array': {
            stat.value.push(`[`)
            for(let i = 0; i < tree[1].length; i++){
                prepare(tree[1][i], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
                if(i < tree[1].length-1) stat.value.push(',')
            }
            stat.value.push(`]`)
            break
        }
        case 'regexp': {
            stat.value.push(`/${tree[1]}/${tree[2]}`)
            break
        }
        case 'switch': {
            stat.value.push(`switch(`)
            prepare(tree[1], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
            stat.value.push(`){`)
            for(let i = 0; i < tree[2].length; i++){
                if(tree[2][i][0] === null){
                    stat.value.push(`default: `)
                    prepare(tree[2][i][1], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
                } else {
                    stat.value.push(`case `)
                    prepare(tree[2][i][0], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
                    stat.value.push(`:`)
                    prepare(tree[2][i][1], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
                    
                }
                
            }
            stat.value.push(`}`)
            break
        }
        case 'seq': {
            stat.value.push(`(`)
            for(let i = 1; i < tree.length; i++){
                prepare(tree[i], compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
                if(i < tree.length-1) stat.value.push(',')
            }
            stat.value.push(`)`)
            break
        }
        case 'break': {
            stat.value.push(`break;`)
            break
        }
        case 'continue': {
            stat.value.push(`continue;`)
            break
        }
        default: {
            if(stat.type !== 'QAlias'){
                for(let t of tree){
                    prepare(t, compiledFile, currentInstructions, stat, propValue, assign, prevCommand, currentObj)
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

let code = []
function treeCompile(compiledFile, currentInstructions, updatePrimaryList = [], updateList = [], step = 0, innerComponent = false){
    code.push(currentInstructions.UID > 1 ? `let el${currentInstructions.UID} = new ${currentInstructions.getClassName()}(${innerComponent ? 'currParent,inCtx,exModel' : currentInstructions.parent.name+',inCtx'})` : `let el${currentInstructions.UID} = this`)

    let extendInstruction = currentInstructions
    while(extendInstruction && listComponents.indexOf(extendInstruction.className) < 0){
        let extendsFile = compiledFiles[path.resolve(path.resolve(__dirname, source), extendInstruction.className.replaceAll('_', '/')).replaceAll('\\', '/') + '.qml']
        currentInstructions.extends = extendsFile
        extendInstruction = extendsFile ? extendsFile.instructions : null
    }

    let special = false
    let isLoader = false
    let loaderLR = 0
    let loaderTB = 0
    let tempInstruction = currentInstructions
    while(tempInstruction){
        if(tempInstruction.className === 'Loader'){
            isLoader = true
        }
        if(tempInstruction.className === 'Loader' || tempInstruction.className === 'Repeater' || tempInstruction.className === 'ListView' || tempInstruction.className === 'GridView'){
            special = true
        }
        tempInstruction = tempInstruction.extends ? tempInstruction.extends.instructions : null
    }
    
    if(currentInstructions.id) {
        code.push(`inCtx.add('${currentInstructions.id}',el${currentInstructions.UID})`)
        code.push(`el${currentInstructions.UID}.$id='${currentInstructions.id}'`)
    }

    if(special){
        // code.push(`el${currentInstructions.UID}.$exCtx=inCtx`)
        code.push(`el${currentInstructions.UID}.$path='${compiledFile.namespace}'`)
    }

    let updateAnchors = false
    let updateFont = false
    // let updateList = []
    // let updatePrimaryList = []

    for(let property of currentInstructions.properties){
        let pathName = property.name.split('.')
        if(pathName[0] !== 'anchors' && pathName[0] !== 'font') continue

        for(let i = 0; i < pathName.length; i++){
            pathName[i] = `getStatement('${pathName[i]}')`
        }
        if(isLoader){
            if(property.name === 'anchors.fill'){
                loaderLR = 2
                loaderTB = 2
            }
            if(property.name === 'anchors.left' || property.name === 'anchors.right'){
                loaderLR += 1
            }
            if(property.name === 'anchors.top' || property.name === 'anchors.bottom'){
                loaderTB += 1
            }

            if(loaderLR >= 2) {
                code.push(`${currentInstructions.name}.$widthAuto=false`)
                loaderLR = 0
            }
            if(loaderTB >= 2) {
                code.push(`${currentInstructions.name}.$heightAuto=false`)
                loaderTB = 0
            }
        }
        let stat = {
            return: false,
            compute: false,
            subscribe: [],
            value: [],
            ignore: [],
            params: [],
        }
        if(property.val) {
            try {
                prepare(property.val, compiledFile, currentInstructions, stat, false, false, '', {})
            } catch (error) {
                console.log('error', compiledFile.fileName, 'property', property.name)
            }
            
        }
        if(stat.compute){
            if(stat.subscribe.length){
                let subscribe = []
                for(let subs of stat.subscribe){
                    subscribe.push(`${currentInstructions.name}.${pathName.join('.')}.subscribe(${subs})`)
                }
                if(property.command === 'create'){
                    if(listProperties[property.type]){
                        // code.push(`${currentInstructions.name}.${property.name} = new ${property.type}(function(){${!stat.return ? 'return' : ''} ${stat.value.join('')}})`)
                        code.push(`${currentInstructions.name}.createProperty('${property.name}',${property.type},function(){${!stat.return ? 'return' : ''} ${stat.value.join('')}})`)
                    } else {
                        code.push(`${currentInstructions.name}.createVariantProperty('${property.name},${property.type},function(){${!stat.return ? 'return' : ''} ${stat.value.join('')}})`)
                    }
                    
                } else {
                    code.push(`${currentInstructions.name}.${pathName.join('.')}.setCompute(function(){${!stat.return ? 'return' : ''} ${stat.value.join('')}})`)
                }
                
            } else {
                if(property.command === 'create'){
                    if(listProperties[property.type]){
                        // code.push(`${currentInstructions.name}.${property.name} = new ${property.type}(function(){${!stat.return ? 'return' : ''} ${stat.value.join('')}})`)
                        code.push(`${currentInstructions.name}.createProperty('${property.name}',${property.type},function(){${!stat.return ? 'return' : ''} ${stat.value.join('')}})`)
                    } else {
                        code.push(`${currentInstructions.name}.createVariantProperty('${property.name}',${property.type},function(){${!stat.return ? 'return' : ''} ${stat.value.join('')}})`)
                    }
                    
                } else {
                    code.push(`${currentInstructions.name}.${pathName.join('.')}.setCompute(function(){${!stat.return ? 'return' : ''} ${stat.value.join('')}})`)
                }
                
            }
            if(!updateAnchors && property.name.split('.')[0] === 'anchors'){
                updateAnchors = true
                updatePrimaryList.push(`${currentInstructions.name}.${pathName[0]}.update()`)
            }
            if(!updateFont && property.name.split('.')[0] === 'font'){
                updateFont = true
                updatePrimaryList.push(`${currentInstructions.name}.${pathName[0]}.update()`)
            }
            
        } else {
            if(property.command === 'create'){
                if(listProperties[property.type]){
                    // code.push(`${currentInstructions.name}.${property.name} = new ${property.type}(${stat.value.join('')})`)
                    code.push(`${currentInstructions.name}.createProperty('${property.name}',${property.type},${stat.value.join('')})`)
                } else {
                    code.push(`${currentInstructions.name}.createVariantProperty('${property.name}',${property.type},${stat.value.join('')})`)
                }
                
            } else {
                code.push(`${currentInstructions.name}.${property.name} = ${stat.value.join('')}`)
            }
            
        }
        
    }
    let readySignals = []
    for(let signal of currentInstructions.connectionSignals){
        if(currentInstructions.defineSignals[signal.name]) continue
        
        let name = signal.name
        let linkedProperty = testProperty(name, currentInstructions)
        if(linkedProperty){
            name = name.replaceAll('Changed', '')
            if(currentInstructions.propertiesNames.indexOf(name) >= 0) continue
        }   
        readySignals.push(name)    
        let stat = {
            return: false,
            compute: false,
            subscribe: [],
            value: [],
            ignore: [],
            params: [],
        }

        if(!linkedProperty){
            for(let p of getSignalParams(name, currentInstructions)){
                stat.params.push(p.replaceAll('`', ''))
                // stat.ignore.push(p.replaceAll('`', ''))
            }
        }
        
        try {
            prepare(signal.val, compiledFile, currentInstructions, stat, true, false, '', {})
        } catch (error) {
            console.log('error', compiledFile.fileName, 'signal', signal.name)
        }

        for(let ignore of stat.ignore){
            let index = stat.params.indexOf(ignore)
            if(index >= 0) stat.params[index] = '$'+stat.params[index]
        }
        
        if(linkedProperty){
            code.push(`${currentInstructions.name}.getStatement('${name}').getNotify().connect(${currentInstructions.name}, function(){${stat.value.join('')}})`)
        } else {
            code.push(`${currentInstructions.name}.getStatement('${name}').connect(${currentInstructions.name}, function(${stat.params.join(',')}){${stat.value.join('')}})`)
        }
        
        
    }

    // if(currentInstructions.className === 'Component') {
    //     code.push(`${currentInstructions.name}.createObject=(currParent)=>{`)
    //     for(let i = currentInstructions.children.length-1; i >= 0; i--){      
    //         // preTreeCompile(compiledFile, currentInstructions.children[i], true)
    //         treeCompile(compiledFile, currentInstructions.children[i], true)
    //     }
    //     code.push(`}`)
    // } else {
    //     for(let i = currentInstructions.children.length-1; i >= 0; i--){      
    //         preTreeCompile(compiledFile, currentInstructions.children[i], false)
    //     }
    // }





    for(let property of currentInstructions.properties){
        let pathName = property.name.split('.')

        if(pathName[0] === 'anchors' || pathName[0] === 'font') continue

        for(let i = 0; i < pathName.length; i++){
            pathName[i] = `getStatement('${pathName[i]}')`
        }
        let stat = {
            return: false,
            compute: false,
            subscribe: [],
            value: [],
            ignore: [],
            type: property.type,
            params: [],
        }

        if(isLoader){
            if(property.name === 'width'){
                code.push(`${currentInstructions.name}.$widthAuto=false`)
            }
            if(property.name === 'height'){
                code.push(`${currentInstructions.name}.$heightAuto=false`)
            }
        }
        
        if(property.isElement){
            if(property.command === 'create' && property.val.className !== 'Component'){
                if(property.type === 'Component'){  
                    code.push(`${currentInstructions.name}.$temp = new ${property.type}(${currentInstructions.name})`)
                    code.push(`${currentInstructions.name}.$temp.createObject=function(currParent,exCtx,exModel){`)
                    code.push(`let inCtx = new ContextController(exCtx)`)
                    treeCompile(compiledFile, property.val, [], [], 0, true)
                    code.push(`}`)
                    
                    // code.push(`${currentInstructions.name}.${property.name} = new ${property.type}(${property.val.name})`)
                    code.push(`${currentInstructions.name}.${property.name} = ${currentInstructions.name}.$temp`)
                    code.push(`delete ${currentInstructions.name}.$temp`)
                } else {
                    treeCompile(compiledFile, property.val)
                    if(listProperties[property.type]){
                        // code.push(`${currentInstructions.name}.${property.name} = new ${property.type}(${property.val.name})`)
                        code.push(`${currentInstructions.name}.createProperty('${property.name}',${property.type},${property.val.name})`)
                    } else {
                        code.push(`${currentInstructions.name}.createVariantProperty('${property.name}',${property.type},${property.val.name})`)
                    }
                }
                
                
            } else {
                if(property.val.className !== 'Component'){  
                    // TEMP !!!
                    if((property.name === 'delegate' && (currentInstructions.className === 'ListView'|| currentInstructions.className === 'GridView' || currentInstructions.className === 'Repeater')) || (property.name === 'sourceComponent' && currentInstructions.className === 'Loader')){
                        code.push(`${currentInstructions.name}.getStatement('${property.name}').value = new Component(${currentInstructions.name}, inCtx)`)
                        code.push(`${currentInstructions.name}.getStatement('${property.name}').value.createObject=function(currParent,exCtx,exModel){`)
                        
                        code.push(`let inCtx = new ContextController(exCtx)`)
                        treeCompile(compiledFile, property.val, [], [], 0, true)
                        code.push(`}`)
                        updateList.push(`${currentInstructions.name}.getStatement('${property.name}').getNotify()()`)
                    } else {
                        treeCompile(compiledFile, property.val)
                        code.push(`${currentInstructions.name}.${property.name} = ${property.val.name}`)
                    }
                    
                } else {
                    treeCompile(compiledFile, property.val)
                    // code.push(`${currentInstructions.name}.createVariantProperty('${property.name}',${property.type},${property.val.name})`)
                    code.push(`${currentInstructions.name}.${property.name} = ${property.val.name}`)

                }
                
            }
        } else {
            if(property.val) {
                try {
                    prepare(property.val, compiledFile, currentInstructions, stat, false, false, '', {})
                } catch (error) {
                    console.log('error', compiledFile.fileName, 'property', property.name)
                }
                
            }
            if(stat.compute){
                if(stat.subscribe.length){
                    let subscribe = []
                    for(let subs of stat.subscribe){
                        subscribe.push(`${currentInstructions.name}.${pathName.join('.')}.subscribe(${subs})`)
                    }
                    if(property.command === 'create'){
                        if(listProperties[property.type]){
                            // code.push(`${currentInstructions.name}.${property.name} = new ${property.type}(function(){${!stat.return ? 'return' : ''} ${stat.value.join('')}})`)
                            code.push(`${currentInstructions.name}.createProperty('${property.name}',${property.type},function(){${!stat.return ? 'return' : ''} ${stat.value.join('')}})`)
                        } else {
                            code.push(`${currentInstructions.name}.createVariantProperty('${property.name}',${property.type},function(){${!stat.return ? 'return' : ''} ${stat.value.join('')}})`)
                        }
                        
                    } else {
                        code.push(`${currentInstructions.name}.${pathName.join('.')}.setCompute(function(){${!stat.return ? 'return' : ''} ${stat.value.join('')}})`)
                    }
                    
                } else {
                    if(property.command === 'create'){
                        if(listProperties[property.type]){
                            // code.push(`${currentInstructions.name}.${property.name} = new ${property.type}(function(){${!stat.return ? 'return' : ''} ${stat.value.join('')}})`)
                            code.push(`${currentInstructions.name}.createProperty('${property.name}',${property.type},function(){${!stat.return ? 'return' : ''} ${stat.value.join('')}})`)
                        } else {
                            code.push(`${currentInstructions.name}.createVariantProperty('${property.name}',${property.type},function(){${!stat.return ? 'return' : ''} ${stat.value.join('')}})`)
                        }
                        
                    } else {
                        code.push(`${currentInstructions.name}.${pathName.join('.')}.setCompute(function(){${!stat.return ? 'return' : ''} ${stat.value.join('')}})`)
                    }
                    
                }
                updateList.push(`${currentInstructions.name}.${pathName[0]}.update()`)
            } else {
                if(property.command === 'create'){
                    if(listProperties[property.type]){
                        // code.push(`${currentInstructions.name}.${property.name} = new ${property.type}(${stat.value.join('')})`)
                        code.push(`${currentInstructions.name}.createProperty('${property.name}',${property.type},${stat.value.join('')})`)
                    } else {
                        code.push(`${currentInstructions.name}.createVariantProperty('${property.name}',${property.type},${stat.value.join('')})`)
                    }
                    
                } else {
                    code.push(`${currentInstructions.name}.${property.name} = ${stat.value.join('')}`)
                }
                
            }
        }
        
        
    }

    for(let name in currentInstructions.defineSignals){
        // prepare(currentInstructions.methods[name].val, compiledFile, currentInstructions, stat, true, false)
        code.push(`${currentInstructions.name}.createSignal('${name}',${currentInstructions.defineSignals[name].join(',')})`)
    }

    for(let signal of currentInstructions.connectionSignals){
        let name = signal.name
        let linkedProperty = testProperty(name, currentInstructions)
        if(linkedProperty){
            name = name.replaceAll('Changed', '')
            if(currentInstructions.propertiesNames.indexOf(name) < 0) continue
        }       
        if(readySignals.indexOf(name) >= 0) continue
        let stat = {
            return: false,
            compute: false,
            subscribe: [],
            value: [],
            ignore: [],
            params: [],
        }

        if(!linkedProperty){
            for(let p of getSignalParams(name, currentInstructions)){
                stat.params.push(p.replaceAll('`', ''))
                // stat.ignore.push(p.replaceAll('`', ''))
            }
        }

        try {
            prepare(signal.val, compiledFile, currentInstructions, stat, true, false, '', {})
        } catch (error) {
            console.log('error', compiledFile.fileName, 'signal', signal.name)
        }

        for(let ignore of stat.ignore){
            let index = stat.params.indexOf(ignore)
            if(index >= 0) stat.params[index] = '$'+stat.params[index]
        }
        
        if(linkedProperty){
            code.push(`${currentInstructions.name}.getStatement('${name}').getNotify().connect(${currentInstructions.name}, function(){${stat.value.join('')}})`)
        } else {
            code.push(`${currentInstructions.name}.getStatement('${name}').connect(${currentInstructions.name}, function(${stat.params.join(',')}){${stat.value.join('')}})`)
        }
        
        
    }

    for(let name in currentInstructions.methods){
        let stat = {
            return: false,
            compute: false,
            subscribe: [],
            value: [],
            ignore: [],
            params: [],
        }
        try {
            prepare(currentInstructions.methods[name].val, compiledFile, currentInstructions, stat, true, false, '', {})
        } catch (error) {
            console.log('error', compiledFile.fileName, 'method', name)
        }
        
        code.push(`${currentInstructions.name}.${name}=${stat.value.join('')}`)
    }

    if(currentInstructions.className === 'Component') {
        code.push(`${currentInstructions.name}.createObject=function(currParent,exCtx,exModel){`)
        code.push(`let inCtx = new ContextController(exCtx)`)
        for(let i = currentInstructions.children.length-1; i >= 0; i--){
            treeCompile(compiledFile, currentInstructions.children[i], [], [], 0, true)
        }
        code.push(`}`)
    } else {    
        // for(let i = currentInstructions.children.length-1; i >= 0; i--){
        //     treeCompile(compiledFile, currentInstructions.children[i], updatePrimaryList, updateList, step + 1, false)
        // }
        for(let i = 0; i < currentInstructions.children.length; i++){
            treeCompile(compiledFile, currentInstructions.children[i], updatePrimaryList, updateList, step + 1, false)
        }
    }

    if(step === 0){
        code.push('updateList.push(()=>{')
        for(let i = updatePrimaryList.length-1; i >= 0; i--){
            code.push(updatePrimaryList[i])
        }
    
        for(let upd of updateList){
            code.push(upd)
        }
        code.push('})')
    }
    
    if(innerComponent) code.push(`return el${currentInstructions.UID}`)
}
let queueList = []
let queueFiles = []
for(let fileName in compiledFiles){
    queueFiles.push(fileName)
}
while(queueFiles.length){
    let fileName = queueFiles.shift()
    let compiledFile = compiledFiles[fileName]

    let className = path.relative(path.resolve(__dirname, source), fileName).replaceAll('/', '_').replaceAll('\\', '_').replaceAll('.qml', '')

    if(listComponents.indexOf(className)>=0){
        console.log(`Warning: skip define class ${className}`)
        continue
    } else {
        let pathName = path.resolve(path.resolve(__dirname, source), compiledFile.instructions.getClassName().replaceAll('_', '/')).replaceAll('\\', '/') + '.qml'
        if(queueFiles.indexOf(pathName) >= 0){
            queueFiles.push(fileName)
            continue
        }
        code.push(`class ${className} extends ${compiledFile.instructions.getClassName()} {`)
    }
    
    let SingletonName = className.split('_').pop()
    code.push(`constructor(parent, exCtx, exModel) {`)
    if(compiledFile.instructions.Singleton) code.push(`if(Singletons['${SingletonName}']) return Singletons['${SingletonName}']`)
    code.push(`super(parent, exCtx, exModel)`)
    if(compiledFile.instructions.Singleton) code.push(`Singletons['${SingletonName}'] = this`)
    // if(compiledFile.instructions.Singleton) code.push(`Singletons['${SingletonName}'] = this`)
    code.push(`let inCtx = new ContextController(exCtx)`)

    treeCompile(compiledFile, compiledFile.instructions)

    code.push(`}`)
    code.push(`}`)
    if(compiledFile.instructions.Singleton) code.push(`SingletonClass['${SingletonName}'] = ${className}`)

}

// console.log(code.join('\n'))
fs.writeFileSync(path.resolve(__dirname, source, 'jqml.full.js'), code.join('\n'))

// const crypto = require('crypto')
// const zlib = require('zlib')

// fs.writeFileSync(path.resolve(__dirname, source, 'jqml.full.js.gzip'), zlib.gzipSync(code.join('\n')))