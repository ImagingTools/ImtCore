const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { findFile } = require('../QtQml/files')
const { Item } = require('./Item')

function componentForSource(source, base){
    let Class = findFile(source, base)
    if(!Class) return null
    return { createObject: (parent) => new Class(parent) }
}

class Loader extends Item {
    constructor(parent = null){
        super(parent)
        this.$sourceComponent = null
        this.$properties = null
        this.$base = ''         // url of the file declaring the Loader
        engine.scheduleComponentComplete(this)
    }

    // "If source or sourceComponent changes after this function is called but prior to the Loader
    // becoming active, the cached properties will be cleared." (Qt docs)
    setSource(source, properties){
        this.$settingSource = true
        try {
            this.source = source
        } finally {
            this.$settingSource = false
        }
        this.$properties = properties || null
        if(this.$v[Loader.ACTIVE] && this.$f & engine.COMPLETED && !this.$v[Loader.ITEM]) this.$load()
        else if(this.$v[Loader.ACTIVE] && this.$f & engine.COMPLETED) this.$load()
    }

    $sourceChanged(){
        if(!this.$settingSource) this.$properties = null
        if(this.$f & engine.COMPLETED && !this.$settingSource) this.$load()
    }

    // "If an explicit size is not specified for the Loader, the Loader is automatically resized to the size
    // of the loaded item ... If the size of the Loader is specified explicitly ... the loaded item will be
    // resized to the size of the Loader." Both keep following later changes (Loader_006).
    $followItem(item){
        for(let link of this.$sizeLinks || []) engine.unsubscribe(link)
        this.$sizeLinks = []
        if(!(item instanceof Item)) return
        let explicitWidth = this.$explicit & 1, explicitHeight = this.$explicit & 2
        if(explicitWidth) item.width = this.width
        else this.$write(Item.IMPLICITWIDTH, item.width)
        if(explicitHeight) item.height = this.height
        else this.$write(Item.IMPLICITHEIGHT, item.height)
        let geometry = engine.PRIORITY_GEOMETRY
        this.$sizeLinks.push(explicitWidth
            ? engine.connect(this, Item.$type.props[Item.WIDTH].signal, () => { item.width = this.width }, null, geometry)
            : engine.connect(item, Item.$type.props[Item.WIDTH].signal, () => this.$write(Item.IMPLICITWIDTH, item.width), null, geometry))
        this.$sizeLinks.push(explicitHeight
            ? engine.connect(this, Item.$type.props[Item.HEIGHT].signal, () => { item.height = this.height }, null, geometry)
            : engine.connect(item, Item.$type.props[Item.HEIGHT].signal, () => this.$write(Item.IMPLICITHEIGHT, item.height), null, geometry))
    }

    $load(){
        let old = this.$v[Loader.ITEM]
        if(old){
            this.$write(Loader.ITEM, null)
            old.$releaseLater(true, true)
        }
        if(!this.$v[Loader.ACTIVE]){
            this.$write(Loader.STATUS, 0)
            return
        }
        let component = this.$v[Loader.SOURCECOMPONENT]
        let source = this.$v[Loader.SOURCE]
        if(!component && source){
            component = componentForSource(source, this.$base)
            if(!component){
                engine.reportError('cannot load ' + source, 'Loader')
                this.$write(Loader.STATUS, 3)
                return
            }
        }
        if(!component){
            this.$write(Loader.STATUS, 0)
            return
        }
        let item = null
        let previous = engine.beginCreate()
        try {
            item = component.createObject(this)
            if(item && this.$properties){
                for(let key in this.$properties) item[key] = this.$properties[key]
            }
            engine.creatingRoot(item)
        } finally {
            engine.endCreate(previous)
        }
        this.$properties = null
        this.$followItem(item)
        this.$write(Loader.ITEM, item)
        this.$write(Loader.STATUS, 1)
        engine.emitArgs(this, Loader.SIGNAL_loaded, [])
    }

    $componentComplete(){
        super.$componentComplete()
        this.$load()
    }
}

const reload = (loader) => { if(loader.$f & engine.COMPLETED) loader.$load() }
const sourceChanged = (loader) => loader.$sourceChanged()

defineType(Loader, {
    props: {
        source: { kind: 'string', effect: sourceChanged },
        sourceComponent: { kind: 'component', effect: sourceChanged },
        active: { kind: 'bool', initial: true, effect: reload },
        item: { kind: 'var', initial: null },
        status: 'int',
        progress: { kind: 'real', initial: 1 },
        asynchronous: 'bool',
    },
    signals: ['loaded'],
})
Loader.Null = 0
Loader.Ready = 1
Loader.Loading = 2
Loader.Error = 3

module.exports = { Loader }
