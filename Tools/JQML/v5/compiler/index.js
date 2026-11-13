// QML -> JS compiler. Output <name>.js = dist/core.js (the runtime) + generated code + bootstrap.
const fs = require('fs')
const path = require('path')
const parser = require('./parser')
const { FileModel } = require('./model')
const { Codegen } = require('./codegen')
const { loadConfig } = require('./config')

function readFileModel(fileName){
    let source = fs.readFileSync(fileName, 'utf8')
    parser.parse.nowParsingFile = fileName
    let ast = parser.parse(source)
    let name = path.basename(fileName, '.qml')
    let model = new FileModel(name, fileName).build(ast)
    model.createsComponents = source.includes('createComponent')
    return model
}

// qmldir lines: "module Name", "Type 1.0 File.qml", "singleton Type 1.0 File.qml"
function readQmldir(dir){
    let qmldir = path.resolve(dir, 'qmldir')
    let entries = []
    entries.module = ''
    if(!fs.existsSync(qmldir)) return entries
    for(let line of fs.readFileSync(qmldir, 'utf8').split(/\r?\n/)){
        let parts = line.trim().split(/\s+/)
        if(parts[0] === 'module') entries.module = parts[1] || ''
        if(!parts[0] || parts[0] === 'module' || parts[0].startsWith('#')) continue
        let singleton = parts[0] === 'singleton'
        if(singleton) parts.shift()
        if(parts.length >= 3 && parts[2].endsWith('.qml')) entries.push({ type: parts[0], file: path.resolve(dir, parts[2]), singleton })
        else if(parts.length >= 3 && parts[2].endsWith('.js')) entries.push({ type: parts[0], file: path.resolve(dir, parts[2]), script: true })
    }
    return entries
}

const pathKey = (fileName) => {
    let key = path.normalize(path.resolve(fileName))
    return process.platform === 'win32' ? key.toLowerCase() : key
}

// module: { name, root } of a module dir, or null for a plain dir (the entry's, an imported one). The files
// are the qmldir ones, or every .qml with all or when the dir has no qmldir. A file's url is its qrc path
// under qml/: relative to the root of the module that holds it, or to the entry's dir.
function addDir(files, dir, module, modules, entryDir, all = false){
    let entries = readQmldir(dir)
    let byFile = new Map(entries.map(e => [path.normalize(e.file), e]))
    for(let e of entries){
        if(e.script && fs.existsSync(e.file)) files.$scripts.set(e.type, e.file)
    }
    entries = entries.filter(e => !e.script)
    let names = !all && (module || fs.existsSync(path.resolve(dir, 'qmldir'))) ? entries.map(e => e.file) : fs.readdirSync(dir).filter(n => n.endsWith('.qml')).map(n => path.resolve(dir, n))
    let holder = module || modules.find(m => !path.relative(m.root, dir).startsWith('..') && !path.isAbsolute(path.relative(m.root, dir)))
    for(let fileName of names){
        let key = pathKey(fileName)
        if(!fs.existsSync(fileName) || files.$paths.has(key)) continue
        files.$paths.add(key)
        let entry = byFile.get(path.normalize(fileName))
        let model = readFileModel(fileName)
        if(entry){
            model.name = entry.type
            if(entry.singleton) model.singleton = true
        }
        model.dir = pathKey(dir)
        model.module = module ? module.name : ''
        let relative = path.relative(holder ? holder.root : entryDir, fileName).split(path.sep).join('/')
        model.url = holder ? holder.name + '/' + relative : relative
        model.fromModule = !!module
        files.set(model.url, model)
        if(module){
            if(!files.$modules.has(module.name)) files.$modules.set(module.name, new Map())
            files.$modules.get(module.name).set(model.name, model)
        }
    }
}

// Types visible to the entry: module dirs of the config (qmldir), every .qml next to the entry and the dirs
// files import by path. files: url -> FileModel; $modules: module name -> (type -> FileModel);
// $scripts: qmldir qualifier -> .js file. A dir import gets dirKey, the key of FileModel.dir.
function collectFiles(entry, configPath){
    let files = new Map()
    files.$scripts = new Map()
    files.$modules = new Map()
    files.$paths = new Set()
    let dirs = configPath && fs.existsSync(configPath) ? loadConfig(configPath).filter(dir => fs.existsSync(dir)) : []
    // a module may span several dirs with their own qmldir; urls are relative to the topmost one
    let modules = dirs.map(dir => ({ name: readQmldir(dir).module || path.basename(dir), root: path.resolve(dir) }))
    for(let module of modules){
        for(let other of modules){
            if(other.name === module.name && other.root.length < module.root.length) module.root = other.root
        }
    }
    let entryDir = path.dirname(entry)
    let added = new Set(dirs.map(pathKey))
    dirs.forEach((dir, i) => addDir(files, dir, modules[i], modules, entryDir))
    addDir(files, entryDir, null, modules, entryDir, true)
    added.add(pathKey(entryDir))
    for(let file of files.values()){
        for(let imp of file.imports){
            let target = String(imp.path || '').replace(/^["']|["']$/g, '')
            if(imp.isModule || target.endsWith('.js') || /^[a-z]+:/i.test(target)) continue
            let dir = path.resolve(path.dirname(file.fileName), target)
            imp.dirKey = pathKey(dir)
            if(added.has(imp.dirKey) || !fs.existsSync(dir)) continue
            added.add(imp.dirKey)
            addDir(files, dir, null, modules, entryDir)
        }
    }
    return files
}

// The .qml files compile() reads (--list-qml, the build's dependencies): those declared by the qmldir of every
// module dir of the config, and the ones next to the entry when its dir is not one of them
function listQmlFiles(configPath, entry = ''){
    let files = []
    let seen = new Set()
    let remember = (fileName) => {
        let absolute = path.resolve(fileName).replace(/\\+/g, '/')
        let key = process.platform === 'win32' ? absolute.toLowerCase() : absolute
        if(seen.has(key)) return
        seen.add(key)
        files.push(absolute)
    }
    let entryDir = entry ? path.dirname(path.resolve(path.dirname(path.resolve(configPath)), entry)) : ''
    let entryIsModule = false
    for(let dir of loadConfig(configPath)){
        if(entryDir && path.resolve(dir) === entryDir) entryIsModule = true
        if(!fs.existsSync(path.resolve(dir, 'qmldir'))) throw new Error('no qmldir in ' + dir)
        for(let e of readQmldir(dir)) if(!e.script) remember(e.file)
    }
    if(entryDir && !entryIsModule){
        for(let name of fs.readdirSync(entryDir)){
            if(name.endsWith('.qml') && fs.statSync(path.resolve(entryDir, name)).isFile()) remember(path.resolve(entryDir, name))
        }
    }
    return files
}

function html(options){
    let icon = options.icon ? `<link rel="icon" href="${options.icon}">` : ''
    return `<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>${options.name}</title>
${icon}
<script src="${options.name}.js"></script>
</head>
<body></body>
</html>
`
}

// Escapes chars above Latin-1 so the browser keeps the script one byte per char
function escapeNonLatin1(code){
    return code.replace(/[Ā-￿]/g, ch => '\\u' + ch.charCodeAt(0).toString(16).padStart(4, '0'))
}

function compile(options){
    if(options.output) fs.mkdirSync(path.resolve(options.output), { recursive: true })
    if(options.mode === 'html'){
        fs.writeFileSync(path.resolve(options.output, options.name + '.html'), html(options))
        return
    }

    let entry = path.resolve(options.entry)
    let files = collectFiles(entry, options.config)
    let entryFile = [...files.values()].find(file => path.normalize(file.fileName).toLowerCase() === path.normalize(entry).toLowerCase())
    let codegen = new Codegen(files, entryFile)
    let code = codegen.generate()
    for(let warning of codegen.warnings) console.log(warning)

    let runtime = fs.readFileSync(path.resolve(__dirname, '../dist/core.js'), 'utf8')
    let bootstrap = `
window.addEventListener('load', () => {
    // resource root (-r): the page path up to and including it, as the current runtime sets rootPath
    const base = ${JSON.stringify(options.root || '/')}
    JQ.rootPath = location.pathname.indexOf(base) >= 0 ? location.origin + location.pathname.slice(0, location.pathname.indexOf(base) + base.length) : base
    // the window's contentItem, sized to the viewport; the root object is its child (QQuickView)
    const content = new JQ.types.Item(null)
    const fit = () => JQ.batch(() => {
        content.width = window.innerWidth
        content.height = window.innerHeight
    })
    fit()
    window.addEventListener('resize', fit)
    content.$mount(document.body)
    const root = JQ.create(() => new (${entryFile.typeRef.ref}$())(content))
    window.__root = root
})`
    let output = runtime + '\n;(function(){\n' + code + '\n' + bootstrap + '\n})();\n'
    fs.writeFileSync(path.resolve(options.output, options.name + '.js'), escapeNonLatin1(output))
    return { warnings: codegen.warnings }
}

module.exports = { compile, listQmlFiles }
