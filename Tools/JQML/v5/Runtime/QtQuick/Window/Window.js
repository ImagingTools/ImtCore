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
        if(this.$ownVisible) this.requestActivate()
    }

    show(){
        this.visible = true
        this.requestActivate()
    }
    hide(){ this.visible = false }
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
