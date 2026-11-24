// ScrollView of QtQuick.Templates (qquickscrollview.cpp, Qt 6.12). Checked against Qt 6.12 (tests/diff/cases/scrollview.qml):
//  - its content item is a Flickable: a Flickable declared in it (a ListView...) is the content item itself, other items
//    go into a Flickable it makes, whose content size is the scroll view's contentWidth / contentHeight (the implicit
//    size of a single item, else set); a TextArea in it is as wide as the view, as tall as its text;
//  - its ScrollBar.vertical / .horizontal follow the Flickable (the Basic style places them at its right and bottom);
//  - effectiveScrollBarWidth / Height: the width of the vertical bar, the height of the horizontal one, when shown.
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { Item } = require('../Item')
const { Control, withSetters } = require('./Control')
const { Pane } = require('./Containers')

const isFlickable = (item) => item && typeof item.$updateVisibleArea === 'function'

class ScrollView extends Pane {
    constructor(parent = null){
        super(parent)
        // (a control made outside a component is complete in its constructor: its Flickable is there already)
        if(this.$flickable === undefined) this.$flickable = null
        this.$write(Control.WHEELENABLED, true)
        this.$updateContentSize()
    }
    // the Flickable: a declared one, else one it makes
    $ensureFlickable(){
        if(this.$flickable) return this.$flickable
        let { Flickable } = require('../Flickable')
        let flickable = new Flickable(null)
        flickable.clip = true
        this.$setFlickable(flickable, true)
        return flickable
    }
    $setFlickable(flickable, own){
        this.$flickable = flickable
        this.$ownFlickable = own
        flickable.$delegateOf = this
        this.$setDelegate(Control.CONTENTITEM, flickable)
        // the bars of the scroll view follow it
        this.$moveBarsTo(flickable)
        if(own){
            let w = () => (this.$contentSizeSet.w ? this.$v[ScrollView.CONTENTWIDTH] : this.$v[ScrollView.CONTENTWIDTH])
            flickable.$bind(flickable.constructor.CONTENTWIDTH, () => this.contentWidth, null, flickable)
            flickable.$bind(flickable.constructor.CONTENTHEIGHT, () => this.contentHeight, null, flickable)
        }
    }
    $childAdded(child){
        if(child.$delegateOf === this || child === this.$v[Control.CONTENTITEM] || child === this.$v[Control.BACKGROUND]) return
        if(this.$isBar(child)) return
        if(isFlickable(child) && !this.$flickable){
            // QQuickScrollViewPrivate::setFlickable: a Flickable in it is the content item
            this.$setFlickable(child, false)
            this.$updateContentSize()
            return
        }
        let flickable = this.$ensureFlickable()
        child.$setParent(flickable.contentItem)
        this.$contentChild = child
        this.$watchContentChild(child)
        // a TextArea fills the width of the view, its height the text's (QQuickTextAreaPrivate::attachFlickable)
        if(child.constructor.$type.propByName.has('placeholderText') && child.constructor.$type.propByName.has('wrapMode')){
            child.$bind(Item.WIDTH, () => flickable.width, null, child)
            child.$bind(Item.HEIGHT, () => Math.max(child.implicitHeight, 0), null, child)
        }
        this.$updateContentSize()
    }
    // the scroll bars of the view are its own children
    $isBar(child){ let { ScrollBar } = require('./ScrollBar'); return child instanceof ScrollBar }
    $contentChildren(){
        let flickable = this.$flickable
        if(!flickable) return []
        if(!this.$ownFlickable) return [flickable]
        let content = flickable.contentItem
        return content && content.$children ? content.$children.filter(c => c.$isItem) : []
    }
    // QQuickScrollViewPrivate::updateContentWidth: a single item's implicit size, a declared Flickable's content size (-1)
    $updateContentSize(){
        if(!this.$contentSizeSet) return
        let w = 0, h = 0
        let flickable = this.$flickable
        if(flickable && !this.$ownFlickable){
            w = flickable.contentWidth
            h = flickable.contentHeight
            if(!this.$flickableWatched){
                this.$flickableWatched = true
                engine.connect(flickable, flickable.constructor.SIGNAL_contentWidthChanged, () => this.$updateContentSize(), null, engine.PRIORITY_NORMAL, this)
                engine.connect(flickable, flickable.constructor.SIGNAL_contentHeightChanged, () => this.$updateContentSize(), null, engine.PRIORITY_NORMAL, this)
            }
        } else {
            // no content: -1 (QQuickScrollViewPrivate::updateContentWidth)
            let children = this.$contentChildren()
            let single = children.length === 1 ? children[0] : null
            w = single ? single.$v[Item.IMPLICITWIDTH] : children.length ? 0 : -1
            h = single ? single.$v[Item.IMPLICITHEIGHT] : children.length ? 0 : -1
            if(single && single.$v[Item.HEIGHT] > h && single.$explicit & 2) h = single.$v[Item.HEIGHT]
        }
        if(!this.$contentSizeSet.w) this.$plain(ScrollView.CONTENTWIDTH, w)
        if(!this.$contentSizeSet.h) this.$plain(ScrollView.CONTENTHEIGHT, h)
        this.$plain(Control.IMPLICITCONTENTWIDTH, w)
        this.$plain(Control.IMPLICITCONTENTHEIGHT, h)
    }
    // the bars attached to the scroll view take its Flickable as their view
    $moveBarsTo(flickable){
        let group = this.$v[Item.$type.propByName.get('ScrollBar').id]
        if(group && group.$retarget) group.$retarget(flickable)
    }
    $updateBarSizes(){
        let group = this.$v[Item.$type.propByName.get('ScrollBar').id]
        let v = group && group.vertical, h = group && group.horizontal
        // a bar that is shown: always on, or as needed with content larger than the view
        let shown = (bar) => bar && bar.visible && (bar.policy === 2 || (bar.policy === 0 && bar.size > 0 && bar.size < 1.0))
        this.$plain(ScrollView.EFFECTIVESCROLLBARWIDTH, shown(v) ? v.width : 0)
        this.$plain(ScrollView.EFFECTIVESCROLLBARHEIGHT, shown(h) ? h.height : 0)
    }
    // QQuickScrollView::componentComplete: a Flickable also without content
    $componentComplete(){
        this.$ensureFlickable()
        super.$componentComplete()
    }
    // the wheel goes to the Flickable
    $wheel(){ return false }
    $mousePress(){ return false }
}
defineType(ScrollView, { props: { effectiveScrollBarWidth: { kind: 'real', readonly: true }, effectiveScrollBarHeight: { kind: 'real', readonly: true } } })
ScrollView.prototype.$controlName = 'ScrollView'

module.exports = { ScrollView }
