// Command line of the compiler, the same as JQML v3 (ImtCore Config/CMake/WebCompiler.cmake):
//   compiler.js -c <config.json> -e <entry.qml> -o <outDir> -n <name> -r <root> -m js
//   compiler.js -n <name> -i <icon> -o <outDir> -m html
//   compiler.js --list-qml -c <config.json> [-e <entry.qml>]
// Parsed here without packages, so the compiler needs only uglify-js.
const { compile, listQmlFiles } = require('./index')

const OPTIONS = [
    ['c', 'config', '<path>', 'config file path'],
    ['o', 'output', '<path>', 'output dir path'],
    ['e', 'entry', '<path>', 'entry qml file path'],
    ['i', 'icon', '<path>', 'icon file path for compile mode html'],
    ['n', 'name', '<string>', 'application name (default: "")'],
    ['r', 'root', '<string>', 'root path (default: "/")'],
    ['m', 'mode', '<string>', 'compile mode (choices: "js", "html")'],
    ['', 'list-qml', '', 'print qml files declared by config qmldirs, one path per line'],
]

const usage = () => 'Usage: compiler [options]\n\nOptions:\n' + OPTIONS.map(([s, l, arg, text]) =>
    ('  ' + (s ? '-' + s + ', ' : '') + '--' + l + (arg ? ' ' + arg : '')).padEnd(24) + text).join('\n') + '\n'

function parse(args){
    let options = { name: '', root: '/' }
    for(let i = 0; i < args.length; i++){
        let arg = args[i], value
        let eq = arg.startsWith('--') ? arg.indexOf('=') : -1
        if(eq > 0){
            value = arg.slice(eq + 1)
            arg = arg.slice(0, eq)
        }
        if(arg === '-h' || arg === '--help') return null
        let option = OPTIONS.find(([s, l]) => arg === '--' + l || (s && arg === '-' + s))
        if(!option) throw new Error("unknown option '" + arg + "'")
        let key = option[1] === 'list-qml' ? 'listQml' : option[1]
        if(!option[2]){
            options[key] = true
            continue
        }
        if(value === undefined){
            if(i + 1 >= args.length) throw new Error("option '" + arg + "' needs a value")
            value = args[++i]
        }
        options[key] = value
    }
    if(options.mode !== undefined && options.mode !== 'js' && options.mode !== 'html')
        throw new Error("option '-m, --mode' argument '" + options.mode + "' is invalid; allowed: js, html")
    return options
}

try {
    let options = parse(process.argv.slice(2))
    if(options && options.listQml){
        if(!options.config) throw new Error('--config is required with --list-qml')
        let files = listQmlFiles(options.config, options.entry || '')
        process.stdout.write(files.join('\n') + (files.length ? '\n' : ''))
    } else if(options && (options.mode === 'js' || options.mode === 'html')){
        if(options.mode === 'js' && !options.entry) throw new Error('--entry is required with -m js')
        compile(options)
    } else {
        process.stdout.write(usage())
    }
} catch(error) {
    console.error('JQML: ' + (error && error.message ? error.message : error))
    process.exit(1)
}
