const fs = require('fs')
const path = require('path')
const parser = require('./parser')
const QML = [
    'Component',
    'QtObject',
    'Item',
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
            if(path.extname(name) === '.qml') _files.push(name)
        }
    }
    return _files
}

if(!source) source = `C:\\Users\\Artur\\Documents\\projects\\QMLTEST`
if(!destination) destination = source
  
let files = getFiles(source)

let IDList = new Set()

function getBaseStructure(){
    return {
        class: '',
        id: '',
        properties: {},
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
function qmlimport(m, instructions, file){
    for(let imp of m){
        if(imp[0] === 'qmlimport'){
            if(imp[1].indexOf('js') >= 0){
                jsName = imp[1]
                jsAs = imp[3]
                let path = file.split('/')
                path = path.slice(0, path.length-1).join('/') + '/' + jsName
                let content = fs.readFileSync(path, encoding='utf-8')
                let meta = parser.jsparse(content)
                
                instructions.js[jsName] = meta.source.replaceAll(/(?<=[^\\])[`]/g, '\\`').replaceAll('\\', '\\\\') + (jsAs ? `\r\nvar ${jsAs} = {${meta.exports.join(',')}}\r\n` : '')
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
function qmlelem(m, instructions, file){
    let cls = m[1]
    let childInstructions = getBaseStructure()
    if(QML.indexOf(m[1]) < 0){
        if(m[1][0] === 'dot'){
            let name = m[1][2]
            let as = m[1][1]
            for(let qml of instructions.qml){
                if(qml.as === as){
                    let childFile = [source,qml.path,name + '.qml'].join('/') 
                    let data = fs.readFileSync(childFile, {encoding:'utf8', flag:'r'})

                    data = data.replaceAll(/((?<=[^:])\/\/(.*?)\n)|(\/\*(.*?)\*\/)/gms, '\n')
                    let meta = parser.parse(data)
                    
                    qmlimport(meta[1], childInstructions, childFile)
                    preCompile(meta[2][1], meta[2][3], meta[2][2], childInstructions, childFile)
                    cls = meta[2][1]
                }
            }
        } else {
            let childFile = [source,m[1] + '.qml'].join('/') 
            let data = fs.readFileSync(childFile, {encoding:'utf8', flag:'r'})

            data = data.replaceAll(/((?<=[^:])\/\/(.*?)\n)|(\/\*(.*?)\*\/)/gms, '\n')
            let meta = parser.parse(data)
            
            qmlimport(meta[1], childInstructions, childFile)
            preCompile(meta[2][1], meta[2][3], meta[2][2], childInstructions, childFile)
            cls = meta[2][1]
        }
        
    }
    preCompile(cls, m[3], m[2], childInstructions, file) 
    instructions.children.push(childInstructions)
}
function qmlsignaldef(m, instructions){
    let params = []
    for(let p of m[2]){
        params.push(`\`${p.name}\``)
    }
    let name = m[1]
    instructions.defineSignals[name] = params.join(',')
}
function qmlmethod(m, instructions){
    let name = m[1]
    let params = m[2][2].join(',')
    let src = m[3].trimStart().trimEnd()
    let lpos = src.indexOf('{')
    let rpos = src.lastIndexOf('}')
    if(lpos >= 0 && rpos >= 0) src = src.slice(lpos+1, rpos)
    instructions.methods[name] = {
        params: params,
        source: src
    }
}
function qmlaliasdef(m, instructions){
    let name = m[1]
    instructions.propertiesLazy[name] = m.slice(2).join('.')
}
function qmlpropdef(m, instructions){
    try {
        let name = m[1]
        let _meta = parser.parse(m[4])
        if(!_meta[2]) throw 1
        let propertyInstructions = getBaseStructure()
        preCompile(_meta[2][1], _meta[2][3], _meta[2][2], propertyInstructions) 
        instructions.propertiesQMLNew[name] = propertyInstructions
    } catch {
        let name = m[1]
        let type = m[2]
        let val = m[4] ? m[4].trimStart().trimEnd() : undefined
        try {
            let cval = eval(val)
            instructions.propertiesNew[name] = cval
        } catch (error) {
            instructions.propertiesLazyNew[name] = val
        }

        try {
            let cval = eval(val)

            if(typeof cval === 'string') {
                instructions.propertiesNew[name] = cval
            } else if(typeof cval === 'object'){
                instructions.propertiesNew[name] = cval
            } else {
                instructions.propertiesNew[name] = cval
            }
        } catch (error) {
            instructions.propertiesLazyNew[name] = val
        }
    }
}
function qmlprop(m, instructions){
    if(m[2][0] === "block" || m[2][1][0] === "assign"){
        if(m[1][0] === "dot"){
            let signalCall = m[1][2]
            let signalName = signalCall.slice(2).split('')
            signalName[0] = signalName[0].toLowerCase()
            signalName = signalName.join('')
            let src = m[3].trimStart().trimEnd()
            let lpos = src.indexOf('{')
            let rpos = src.lastIndexOf('}')
            if(lpos >= 0 && rpos >= 0) src = src.slice(lpos+1, rpos)
            instructions.connectionSignals.push({
                name: signalName,
                source: src
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
                source: src
            })
        } 
    } else if (m[1][0] === "dot"){
        let name = m[1].slice(1).join('.')
        let val = m[3].trimStart().trimEnd()
        
        try {
            let cval = eval(val)
            instructions.properties[name] = cval
        } catch (error) {
            instructions.propertiesLazy[name] = val
        }
    } else if(m[2][1][0] === "binary" || m[2][1][0] === "dot" || m[2][1][0] === "conditional"){
        let name = m[1]
        let val = m[3].trimStart().trimEnd()
        
        try {
            let cval = eval(val)
            instructions.properties[name] = cval
        } catch (error) {
            instructions.propertiesLazy[name] = val
        }
    } else if(m[1] === "delegate"){
        let _meta = parser.parse(m[3])
        let delegateInstructions = getBaseStructure()
        preCompile(_meta[2][1], _meta[2][3], _meta[2][2], delegateInstructions) 
        instructions.propertiesSpecial.delegate = delegateInstructions
    } else if(m[1] === "validator"){
        let _meta = parser.parse(m[3])
        let validatorInstructions = getBaseStructure()
        preCompile(_meta[2][1], _meta[2][3], _meta[2][2], validatorInstructions) 
        instructions.propertiesSpecial.validator = validatorInstructions
    } else if(m[1] === "gradient"){
        let _meta = parser.parse(m[3])
        let gradientInstructions = getBaseStructure()
        preCompile(_meta[2][1], _meta[2][3], _meta[2][2], gradientInstructions) 
        instructions.propertiesSpecial.gradient = gradientInstructions
    } else {
        try {
            let name = m[1]
            let _meta = parser.parse(m[3])
            if(!_meta[2]) throw 1
            let propertyInstructions = getBaseStructure()
            preCompile(_meta[2][1], _meta[2][3], _meta[2][2], propertyInstructions) 
            instructions.propertiesQML[name] = propertyInstructions
        } catch {
            let name = m[1]
            let val = m[3].trimStart().trimEnd()

            if(name.slice(0, 2) === 'on'){
                let signalCall = name
                let signalName = signalCall.slice(2).split('')
                signalName[0] = signalName[0].toLowerCase()
                signalName = signalName.join('')
                let src = val
                instructions.signals.push({
                    name: signalName,
                    source: src
                })
            } else {
                if(name === 'id'){
                    instructions[name] = val
                    IDList.add(val)
                } else {
                    try {
                        let cval = eval(val)
                        instructions.properties[name] = cval
                    } catch (error) {
                        instructions.propertiesLazy[name] = val
                    }
                }
                
            }

            
        }
    }
}
function preCompile(cls, meta, on, instructions, file){
    instructions.class = cls[0] === 'dot' ? cls.slice(1).pop() : cls
    if(on) {
        instructions.properties.properties = on
    }
    for(let m of meta){
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
            qmlelem(m, instructions, file)
        }
    }
}

let compiledFiles = {}

for(let file of files){
    let data = fs.readFileSync(file, {encoding:'utf8', flag:'r'})

    compiledFiles[file] = {
        'instructions': {},
        'code': '',
    }
    let instructions = getBaseStructure()
    
    data = data.replaceAll(/((?<=[^:])\/\/(.*?)\n)|(\/\*(.*?)\*\/)/gms, '\n')
    
    let meta = parser.parse(data)
    qmlimport(meta[1], instructions, file)

    if(meta[2][0] === 'qmlelem'){
        preCompile(meta[2][1], meta[2][3], meta[2][2], instructions, file)
        compiledFiles[file].instructions = instructions
    }
    console.log(instructions)
}

function compile(instructions, code, curr = '$root', prev = ''){
    for(let file in instructions.js){
        code.push(`${instructions.js[file]}`)
    }
    if(!prev){
        code.push(`let ${curr}=Core.createComponent(\`${instructions.class}\`, parent)`)

    } else {
        code.push(`let ${curr}=Core.createComponent(\`${instructions.class}\`, ${prev})`)
    }
    if(instructions.id) code.push(`${curr}.id=\`${instructions.id}\``)
    for(let prop in instructions.properties){
        let val = instructions.properties[prop]
        if(typeof val === 'string'){
            code.push(`${curr}.${prop}=\`${val}\``)
        } else {
            code.push(`${curr}.${prop}=${val}`)
        }
        
    }
    for(let prop in instructions.propertiesLazy){
        let val = instructions.propertiesLazy[prop]
        code.push(`${curr}.$setProperty(\`${prop}\`, function(){return ${val}})`)
    }
    for(let prop in instructions.propertiesNew){
        let val = instructions.propertiesNew[prop]
        if(typeof val === 'string'){
            code.push(`${curr}.$cP(\`${prop}\`,\`${val}\`)`)
        } else {
            code.push(`${curr}.$cP(\`${prop}\`,${val})`)
        }
        
    }
    for(let prop in instructions.propertiesLazyNew){
        let val = instructions.propertiesLazyNew[prop]
        code.push(`${curr}.$cP(\`${prop}\`,function(){return ${val}})`)
    }

    for(let prop in instructions.propertiesSpecial){
        let codeNew = []
        codeNew.push(`function(parent){`)
        compile(instructions.propertiesSpecial[prop], codeNew)
        codeNew.push(`return $root`)
        codeNew.push(`}`)
        let val = codeNew.join('\n')
        code.push(`${curr}.${prop}=${val}`)
    }

    for(let prop in instructions.propertiesQML){
        let codeNew = []
        codeNew.push(`function(parent){`)
        compile(instructions.propertiesQML[prop], codeNew)
        codeNew.push(`return $root`)
        codeNew.push(`}`)
        let val = codeNew.join('\n')
        code.push(`${curr}.${prop}=${val}(${curr})`)
    }
    for(let prop in instructions.propertiesQMLNew){
        let codeNew = []
        codeNew.push(`function(parent){`)
        compile(instructions.propertiesQMLNew[prop], codeNew)
        codeNew.push(`return $root`)
        codeNew.push(`}`)
        let val = codeNew.join('\n')
        code.push(`${curr}.$cP(\`${prop}\`,${val}(${curr}))`)
    }

    for(let name in instructions.methods){
        code.push(`${curr}.${name}=function(${instructions.methods[name].params}){${instructions.methods[name].source}}`)
    }
    for(let name in instructions.defineSignals){
        if(instructions.defineSignals[name]){
            code.push(`${curr}.$createSignal(\`${name}\`,${instructions.defineSignals[name]})`)
        } else {
            code.push(`${curr}.$createSignal(\`${name}\`)`)
        }
        
    }
    for(let signal of instructions.connectionSignals){
        code.push(`${curr}['${signal.name}'].connect(function(){${signal.source}})`)
    }
    
    let step = 0
    for(let child of instructions.children){
        compile(child, code, curr+'c'+step, curr)
        step++
    }
    
}

for(file in compiledFiles){
    let name = file.split('/').pop().replaceAll('.qml', '')
    let code = []
    let instructions = compiledFiles[file].instructions
    code.push(`function ${name}(parent){`)
    compile(instructions, code)
    code.push(`return $root`)
    code.push(`}`)
    for(let ID of IDList){
        for(let i = 0; i < code.length; i++){
            if(code[i].indexOf('.id=')<0) code[i] = code[i].replaceAll(ID, `IDManager.get(this,\`${ID}\`)`)
        }
    }
    compiledFiles[file].code = code.join('\n')

    fs.writeFile(file.replaceAll('.qml', '.js'), compiledFiles[file].code, function(error){
        if(error) throw error
    })
}


// fs.writeFile([destination, 'fullImport.js'].join('/'), `function fullImport(){${fullImport.join('\n')}}`, function(error){
//     if(error) throw error
// })
