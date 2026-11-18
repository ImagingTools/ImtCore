const engine = require('../Core/engine')
const { GroupObject } = require('../Core/QtObject')
const { defineType } = require('../Core/type')
const { Item } = require('./Item')

// DropArea.drag: the position and the source of the drag it contains
class DropAreaDrag extends GroupObject {}
defineType(DropAreaDrag, {
    props: { x: { kind: 'real', readonly: true }, y: { kind: 'real', readonly: true }, source: { kind: 'var', readonly: true } },
    signals: ['positionChanged'],
})

// QQuickDropAreaPrivate::hasMatchingKey: no keys take any drag; else a key of the drag matches one of them as a
// whole, '*' standing for any text
function matchesKeys(keys, dragKeys){
    if(!keys || !keys.length) return true
    let escape = (key) => String(key).replace(/[.*+?^${}()|[\]\\]/g, '\\$&').replace(/\\\*/g, '.+')
    let pattern = new RegExp('^(' + keys.map(escape).join('|') + ')$')
    for(let key of dragKeys || []) if(pattern.test(String(key))) return true
    return false
}

// QQuickDropArea: takes the drag events of the scene (ItemAcceptsDrops), Drag.js delivers them
class DropArea extends Item {
    get $acceptsDrops(){
        return true
    }

    $setDrag(x, y, source){
        let drag = this.drag
        drag.$write(DropAreaDrag.X, x)
        drag.$write(DropAreaDrag.Y, y)
        if(source !== undefined) drag.$write(DropAreaDrag.SOURCE, source)
    }

    // QQuickDropArea::dragEnterEvent: not when disabled, already containing a drag, without a matching key, or when
    // the source is an ancestor of the area (dragging it drags the area too)
    $dragEnter(event){
        if(!this.$v[Item.ENABLED] || this.$v[DropArea.CONTAINSDRAG] || !matchesKeys(this.$v[DropArea.KEYS], event.keys)) return
        for(let it = this.$parent; it; it = it.$parent) if(it === event.source) return
        event.accepted = true
        engine.emitArgs(this, DropArea.SIGNAL_entered, [event])
        if(!event.accepted) return
        this.$setDrag(event.x, event.y, event.source)
        engine.emit(this.drag, DropAreaDrag.SIGNAL_positionChanged)
        this.$write(DropArea.CONTAINSDRAG, true)
    }

    $dragMove(event){
        if(!this.$v[DropArea.CONTAINSDRAG]) return
        this.$setDrag(event.x, event.y)
        engine.emit(this.drag, DropAreaDrag.SIGNAL_positionChanged)
        event.accepted = true
        engine.emitArgs(this, DropArea.SIGNAL_positionChanged, [event])
    }

    $dragLeave(){
        if(!this.$v[DropArea.CONTAINSDRAG]) return
        engine.emitArgs(this, DropArea.SIGNAL_exited, [])
        this.$write(DropArea.CONTAINSDRAG, false)
        this.drag.$write(DropAreaDrag.SOURCE, null)
    }

    $dropEvent(event){
        if(!this.$v[DropArea.CONTAINSDRAG]) return
        engine.emitArgs(this, DropArea.SIGNAL_dropped, [event])
        this.$write(DropArea.CONTAINSDRAG, false)
        this.drag.$write(DropAreaDrag.SOURCE, null)
    }
}
defineType(DropArea, {
    props: { keys: 'var', containsDrag: { kind: 'bool', readonly: true }, drag: { group: DropAreaDrag } },
    signals: ['entered(drag)', 'exited', 'positionChanged(drag)', 'dropped(drop)'],
})

module.exports = { DropArea }
