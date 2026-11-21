const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { Item } = require('../Item')

// ---- Window: the page is the only window; a Window is an item filling it -----------------------

class Window extends Item {
    constructor(parent = null){
        super(parent)
        // the items of a Window belong to it from the start, also while it is being created
        require('../KeyDelivery').windowCreated()
        engine.scheduleComponentComplete(this)
    }

    $componentComplete(){
        super.$componentComplete()
        this.$showTopLevel()
        if(this.$ownVisible) this.requestActivate()
    }
    // a top-level window (made with no parent: Qt.createComponent(...).createObject(null), a C++ QQmlComponent::create of
    // main.cpp) is a window of the screen in Qt: here it goes on the page, as the root object does
    $showTopLevel(){
        let page = Window.$pageContent
        if(!page || this.$parent || !this.$ownVisible || this.$f & engine.DESTROYED) return
        this.$setParent(page)
    }

    show(){
        this.visible = true
        this.$showTopLevel()
        this.requestActivate()
    }
    hide(){ this.visible = false }
    $write(id, value){
        super.$write(id, value)
        if(id === Item.VISIBLE && this.$f & engine.COMPLETED) this.$showTopLevel()
    }
    showNormal(){ this.show() }
    showMaximized(){ this.show() }
    showFullScreen(){ this.show() }
    close(){
        let close = { accepted: true }
        engine.emitArgs(this, Window.SIGNAL_closing, [close])
        if(close.accepted) this.visible = false
    }
    raise(){}
    lower(){}
    requestActivate(){ require('../KeyDelivery').activateWindow() }
}
defineType(Window, {
    props: {
        title: 'string',
        color: { kind: 'color', initial: '#ffffff' },
        flags: 'int',
        modality: 'int',
        visibility: 'int',
        minimumWidth: 'real',
        minimumHeight: 'real',
        maximumWidth: { kind: 'real', initial: 16777215 },
        maximumHeight: { kind: 'real', initial: 16777215 },
        active: { kind: 'bool', initial: true, readonly: true },
        contentItem: { kind: 'var', readonly: true, get(){ return this } },
    },
    signals: ['closing(close)'],
})
Object.assign(Window, { Windowed: 2, Minimized: 3, Maximized: 4, FullScreen: 5, AutomaticVisibility: 1, Hidden: 0 })

module.exports = { Window }
