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
        // x and y of a window on the page are its place on the screen (QWindow::setPosition): kept, emitted, not
        // moving the items, whose scene starts at 0, 0
        if((id === Item.X || id === Item.Y) && this.$isTopLevel()){
            let screen = this.$screen || (this.$screen = [0, 0])
            let i = id === Item.X ? 0 : 1
            value = Number(value) || 0
            if(screen[i] === value) return
            let old = screen[i]
            screen[i] = value
            if(this.$v[id] !== 0) super.$write(id, 0)
            engine.emit(this, Item.$type.props[id].signal, old, value)
            return
        }
        super.$write(id, value)
        if(id === Item.VISIBLE && this.$f & engine.COMPLETED) this.$showTopLevel()
    }

    // a window of the page: not inside an item of the document (the root object, or one made with no parent)
    $isTopLevel(){
        let parent = this.$parent
        return !parent || parent === Window.$pageContent || !(parent instanceof Item) || !parent.$parent
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
// the position of a window of the page is the one on the screen
for(let [name, i] of [['x', 0], ['y', 1]]){
    let base = Object.getOwnPropertyDescriptor(Item.prototype, name)
    Object.defineProperty(Window.prototype, name, {
        get(){
            if(!this.$screen) return base.get.call(this)
            engine.track(this, Item.$type.props[i === 0 ? Item.X : Item.Y].signal)
            return this.$screen[i]
        },
        set(value){ base.set.call(this, value) },
        configurable: true,
    })
}
Object.assign(Window, { Windowed: 2, Minimized: 3, Maximized: 4, FullScreen: 5, AutomaticVisibility: 1, Hidden: 0 })

module.exports = { Window }
