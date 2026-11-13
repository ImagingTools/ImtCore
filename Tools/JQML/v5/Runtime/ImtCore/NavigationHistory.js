// The web counterpart of imtcontrols' NavigationHistory singleton: the browser history, a path per entry after
// "#/" (Devices/Device/12345), so the back and forward buttons, reloading and links reach the same view
const engine = require('../Core/engine')
const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')

function pathOf(hash){
    return String(hash || '').replace(/^#\/?/, '').split('/').map(segment => {
        try {
            return decodeURIComponent(segment)
        } catch(error) {
            return segment
        }
    }).join('/')
}

const hashOf = (path) => '#/' + String(path).split('/').map(encodeURIComponent).join('/')

class NavigationHistory extends QtObject {
    constructor(){
        super(null)
        if(typeof window === 'undefined') return
        this.$v[NavigationHistory.AVAILABLE] = true
        this.$v[NavigationHistory.PATH] = pathOf(window.location.hash)
        window.addEventListener('popstate', () => engine.batch(() => {
            let path = pathOf(window.location.hash)
            this.$write(NavigationHistory.PATH, path)
            engine.emitArgs(this, NavigationHistory.SIGNAL_popped, [path])
        }))
    }

    push(path){
        this.$go('pushState', path)
    }

    replace(path){
        this.$go('replaceState', path)
    }

    $go(method, path){
        if(!this.$v[NavigationHistory.AVAILABLE]) return
        path = String(path || '')
        if(path === this.$v[NavigationHistory.PATH] && method === 'pushState') return
        window.history[method](null, '', path ? hashOf(path) : window.location.pathname + window.location.search)
        this.$write(NavigationHistory.PATH, path)
    }

    back(){
        if(this.$v[NavigationHistory.AVAILABLE]) window.history.back()
    }

    forward(){
        if(this.$v[NavigationHistory.AVAILABLE]) window.history.forward()
    }
}

defineType(NavigationHistory, {
    props: {
        available: { kind: 'bool', readonly: true },
        path: { kind: 'string', readonly: true },
    },
    signals: ['popped(path)'],
})
NavigationHistory.$singleton = true

module.exports = { NavigationHistory }
