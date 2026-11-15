// The web counterpart of CClientUserContextComp, the root context property "context" (core/JQApplication.js):
// appName, language, application, location; translations come from ../Translations/<application>_<language>.ts,
// loaded in the background
const engine = require('../Core/engine')
const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')

class WebContext extends QtObject {
    constructor(){
        super(null)
        this.$languages = {}
        this.$dict = null
        if(typeof window !== 'undefined') this.$v[WebContext.LOCATION] = window.location
    }

    // qsTr bindings are evaluated again when the translations change (QQmlEngine::retranslate)
    translate(sourceText){
        engine.track(this, WebContext.SIGNAL_translationsChanged)
        let result = this.$dict ? this.$dict[sourceText] : undefined
        return result ? result : sourceText
    }

    // The .ts files load in the background; until they are there the texts keep the previous language
    $loadLanguage(){
        let language = this.$v[WebContext.LANGUAGE]
        if(!language) return
        if(this.$languages[language]){
            this.$useLanguage(language)
            return
        }
        if(typeof fetch === 'undefined' || typeof DOMParser === 'undefined') return
        let application = this.$v[WebContext.APPLICATION]
        let prefixes = typeof application === 'string' ? [application] : Array.isArray(application) ? application : []
        let load = (prefix) => fetch(prefix ? '../Translations/' + prefix + '_' + language + '.ts' : '../Translations/' + language + '.ts')
            .then(response => response.ok ? response.text() : '')
            .catch(() => '')
        Promise.all(prefixes.map(load)).then(texts => {
            let dict = {}
            for(let text of texts){
                if(!text) continue
                let xml = new DOMParser().parseFromString(text, 'text/xml')
                for(let message of xml.getElementsByTagName('message')){
                    let source = message.getElementsByTagName('source')[0]
                    let translation = message.getElementsByTagName('translation')[0]
                    if(source && translation && translation.textContent) dict[source.textContent] = translation.textContent
                }
            }
            this.$languages[language] = dict
            if(this.$v[WebContext.LANGUAGE] === language) this.$useLanguage(language)
        })
    }

    $useLanguage(language){
        if(this.$dict === this.$languages[language]) return
        this.$dict = this.$languages[language]
        engine.batch(() => engine.emit(this, WebContext.SIGNAL_translationsChanged))
    }

    $write(id, value){
        if(id === WebContext.LANGUAGE){
            let old = this.$v[id]
            if(old === value) return
            this.$v[id] = value
            this.$loadLanguage()
            engine.emit(this, WebContext.SIGNAL_languageChanged, old, value)
            return
        }
        super.$write(id, value)
    }
}

defineType(WebContext, {
    props: {
        appName: 'string',
        language: 'string',
        application: { kind: 'var', initial: [] },
        location: 'var',
    },
    signals: ['translationsChanged'],
})

module.exports = { WebContext }
