

// ---- Loader -------------------------------------------------------------------------------

const files = {}        // qrc path under qml/ ("imtgui/Panels/X.qml") -> class
let byName = null       // file name -> paths, built on the first lookup by name

function normalize(url){
    let parts = []
    for(let part of url.split('/')){
        if(part === '..') parts.pop()
        else if(part && part !== '.') parts.push(part)
    }
    return parts.join('/')
}

// "qrc:/qml/imtgui/X.qml", "imtgui/X.qml" or a url relative to base, the file that asks for it
// (Qt resolves against the caller's context); otherwise the file of that name whose path ends like
// the url, the last one registered
// a type compiled lazily is registered as a function that defines it on the first lookup
function load(T){
    return T && T.$lazy ? T() : T
}

function findFile(url, base = ''){
    return load(find(url, base))
}

function find(url, base){
    let text = String(url).replace(/[?#].*$/, '')
    let absolute = /^[a-z]+:/i.test(text) || text.startsWith('/')
    let key = normalize(text.replace(/^[a-z]+:/i, '').replace(/^\/*(qml\/)?/, ''))
    if(!absolute && base){
        let resolved = normalize(base.slice(0, base.lastIndexOf('/') + 1) + text)
        if(files[resolved]) return files[resolved]
    }
    if(files[key]) return files[key]
    if(!byName){
        byName = {}
        for(let path of Object.keys(files)){
            let name = path.slice(path.lastIndexOf('/') + 1)
            if(!byName[name]) byName[name] = []
            byName[name].push(path)
        }
    }
    let paths = byName[key.slice(key.lastIndexOf('/') + 1)]
    if(!paths) return null
    let matching = paths.filter(path => path.endsWith('/' + key))
    if(!matching.length) matching = paths
    return files[matching[matching.length - 1]]
}

// Qt.createComponent: the url resolved like QQmlContext::resolvedUrl against base, the file that asks for it, and
// looked up in the bundle only there. The bundle stands for the qrc:/qml/ resources: { url, key, type } with the
// absolute url, the bundle path and the registered type (null when the bundle has no such file).
function resolveExact(url, base = ''){
    let text = String(url)
    let suffix = (text.match(/[?#].*$/) || [''])[0]
    let path = text.slice(0, text.length - suffix.length)
    let key = null
    let scheme = (path.match(/^([a-z][a-z0-9+.-]*):/i) || [])[1]
    if(scheme){
        if(scheme.toLowerCase() !== 'qrc') return { url: text, key: null, type: null }
        let rest = path.slice(scheme.length + 1).replace(/^\/+/, '')
        key = rest.startsWith('qml/') ? normalize(rest.slice(4)) : null
    } else if(path.startsWith('/')){
        let rest = path.replace(/^\/+/, '')
        key = rest.startsWith('qml/') ? normalize(rest.slice(4)) : null
    } else {
        key = normalize(base.slice(0, base.lastIndexOf('/') + 1) + path)
    }
    if(key === null) return { url: 'qrc:' + (path.startsWith('/') ? path : '/' + path) + suffix, key: null, type: null }
    return { url: 'qrc:/qml/' + key + suffix, key, type: files[key] || null }
}

module.exports = { files, findFile, load, resolveExact }
