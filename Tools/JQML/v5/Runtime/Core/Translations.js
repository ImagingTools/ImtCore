// Translations as QTranslator has them (qtranslator.cpp, translator.cpp of lrelease): the messages of .ts files by their
// context, source text and disambiguation (comment); a numerus message has a form per plural category of the language.
// QCoreApplication::translate: with n the form is chosen by n and %n / %Ln are replaced, also without a translation.

const ENTITIES = { lt: '<', gt: '>', amp: '&', quot: '"', apos: "'" }

function decode(text){
    return text.replace(/&(#x[0-9a-f]+|#\d+|[a-z]+);/gi, (all, entity) => {
        if(entity[0] === '#') return String.fromCodePoint(entity[1] === 'x' || entity[1] === 'X' ? parseInt(entity.slice(2), 16) : parseInt(entity.slice(1), 10))
        return ENTITIES[entity] !== undefined ? ENTITIES[entity] : all
    })
}

// the text of the first element `name` in xml, null when there is none
function element(xml, name){
    let match = new RegExp('<' + name + '(\\s[^>]*)?(/>|>([\\s\\S]*?)</' + name + '>)').exec(xml)
    if(!match) return null
    return { attributes: match[1] || '', body: match[3] === undefined ? '' : match[3] }
}

function attribute(attributes, name){
    let match = new RegExp('\\s' + name + '\\s*=\\s*"([^"]*)"').exec(attributes)
    return match ? decode(match[1]) : null
}

const key = (source, disambiguation) => source + '\u0004' + (disambiguation || '')

function emptyCatalog(){
    return { language: '', contexts: new Map(), ids: new Map() }
}

// The messages of a .ts file into the catalog; untranslated and obsolete / vanished messages are left out (lrelease)
function parseTs(text, catalog = emptyCatalog()){
    let ts = element(text, 'TS')
    if(!ts) return catalog
    let language = attribute(ts.attributes, 'language')
    if(language && !catalog.language) catalog.language = language
    let contextRe = /<context>([\s\S]*?)<\/context>/g
    let context
    while((context = contextRe.exec(ts.body))){
        let nameElement = element(context[1], 'name')
        let contextName = nameElement ? decode(nameElement.body) : ''
        let messages = catalog.contexts.get(contextName)
        if(!messages) catalog.contexts.set(contextName, messages = new Map())
        let messageRe = /<message(\s[^>]*)?>([\s\S]*?)<\/message>/g
        let message
        while((message = messageRe.exec(context[1]))){
            let attributes = message[1] || ''
            let body = message[2]
            let source = element(body, 'source')
            let translation = element(body, 'translation')
            if(!source || !translation) continue
            let type = attribute(translation.attributes, 'type')
            if(type === 'obsolete' || type === 'vanished') continue
            let comment = element(body, 'comment')
            let value
            if(attribute(attributes, 'numerus') === 'yes'){
                let forms = []
                let formRe = /<numerusform(\s[^>]*)?>([\s\S]*?)<\/numerusform>|<numerusform(\s[^>]*)?\/>/g
                let form
                while((form = formRe.exec(translation.body))) forms.push(decode(form[2] || ''))
                if(!forms.some((f) => f)) continue
                value = forms
            } else {
                value = decode(translation.body)
                if(!value) continue
            }
            messages.set(key(decode(source.body), comment ? decode(comment.body) : ''), value)
            let id = attribute(attributes, 'id')
            if(id) catalog.ids.set(id, value)
        }
    }
    return catalog
}

// QTranslatorPrivate::do_translate: the message of the context with the disambiguation, then without it. Without a
// context (a qsTr of a JavaScript file) the first context that has the message
function lookup(catalog, context, source, disambiguation){
    if(!catalog) return undefined
    let find = (messages) => {
        if(!messages) return undefined
        let found = messages.get(key(source, disambiguation))
        if(found === undefined && disambiguation) found = messages.get(key(source, ''))
        return found
    }
    if(context !== null && context !== undefined) return find(catalog.contexts.get(String(context)))
    for(let messages of catalog.contexts.values()){
        let found = find(messages)
        if(found !== undefined) return found
    }
    return undefined
}

const CATEGORIES = ['zero', 'one', 'two', 'few', 'many', 'other']

// the numerus form of n: the plural categories of the language in the CLDR order are the forms of Qt's rules in the
// languages of the translations (en, de, it, pt, sv, ru, pl, ar, kk, zh); a category past the forms takes the last one
function pluralForm(forms, n, language){
    if(forms.length <= 1) return forms[0] || ''
    let tag = String(language || 'en').replace('_', '-')
    let rules
    try {
        rules = new Intl.PluralRules(tag)
    } catch(error) {
        rules = new Intl.PluralRules('en')
    }
    let categories = CATEGORIES.filter((c) => rules.resolvedOptions().pluralCategories.includes(c))
    let index = categories.indexOf(rules.select(Math.abs(n)))
    let form = forms[Math.min(index < 0 ? forms.length - 1 : index, forms.length - 1)]
    return form || forms.find((f) => f) || ''
}

// QCoreApplication::translate: %n is n, %Ln n in the format of the locale
function replacePercentN(text, n, language){
    if(n === undefined || n === null || n === -1 || typeof text !== 'string' || text.indexOf('%') < 0) return text
    let number = Number(n)
    let localized = (() => {
        try {
            return number.toLocaleString(String(language || '').replace('_', '-') || undefined)
        } catch(error) {
            return String(number)
        }
    })()
    return text.replace(/%L?n/g, (match) => match === '%Ln' ? localized : String(number))
}

function translate(catalog, context, source, disambiguation, n){
    source = String(source)
    let found = lookup(catalog, context, source, disambiguation === undefined || disambiguation === null ? '' : String(disambiguation))
    let text = found === undefined ? source : Array.isArray(found) ? pluralForm(found, n === undefined || n === null || n === -1 ? 1 : n, catalog && catalog.language) : found
    return replacePercentN(text, n, catalog && catalog.language)
}

function translateId(catalog, id, n){
    let found = catalog ? catalog.ids.get(String(id)) : undefined
    let text = found === undefined ? String(id) : Array.isArray(found) ? pluralForm(found, n === undefined || n === -1 ? 1 : n, catalog.language) : found
    return replacePercentN(text, n, catalog && catalog.language)
}

module.exports = { emptyCatalog, parseTs, lookup, pluralForm, replacePercentN, translate, translateId }
