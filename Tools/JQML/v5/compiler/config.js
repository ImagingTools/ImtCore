// Compiler config: { dirs: [...], includes: [...] }, ${ENV} substitution, includes merged recursively.
// Same rules as loadCompilerConfig of compiler/compiler.js.
const fs = require('fs')
const path = require('path')

function envFill(source, env){
    if(source.indexOf('${') < 0) return source
    let result = source
    for(let key of Object.keys(env).sort((a, b) => b.length - a.length)){
        let token = '${' + key + '}'
        if(result.indexOf(token) >= 0) result = result.split(token).join(String(env[key]).replace(/\\/g, '\\\\').trim())
    }
    return result
}

function envFillPath(filePath, env){
    let result = String(filePath || '')
    if(result.indexOf('${') < 0) return result
    for(let key of Object.keys(env).sort((a, b) => b.length - a.length)){
        if(env[key] == null) continue
        let token = '${' + key + '}'
        if(result.indexOf(token) >= 0) result = result.split(token).join(String(env[key]).trim())
    }
    return result
}

function isAbsolutePath(filePath){
    return !!filePath && (path.isAbsolute(filePath) || /^[A-Za-z]:[\\/]/.test(filePath) || filePath.slice(0, 2) === '\\\\')
}

function resolveRef(baseDir, filePath, env){
    let filled = envFillPath(filePath, env)
    if(isAbsolutePath(filled) || filled.indexOf('${') >= 0) return path.normalize(filled)
    return path.resolve(baseDir, filled)
}

// Returns absolute module dirs: those of included configs first, like the old compiler
function loadConfig(configPath, env = process.env){
    let read = (file) => JSON.parse(envFill(fs.readFileSync(file, 'utf8'), env))
    let collect = (config, baseDir) => {
        let dirs = []
        for(let include of config.includes || []){
            let includePath = resolveRef(baseDir, include, env)
            dirs.unshift(...collect(read(includePath), path.dirname(includePath)))
        }
        for(let dir of config.dirs || []) dirs.push(resolveRef(baseDir, dir, env))
        return dirs
    }
    return collect(read(configPath), path.dirname(path.resolve(configPath)))
}

module.exports = { loadConfig }
