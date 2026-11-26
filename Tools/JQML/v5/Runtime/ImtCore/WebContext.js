// The web counterpart of CClientUserContextComp, the root context property "context" (core/JQApplication.js):
// appName, language, application, location; translations come from ../Translations/<application>_<language>.ts,
// loaded in the background
const engine = require('../Core/engine')
const { QtObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')
const Translations = require('../Core/Translations')

class WebContext extends QtObject {
    constructor(){
        super(null)
        this.$languages = {}
        this.$dict = null
        if(typeof window !== 'undefined') this.$v[WebContext.LOCATION] = window.location
    }

    // qsTr bindings are evaluated again when the translations change (QQmlEngine::retranslate)
    translate(sourceText){
        return this.translateIn(null, sourceText)
    }

    translateIn(context, sourceText, disambiguation, n){
        engine.track(this, WebContext.SIGNAL_translationsChanged)
        return Translations.translate(this.$dict, context, sourceText, disambiguation, n)
    }

    translateId(id, n){
        engine.track(this, WebContext.SIGNAL_translationsChanged)
        return Translations.translateId(this.$dict, id, n)
    }

    // The .ts files load in the background; until they are there the texts keep the previous language
    $loadLanguage(){
        let language = this.$v[WebContext.LANGUAGE]
        if(!language) return
        if(this.$languages[language]){
            this.$useLanguage(language)
            return
        }
        if(typeof fetch === 'undefined') return
        let application = this.$v[WebContext.APPLICATION]
        let prefixes = typeof application === 'string' ? [application] : Array.isArray(application) ? application : []
        let load = (prefix) => fetch(prefix ? '../Translations/' + prefix + '_' + language + '.ts' : '../Translations/' + language + '.ts')
            .then(response => response.ok ? response.text() : '')
            .catch(() => '')
        Promise.all(prefixes.map(load)).then(texts => {
            let catalog = Translations.emptyCatalog()
            for(let text of texts) if(text) Translations.parseTs(text, catalog)
            if(!catalog.language) catalog.language = language
            this.$languages[language] = catalog
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
