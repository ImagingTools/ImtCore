

// Resource urls as the current runtime serves them: rootPath + the path without qrc:/ and ../
function resolveUrl(url){
    url = String(url || '')
    if(/^(data:|blob:|https?:|file:)/.test(url)) return url
    let root = (typeof runtimeRoot === 'function' ? runtimeRoot() : '') || ''
    let path = url.split('qrc:/').join('').split('../').join('')
    if(!path.replace(/\//g, '')) return ''
    return (root + path).replace(/([^:])\/{2,}/g, '$1/')
}
let rootProvider = null
function runtimeRoot(){ return rootProvider ? rootProvider() : '' }

function setRootProvider(fn){
    rootProvider = fn
}

module.exports = { resolveUrl, setRootProvider }
