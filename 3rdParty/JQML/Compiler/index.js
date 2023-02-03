const fs = require('fs')
const path = require('path')
const parser = require('./parser')
const crypto = require('crypto')

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

if(!source) source = `C:\\Users\\Артур\\Documents\\projects\\2023\\РТС\\web\\web\\src`
// if(!source) source = `C:\\projects\\ImagingTools\\ItDevelopment\\Lisa\\Bin\\web\\src`
if(!destination) destination = source

if(!fs.existsSync(source + '/cache/')) fs.mkdirSync(source + '/cache/');

let files = getFiles(source)

let IDList = new Set()

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
        source: src
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
                instructions.properties[name] = val
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
                    source: src
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

    if(fs.existsSync(fileHash)){
        let savedHash = fs.readFileSync(fileHash, {encoding:'utf8', flag:'r'}).split('\n')

        if(currentHash === savedHash[0]){
            let savedData = fs.readFileSync(fileSave, {encoding:'utf8', flag:'r'})

            compiledFiles[file].IDList = new Set(savedData[1].split(','))
            for(let id of compiledFiles[file].IDList){
                IDList.add(id)
            }
            compiledFiles[file].code = savedData
            compiledFiles[file].cache = true
            let instructions = getBaseStructure()
    
            data = data.replaceAll(/((?<![:\/])\/\/(.*?)\n)|(\/\*(.*?)\*\/)/gms, '\n')
            
            parser.parse.nowParsingFile = file.replaceAll(/\\+/g, '/')
            let meta = parser.parse(data)
            if(meta[3]) qmlpragma(meta[3], instructions, file)
            compiledFiles[file].instructions = instructions
            continue
        } else {
            compiledFiles[file].currentHash = currentHash
            // fs.writeFile(fileHash, currentHash, function(error){
            //     if(error) throw error
            // })
        }
    } else {
        compiledFiles[file].currentHash = currentHash
        // fs.writeFile(fileHash, currentHash, function(error){
        //     if(error) throw error
        // })
    }

    
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


function IDReplace(instructions){
    for(let ID of IDList){
        // let reg = RegExp(`((?<=\\W)${ID}(?=\\W))|(^${ID}(?=\\W))|((?<=\\W)${ID}$)|(^${ID}$)`, 'gm')
        let reg = RegExp(`((?<!\\w)${ID}(?!\\w))`, 'gm')
        let varTemp = `var ${ID}`
        let letTemp = `let ${ID}`
        // let dotTemp = [`.${ID}`, `_dotTemp_`]
        // let pathTemp = [`./${ID}`, `_pathTemp_`]
        // let keyDict = [`${ID}:`, `${ID} :`, `_keyDict_`]
        // let keyDict2 = [`'${ID}'`, `_keyDict2_`]
        // let keyDict3 = [`"${ID}"`, `_keyDict3_`]
        
        let replaceList = [
            RegExp(`\\.${ID}(?!\\w)`, 'g'),`_dotTemp_`,
            `./${ID}`,`_pathTemp_`,
            RegExp(`(?<!\\?\\s+)${ID}\\s:`, 'g'),`_keyDict_`,
            RegExp(`(?<!\\?\\s+)${ID}:`, 'g'),`_keyDict_`,
            `'${ID}'`,`_keyDict2_`,
            `"${ID}"`,`_keyDict3_`,
            `'${ID}`,`_upTemp_`,
            `${ID}'`,`_upTemp2_`,
            `"${ID}`,`_upUpTemp_`,
            `${ID}"`,`_upUpTemp2_`,
            `?${ID}`,`_questionTemp_`,
            `${ID}.png`,`_pngTemp_`,
            `${ID}.svg`,`_svgTemp_`
        ]
        let replaceListR = [
            `.${ID}`,`_dotTemp_`,
            `./${ID}`,`_pathTemp_`,
            `${ID} :`,`_keyDict_`,
            `${ID}:`,`_keyDict_`,
            `'${ID}'`,`_keyDict2_`,
            `"${ID}"`,`_keyDict3_`,
            `'${ID}`,`_upTemp_`,
            `${ID}'`,`_upTemp2_`,
            `"${ID}`,`_upUpTemp_`,
            `${ID}"`,`_upUpTemp2_`,
            `?${ID}`,`_questionTemp_`,
            `${ID}.png`,`_pngTemp_`,
            `${ID}.svg`,`_svgTemp_`
        ]

        for(let name in instructions.propertiesLazy){
            if(name === ID || !(ID in instructions.properties || ID in instructions.propertiesNew || ID in instructions.propertiesLazy || ID in instructions.propertiesLazyNew || ID in instructions.propertiesQML || ID in instructions.propertiesQMLNew || ID in instructions.propertiesSpecial || ID in instructions.propertiesAlias)) {
                for(let n = 0; n < replaceList.length; n+=2){
                    instructions.propertiesLazy[name] = instructions.propertiesLazy[name].replaceAll(replaceList[n], replaceList[n+1])
                }
                //instructions.propertiesLazy[name] = instructions.propertiesLazy[name].replaceAll(dotTemp[0], dotTemp[1]).replaceAll(pathTemp[0], pathTemp[1])
                if(instructions.id.has(`\`${ID}\``))
                instructions.propertiesLazy[name] = instructions.propertiesLazy[name].replaceAll(reg, `IDManager.get(this,\`${ID}\`)`); else
                instructions.propertiesLazy[name] = instructions.propertiesLazy[name].replaceAll(reg, `IDManager.get0(this,\`${ID}\`)`)
                for(let n = 0; n < replaceListR.length; n+=2){
                    instructions.propertiesLazy[name] = instructions.propertiesLazy[name].replaceAll(replaceListR[n+1], replaceListR[n])
                }
                //instructions.propertiesLazy[name] = instructions.propertiesLazy[name].replaceAll(dotTemp[1], dotTemp[0]).replaceAll(pathTemp[1], pathTemp[0])
            } 
        }
        for(let name in instructions.propertiesLazyNew){
            if(name === ID || !(ID in instructions.properties || ID in instructions.propertiesNew || ID in instructions.propertiesLazy || ID in instructions.propertiesLazyNew || ID in instructions.propertiesQML || ID in instructions.propertiesQMLNew || ID in instructions.propertiesSpecial || ID in instructions.propertiesAlias)) {
                // instructions.propertiesLazyNew[name] = instructions.propertiesLazyNew[name].replaceAll(dotTemp[0], dotTemp[1]).replaceAll(pathTemp[0], pathTemp[1])
                for(let n = 0; n < replaceList.length; n+=2){
                    instructions.propertiesLazyNew[name] = instructions.propertiesLazyNew[name].replaceAll(replaceList[n], replaceList[n+1])
                }
                if(instructions.id.has(`\`${ID}\``))
                instructions.propertiesLazyNew[name] = instructions.propertiesLazyNew[name].replaceAll(reg, `IDManager.get(this,\`${ID}\`)`); else
                instructions.propertiesLazyNew[name] = instructions.propertiesLazyNew[name].replaceAll(reg, `IDManager.get0(this,\`${ID}\`)`)

                // if(ID === 'organizationSubmenuModel' && name === 'submodel'){
                //     console.log(instructions.propertiesLazyNew[name])
                // }
                // instructions.propertiesLazyNew[name] = instructions.propertiesLazyNew[name].replaceAll(dotTemp[1], dotTemp[0]).replaceAll(pathTemp[1], pathTemp[0])
                for(let n = 0; n < replaceListR.length; n+=2){
                    instructions.propertiesLazyNew[name] = instructions.propertiesLazyNew[name].replaceAll(replaceListR[n+1], replaceListR[n])
                }
            } 
            
        }
        for(let name in instructions.propertiesAlias){
            if(name === ID || !(ID in instructions.properties || ID in instructions.propertiesNew || ID in instructions.propertiesLazy || ID in instructions.propertiesLazyNew || ID in instructions.propertiesQML || ID in instructions.propertiesQMLNew || ID in instructions.propertiesSpecial || ID in instructions.propertiesAlias)) {
                // instructions.propertiesAlias[name] = instructions.propertiesAlias[name].replaceAll(dotTemp[0], dotTemp[1]).replaceAll(pathTemp[0], pathTemp[1])
                for(let n = 0; n < replaceList.length; n+=2){
                    instructions.propertiesAlias[name] = instructions.propertiesAlias[name].replaceAll(replaceList[n], replaceList[n+1])
                }
                if(instructions.id.has(`\`${ID}\``))
                instructions.propertiesAlias[name] = instructions.propertiesAlias[name].replaceAll(reg, `IDManager.get(this,\`${ID}\`)`); else
                instructions.propertiesAlias[name] = instructions.propertiesAlias[name].replaceAll(reg, `IDManager.get0(this,\`${ID}\`)`)
                // instructions.propertiesAlias[name] = instructions.propertiesAlias[name].replaceAll(dotTemp[1], dotTemp[0]).replaceAll(pathTemp[1], pathTemp[0])
                for(let n = 0; n < replaceListR.length; n+=2){
                    instructions.propertiesAlias[name] = instructions.propertiesAlias[name].replaceAll(replaceListR[n+1], replaceListR[n])
                }
            } 
        }
        
        for(let name in instructions.methods){
            if(!(ID in instructions.properties || ID in instructions.propertiesNew || ID in instructions.propertiesLazy || ID in instructions.propertiesLazyNew || 
                ID in instructions.propertiesQML || ID in instructions.propertiesQMLNew || ID in instructions.propertiesSpecial || ID in instructions.propertiesAlias || 
                instructions.methods[name].source.indexOf(varTemp) >= 0 || instructions.methods[name].source.indexOf(letTemp) >= 0 || instructions.methods[name].params.indexOf(ID) >= 0)) {
                // instructions.methods[name].source = instructions.methods[name].source.replaceAll(keyDict[0], keyDict[2]).replaceAll(keyDict[1], keyDict[2]).replaceAll(keyDict2[0], keyDict2[1]).replaceAll(keyDict3[0], keyDict3[1]).replaceAll(dotTemp[0], dotTemp[1]).replaceAll(pathTemp[0], pathTemp[1])
                for(let n = 0; n < replaceList.length; n+=2){
                    instructions.methods[name].source = instructions.methods[name].source.replaceAll(replaceList[n], replaceList[n+1])
                }
                if(instructions.id.has(`\`${ID}\``))
                instructions.methods[name].source = instructions.methods[name].source.replaceAll(reg, `IDManager.get(this,\`${ID}\`)`); else
                instructions.methods[name].source = instructions.methods[name].source.replaceAll(reg, `IDManager.get0(this,\`${ID}\`)`)
                
                // instructions.methods[name].source = instructions.methods[name].source.replaceAll(keyDict[2], keyDict[0]).replaceAll(keyDict2[1], keyDict2[0]).replaceAll(keyDict3[1], keyDict3[0]).replaceAll(dotTemp[1], dotTemp[0]).replaceAll(pathTemp[1], pathTemp[0])
                for(let n = 0; n < replaceListR.length; n+=2){
                    instructions.methods[name].source = instructions.methods[name].source.replaceAll(replaceListR[n+1], replaceListR[n])
                }
            } 
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
            for(let i = 0; i < signalParams.length; i++){
                signalParams[i] = signalParams[i].replaceAll('`', '')
            }

            if(signalParams.indexOf(ID) < 0 && !(ID in instructions.properties || ID in instructions.propertiesNew || ID in instructions.propertiesLazy || ID in instructions.propertiesLazyNew || 
                ID in instructions.propertiesQML || ID in instructions.propertiesQMLNew || ID in instructions.propertiesSpecial || ID in instructions.propertiesAlias || 
                signal.source.indexOf(varTemp) >= 0 || signal.source.indexOf(letTemp) >= 0 || (instructions.defineSignals[signal.name] && instructions.defineSignals[signal.name].indexOf(ID) >= 0))) {
                    // signal.source = signal.source.replaceAll(keyDict[0], keyDict[2]).replaceAll(keyDict[1], keyDict[2]).replaceAll(keyDict2[0], keyDict2[1]).replaceAll(keyDict3[0], keyDict3[1]).replaceAll(dotTemp[0], dotTemp[1]).replaceAll(pathTemp[0], pathTemp[1])
                    for(let n = 0; n < replaceList.length; n+=2){
                        signal.source = signal.source.replaceAll(replaceList[n], replaceList[n+1])
                    }
                    if(instructions.id.has(`\`${ID}\``))
                    signal.source = signal.source.replaceAll(reg, `IDManager.get(this,\`${ID}\`)`); else
                    signal.source = signal.source.replaceAll(reg, `IDManager.get0(this,\`${ID}\`)`)
                    // signal.source = signal.source.replaceAll(keyDict[0], keyDict[2]).replaceAll(keyDict[2], keyDict[0]).replaceAll(keyDict2[1], keyDict2[0]).replaceAll(keyDict3[1], keyDict3[0]).replaceAll(dotTemp[1], dotTemp[0]).replaceAll(pathTemp[1], pathTemp[0])
                    for(let n = 0; n < replaceListR.length; n+=2){
                        signal.source = signal.source.replaceAll(replaceListR[n+1], replaceListR[n])
                    }
            } 
        }
    }
    for(let name in instructions.propertiesQML){
        IDReplace(instructions.propertiesQML[name])
    }
    for(let name in instructions.propertiesQMLNew){
        IDReplace(instructions.propertiesQMLNew[name])
    }
    for(let name in instructions.propertiesSpecial){
        IDReplace(instructions.propertiesSpecial[name])
    }
    for(let child of instructions.children){
        IDReplace(child)
    }
}

function ProxyReplace(instructions){
    let reserved = [
        'if',
        'else',
        'for',
        'while',
        'in',
        'typeof',
        'instanceof',
        'console.log',
        'new',
        'function',
        'Function',
        'eval',
        'try',
        'catch',
        'let',
        'var',
        'return',
    ]
    let simbols = '1234567890'
    let reg = /[\+\-\*\/\?\:\!\(\)\%\=\,\<\>\{\}\;\'\"\`\[\]\n ]+/g

    for(let name in instructions.propertiesLazy){
        let parts = instructions.propertiesLazy[name].split(reg)
        for(let part of parts){
            if(part !== '' && reserved.indexOf(part) < 0 && simbols.indexOf(part[0]) < 0 && '"`\''.indexOf(part[part.length-1]) < 0 && part.indexOf('this') < 0 && part.indexOf('IDManager') < 0){
                let p = part.split('.')
                if(QML.indexOf(p[0]) < 0 && p[0] !== 'parent'){
                    instructions.propertiesLazy[name] = instructions.propertiesLazy[name].replaceAll(RegExp(`(?<![\'\"\`])${p[0]}(?![\'\"\`])`, 'g'), `$Proxy('${p[0]}')`)
                }
                
            }
        }
    }
    for(let name in instructions.propertiesLazyNew){
        let parts = instructions.propertiesLazyNew[name].split(reg)
        for(let part of parts){
            if(part !== '' && reserved.indexOf(part) < 0 && simbols.indexOf(part[0]) < 0 && '"`\''.indexOf(part[part.length-1]) < 0 && part.indexOf('this') < 0 && part.indexOf('IDManager') < 0){
                let p = part.split('.')
                if(QML.indexOf(p[0]) < 0 && p[0] !== 'parent'){
                    instructions.propertiesLazyNew[name] = instructions.propertiesLazyNew[name].replaceAll(RegExp(`(?<![\'\"\`])${p[0]}(?![\'\"\`])`, 'g'), `$Proxy('${p[0]}')`)
                }
                
            }
        }
    }
    for(let name in instructions.propertiesAlias){
        let parts = instructions.propertiesAlias[name].split(reg)
        for(let part of parts){
            if(part !== '' && reserved.indexOf(part) < 0 && simbols.indexOf(part[0]) < 0 && '"`\''.indexOf(part[part.length-1]) < 0 && part.indexOf('this') < 0 && part.indexOf('IDManager') < 0){
                let p = part.split('.')
                if(QML.indexOf(p[0]) < 0 && p[0] !== 'parent'){
                    instructions.propertiesAlias[name] = instructions.propertiesAlias[name].replaceAll(RegExp(`(?<![\'\"\`])${p[0]}(?![\'\"\`])`, 'g'), `$Proxy('${p[0]}')`)
                }
                
            }
        }
    }
    
    for(let name in instructions.methods){
        let parts = instructions.methods[name].source.split(reg)
        for(let part of parts){
            if(part !== '' && reserved.indexOf(part) < 0 && simbols.indexOf(part[0]) < 0 && '"`\''.indexOf(part[part.length-1]) < 0 && instructions.methods[name].params.indexOf(part) < 0 && part.indexOf('this') < 0 && part.indexOf('IDManager') < 0){
                let p = part.split('.')
                if(QML.indexOf(p[0]) < 0 && p[0] !== 'parent' && instructions.methods[name].source.indexOf(`var ${p[0]}`) < 0 && instructions.methods[name].source.indexOf(`let ${p[0]}`) < 0){
                    instructions.methods[name].source = instructions.methods[name].source.replaceAll(RegExp(`(?<![\'\"\`])${p[0]}(?![\'\"\`])`, 'g'), `$Proxy('${p[0]}')`)
                }
                
            }
        }
    }
    for(let signal of instructions.connectionSignals){
        let parts = signal.source.split(reg)
        for(let part of parts){
            if(part !== '' && reserved.indexOf(part) < 0 && simbols.indexOf(part[0]) < 0 && '"`\''.indexOf(part[part.length-1]) < 0 && part.indexOf('this') < 0 && part.indexOf('IDManager') < 0){
                let p = part.split('.')
                if(QML.indexOf(p[0]) < 0 && p[0] !== 'parent' && signal.source.indexOf(`var ${p[0]}`) < 0 && signal.source.indexOf(`let ${p[0]}`) < 0){
                    signal.source = signal.source.replaceAll(RegExp(`(?<![\'\"\`])${p[0]}(?![\'\"\`])`, 'g'), `$Proxy('${p[0]}')`)
                }
                
            }
        }
    }
    
    for(let name in instructions.propertiesQML){
        ProxyReplace(instructions.propertiesQML[name])
    }
    for(let name in instructions.propertiesQMLNew){
        ProxyReplace(instructions.propertiesQMLNew[name])
    }
    for(let name in instructions.propertiesSpecial){
        ProxyReplace(instructions.propertiesSpecial[name])
    }
    for(let child of instructions.children){
        ProxyReplace(child)
    }
}

function anchorsReplace(instructions){
    // for(let name in instructions.propertiesLazy){
    //     if(name === 'anchors.fill'){
    //         delete instructions.properties['width']
    //         delete instructions.properties['height']
    //         delete instructions.properties['x']
    //         delete instructions.properties['y']

    //         let target = instructions.propertiesLazy[name]
    //         delete instructions.propertiesLazy[name]

    //         instructions.propertiesLazy['x'] = `anchors.leftMargin - (parent.left - ${target}.left)`
    //         instructions.propertiesLazy['y'] = `anchors.topMargin - (parent.top - ${target}.top)`
    //         instructions.propertiesLazy['width'] = `${target}.width - anchors.rightMargin - anchors.leftMargin`
    //         instructions.propertiesLazy['height'] = `${target}.height - anchors.bottomMargin - anchors.topMargin`
    //     } else if(name === 'anchors.centerIn'){
    //         delete instructions.properties['x']
    //         delete instructions.properties['y']

    //         let target = instructions.propertiesLazy[name]
    //         delete instructions.propertiesLazy[name]

    //         instructions.propertiesLazy['x'] = `${target}.width/2 - width/2 + anchors.leftMargin - anchors.rightMargin - (parent.left - ${target}.left)`
    //         instructions.propertiesLazy['y'] = `${target}.height/2 - height/2 + anchors.topMargin - anchors.bottomMargin - (parent.top - ${target}.top)`
    //     }
    // }
    if(instructions.propertiesLazy['anchors.fill'] && (instructions.propertiesLazy['anchors.left'] || instructions.propertiesLazy['anchors.right'] || instructions.propertiesLazy['anchors.top'] || instructions.propertiesLazy['anchors.bottom'])){
        let target = instructions.propertiesLazy['anchors.fill']
        delete instructions.propertiesLazy['anchors.fill']

        if(!instructions.propertiesLazy['anchors.left']) instructions.propertiesLazy['anchors.left'] = `${target}.left`
        if(!instructions.propertiesLazy['anchors.right']) instructions.propertiesLazy['anchors.right'] = `${target}.right`
        if(!instructions.propertiesLazy['anchors.top']) instructions.propertiesLazy['anchors.top'] = `${target}.top`
        if(!instructions.propertiesLazy['anchors.bottom']) instructions.propertiesLazy['anchors.bottom'] = `${target}.bottom`
    }


    if(instructions.propertiesLazy['anchors.fill']){
        delete instructions.properties['width']
        delete instructions.properties['height']
        delete instructions.properties['x']
        delete instructions.properties['y']

        let target = instructions.propertiesLazy['anchors.fill']
        delete instructions.propertiesLazy['anchors.fill']

        instructions.propertiesLazy['x'] = `anchors.leftMargin - (parent.left - ${target}.left)`
        instructions.propertiesLazy['y'] = `anchors.topMargin - (parent.top - ${target}.top)`
        instructions.propertiesLazy['width'] = `${target}.width - anchors.rightMargin - anchors.leftMargin`
        instructions.propertiesLazy['height'] = `${target}.height - anchors.bottomMargin - anchors.topMargin`
    } else if(instructions.propertiesLazy['anchors.centerIn']){
        delete instructions.properties['x']
        delete instructions.properties['y']

        let target = instructions.propertiesLazy['anchors.centerIn']
        delete instructions.propertiesLazy['anchors.centerIn']

        instructions.propertiesLazy['x'] = `${target}.width/2 - width/2 + anchors.leftMargin - anchors.rightMargin - (parent.left - ${target}.left)`
        instructions.propertiesLazy['y'] = `${target}.height/2 - height/2 + anchors.topMargin - anchors.bottomMargin - (parent.top - ${target}.top)`
    } else {
        if(instructions.propertiesLazy['anchors.horizontalCenter']){
            delete instructions.properties['x']

            let target = instructions.propertiesLazy['anchors.horizontalCenter']
            delete instructions.propertiesLazy['anchors.horizontalCenter']

            instructions.propertiesLazy['x'] = `${target} - width/2 + anchors.leftMargin - anchors.rightMargin - parent.left + anchors.horizontalCenterOffset`
        } else if(instructions.propertiesLazy['anchors.left'] && instructions.propertiesLazy['anchors.right']){
            delete instructions.properties['x']
            delete instructions.properties['width']

            let leftTarget = instructions.propertiesLazy['anchors.left']
            delete instructions.propertiesLazy['anchors.left']
            let rightTarget = instructions.propertiesLazy['anchors.right']
            delete instructions.propertiesLazy['anchors.right']

            instructions.propertiesLazy['x'] = `${leftTarget} + anchors.leftMargin - parent.left`
            instructions.propertiesLazy['width'] = `${rightTarget} - ${leftTarget} - anchors.rightMargin - anchors.leftMargin`
        } else {
            if(instructions.propertiesLazy['anchors.left']){
                delete instructions.properties['x']

                let target = instructions.propertiesLazy['anchors.left']
                delete instructions.propertiesLazy['anchors.left']

                let splitTarget = target.split(':')
                if(splitTarget.length > 1){
                    for(let i = 0; i < splitTarget.length; i++){
                        splitTarget[i] += '+ anchors.leftMargin - parent.left'
                    }
                    instructions.propertiesLazy['x'] = splitTarget.join(':')
                } else {
                    instructions.propertiesLazy['x'] = `${target} + anchors.leftMargin - parent.left`
                }
                //if(target.indexOf('parent') >= 0){
                    
                // } else {
                //     let obj = target.replaceAll('.left', '').replaceAll('.right', '')
                //     instructions.propertiesLazy['x'] = `${target} + anchors.leftMargin - (parent.left - (${obj}.parent.left))`
                // }
                
            }
            if(instructions.propertiesLazy['anchors.right']){
                delete instructions.properties['x']

                let target = instructions.propertiesLazy['anchors.right']
                delete instructions.propertiesLazy['anchors.right']

                let splitTarget = target.split(':')
                if(splitTarget.length > 1){
                    for(let i = 0; i < splitTarget.length; i++){
                        splitTarget[i] += '- width - anchors.rightMargin - parent.left'
                    }
                    instructions.propertiesLazy['x'] = splitTarget.join(':')
                } else {
                    instructions.propertiesLazy['x'] = `${target} - width - anchors.rightMargin - parent.left`
                }
                //if(target.indexOf('parent') >= 0){
                    
                // } else {
                //     let obj = target.replaceAll('.left', '').replaceAll('.right', '')
                //     instructions.propertiesLazy['x'] = `${target} - width - anchors.rightMargin - (parent.left - (${obj}.parent.left))`
                // }
                
            }
        }

        if(instructions.propertiesLazy['anchors.verticalCenter']){
            delete instructions.properties['y']

            let target = instructions.propertiesLazy['anchors.verticalCenter']
            delete instructions.propertiesLazy['anchors.verticalCenter']

            instructions.propertiesLazy['y'] = `${target} - height/2 + anchors.topMargin - anchors.bottomMargin - parent.top + anchors.verticalCenterOffset`
        } else if(instructions.propertiesLazy['anchors.top'] && instructions.propertiesLazy['anchors.bottom']){
            delete instructions.properties['y']
            delete instructions.properties['height']

            let topTarget = instructions.propertiesLazy['anchors.top']
            delete instructions.propertiesLazy['anchors.top']
            let bottomTarget = instructions.propertiesLazy['anchors.bottom']
            delete instructions.propertiesLazy['anchors.bottom']

            instructions.propertiesLazy['y'] = `${topTarget} + anchors.topMargin - parent.top`
            instructions.propertiesLazy['height'] = `${bottomTarget} - ${topTarget} - anchors.bottomMargin - anchors.topMargin`
        } else {
            if(instructions.propertiesLazy['anchors.top']){
                delete instructions.properties['y']

                let target = instructions.propertiesLazy['anchors.top']
                delete instructions.propertiesLazy['anchors.top']

                let splitTarget = target.split(':')
                if(splitTarget.length > 1){
                    for(let i = 0; i < splitTarget.length; i++){
                        splitTarget[i] += '+ anchors.topMargin - parent.top'
                    }
                    instructions.propertiesLazy['y'] = splitTarget.join(':')
                } else {
                    instructions.propertiesLazy['y'] = `${target} + anchors.topMargin - parent.top`
                }
                //if(target.indexOf('parent') >= 0){
                    
                // } else {
                //     let obj = target.replaceAll('.top', '').replaceAll('.bottom', '')
                //     instructions.propertiesLazy['y'] = `${target} + anchors.topMargin - (parent.top - (${obj}.parent.top))`
                // }
                
            }
            if(instructions.propertiesLazy['anchors.bottom']){
                delete instructions.properties['y']

                let target = instructions.propertiesLazy['anchors.bottom']
                delete instructions.propertiesLazy['anchors.bottom']

                let splitTarget = target.split(':')
                if(splitTarget.length > 1){
                    for(let i = 0; i < splitTarget.length; i++){
                        splitTarget[i] += '- height - anchors.bottomMargin - parent.top'
                    }
                    instructions.propertiesLazy['y'] = splitTarget.join(':')
                } else {
                    instructions.propertiesLazy['y'] = `${target} - height - anchors.bottomMargin - parent.top`
                }
                //if(target.indexOf('parent') >= 0){
                    
                // } else {
                //     let obj = target.replaceAll('.top', '').replaceAll('.bottom', '')
                //     instructions.propertiesLazy['y'] = `${target} - height - anchors.bottomMargin - (parent.top - (${obj}.parent.top))`
                // }
                
            }
        }
    }


    for(let name in instructions.propertiesQML){
        anchorsReplace(instructions.propertiesQML[name])
    }
    for(let name in instructions.propertiesQMLNew){
        anchorsReplace(instructions.propertiesQMLNew[name])
    }
    for(let name in instructions.propertiesSpecial){
        anchorsReplace(instructions.propertiesSpecial[name])
    }
    for(let child of instructions.children){
        anchorsReplace(child)
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

    
    IDReplace(compiledFiles[file].instructions)
    
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

    if(instructions.class === 'Column' || instructions.class === 'Row' || instructions.class === 'Image'){
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
    if(instructions.class === 'Flickable' || instructions.class === 'ListView' || instructions.class === 'GridView' || instructions.class === 'Text' || instructions.class === 'TextInput'){
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

    for(let prop in instructions.properties){
        let val = instructions.properties[prop]
        if(instructions.class === 'ListElement'){
            code.push(`${curr}.$cP(\`${prop}\`,${val})`)
        } else {
            code.push(`${curr}.${prop}=${val}`)
        }
        
    }
    for(let prop in instructions.propertiesLazy){
        let val = instructions.propertiesLazy[prop]
        code.push(`${curr}.$sP(\`${prop}\`, function(){with(this)with(QML){return ${val}}}.bind(${curr}))`)
    }
    for(let prop in instructions.propertiesNew){
        let val = instructions.propertiesNew[prop]
        code.push(`${curr}.$cP(\`${prop}\`,${val})`)
    }
    for(let prop in instructions.propertiesLazyNew){
        let val = instructions.propertiesLazyNew[prop]
        code.push(`${curr}.$cP(\`${prop}\`,function(){with(this)with(QML){return ${val}}}.bind(${curr}))`)
    }
    for(let prop in instructions.propertiesAlias){
        let val = instructions.propertiesAlias[prop]
        code.push(`${curr}.$cA(\`${prop}\`,function(){with(this)with(QML){return ${val}}}.bind(${curr}),function(nVal){with(this)with(QML){${val}=nVal}}.bind(${curr}))`)
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
        code.push(`${curr}.${name}=function(${instructions.methods[name].params.join(',')}){let $args = {${instructions.methods[name].params.join(',')}};with(this)with($args)with(QML){${instructions.methods[name].source}}}.bind(${curr})`)
    }
    for(let name in instructions.defineSignals){
        if(instructions.defineSignals[name].length){
            code.push(`${curr}.$cS(\`${name}\`,${instructions.defineSignals[name].join(',')})`)
        } else {
            code.push(`${curr}.$cS(\`${name}\`)`)
        }
        
    }
    for(let signal of instructions.connectionSignals){
        code.push(`${curr}.$s['${signal.name}'].connect(function(){with(this)with(QML)with(this.$s['${signal.name}'].context){${signal.source}}}.bind(${curr}))`)
    }
    
    // code.push(`${curr}.$tryComplete()`)
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

        fs.writeFile(source + '/cache/' + crypto.createHash('md5').update(file).digest("hex"), compiledFiles[file].currentHash + '\n' + tempIDList.join(','), function(error){
            if(error) throw error
        })
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
