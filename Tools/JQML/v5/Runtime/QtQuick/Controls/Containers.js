// The containers of QtQuick.Templates (Qt 6.12): Pane, Frame, GroupBox, Page, ToolBar, ToolSeparator, MenuSeparator,
// Container, TabBar, SwipeView. Checked against Qt 6.12 (tests/diff/cases/containers.qml):
//  - Pane: the items declared in it go into its content item (QQuickContentItem), placed at the paddings; contentWidth /
//    contentHeight are the implicit size of a single item, or set; a press on it is taken (a MouseArea below gets none);
//  - Page: the header at the top and the footer at the bottom, as wide as the page, the content between them with
//    spacing; a ToolBar or TabBar in them takes the position Header / Footer;
//  - Container: the items declared in it are its contentModel (an ObjectModel its content item, a ListView, shows);
//    currentIndex follows the content item's (a swipe) and the checked button of a TabBar;
//  - TabBar: the buttons without a width of their own share what the others leave (QQuickTabBarPrivate::updateLayout).
const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { Item } = require('../Item')
const { Control, withSetters, buttonFocusPolicy } = Object.assign({}, require('./Control'), require('./AbstractButton'))
const { ObjectModel } = require('../../QtQml/Models/ObjectModel')

const Horizontal = 1, Vertical = 2
const Header = 0, Footer = 1

// QQuickContentItem: the content item a Pane makes (its objectName the name of the control's type)
class ContentItem extends Item {}
defineType(ContentItem, {})

// ---- Pane (QQuickPane) ----
class Pane extends Control {
    constructor(parent = null){
        super(parent)
        this.$contentSizeSet = { w: false, h: false }
        let content = new ContentItem(null)
        content.objectName = this.$controlName || 'Pane'
        this.$ownContent = content
        this.$setDelegate(Control.CONTENTITEM, content)
    }
    // an item declared in the pane: a child of its content item
    $childAdded(child){
        if(child.$delegateOf === this || child === this.$v[Control.CONTENTITEM] || child === this.$v[Control.BACKGROUND]) return
        if(this.$isOwnPart && this.$isOwnPart(child)) return
        let content = this.$v[Control.CONTENTITEM]
        if(!content || content === child) return
        child.$setParent(content)
        this.$watchContentChild(child)
        this.$updateContentSize()
        engine.emit(this, Pane.SIGNAL_contentChildrenChanged)
    }
    $setDelegate(id, item, z){
        if(item) item.$delegateOf = this
        super.$setDelegate(id, item, z)
        if(id === Control.CONTENTITEM){
            let content = this.$v[Control.CONTENTITEM]
            if(content) engine.connect(content, Item.SIGNAL_childrenChanged, () => this.$updateContentSize(), null, engine.PRIORITY_NORMAL, this)
            this.$updateContentSize()
        }
    }
    $watchContentChild(child){
        let update = () => this.$updateContentSize()
        engine.connect(child, Item.SIGNAL_implicitWidthChanged, update, null, engine.PRIORITY_NORMAL, this)
        engine.connect(child, Item.SIGNAL_implicitHeightChanged, update, null, engine.PRIORITY_NORMAL, this)
    }
    $contentChildren(){
        let content = this.$v[Control.CONTENTITEM]
        return content && content.$children ? content.$children.filter(c => c.$isItem) : []
    }
    // QQuickPanePrivate::updateContentWidth / Height: a single item's implicit size, unless the size is set
    $updateContentSize(){
        let content = this.$v[Control.CONTENTITEM]
        let w = 0, h = 0
        if(content && content !== this.$ownContent){
            // a content item of its own: its implicit size (QQuickPanePrivate::getContentWidth)
            w = content.$v[Item.IMPLICITWIDTH]
            h = content.$v[Item.IMPLICITHEIGHT]
        } else {
            let children = this.$contentChildren()
            let single = children.length === 1 ? children[0] : null
            w = single ? single.$v[Item.IMPLICITWIDTH] : 0
            h = single ? single.$v[Item.IMPLICITHEIGHT] : 0
        }
        if(!this.$contentSizeSet.w) this.$plain(Pane.CONTENTWIDTH, w)
        if(!this.$contentSizeSet.h) this.$plain(Pane.CONTENTHEIGHT, h)
        this.$plain(Control.IMPLICITCONTENTWIDTH, w)
        this.$plain(Control.IMPLICITCONTENTHEIGHT, h)
    }
    // the implicit content size is the contained item's, not the content item's own
    $updateImplicitContent(){ if(this.$contentSizeSet) this.$updateContentSize() }
    get contentChildren(){ return this.$contentChildren() }
    get contentData(){ return this.$contentChildren() }
    // QQuickPane::mousePressEvent: a pane takes the presses on it
    $mousePress(){ this.$clickFocus(); return true }
    $mouseMove(){}
    $mouseRelease(){}
    $wheel(){ return !!this.$v[Control.WHEELENABLED] }
}
defineType(Pane, { props: { contentWidth: 'real', contentHeight: 'real' }, signals: ['contentChildrenChanged'] })
Pane.$setters = withSetters(Control, [
    [Pane.CONTENTWIDTH, (p, v) => { p.$contentSizeSet.w = v !== undefined; if(v === undefined) p.$updateContentSize(); else p.$plain(Pane.CONTENTWIDTH, Number(v)) }],
    [Pane.CONTENTHEIGHT, (p, v) => { p.$contentSizeSet.h = v !== undefined; if(v === undefined) p.$updateContentSize(); else p.$plain(Pane.CONTENTHEIGHT, Number(v)) }],
])

class Frame extends Pane {}
defineType(Frame, {})

// ---- GroupBox: a title shown by its label ----
class GroupBox extends Frame {
    $setLabel(item){
        let old = this.$v[GroupBox.LABEL]
        if(old === item) return
        if(old && !(old.$f & engine.DESTROYED)){
            if(this.$defaults && this.$defaults.has(old)){
                this.$defaults.delete(old)
                old.destroy()
            } else if(old.$parent === this) old.$setParent(null)
        }
        if(item) item.$delegateOf = this
        this.$plain(GroupBox.LABEL, item || null)
        if(item){
            if(item.$parent !== this) item.$setParent(this)
            this.$watchImplicit(item, GroupBox.LABEL)
        }
        this.$delegateImplicitChanged(GroupBox.LABEL)
    }
    $delegateImplicitChanged(id){
        if(id === GroupBox.LABEL){
            let label = this.$v[GroupBox.LABEL]
            this.$plain(GroupBox.IMPLICITLABELWIDTH, label ? label.$v[Item.IMPLICITWIDTH] : 0)
            this.$plain(GroupBox.IMPLICITLABELHEIGHT, label ? label.$v[Item.IMPLICITHEIGHT] : 0)
            return
        }
        super.$delegateImplicitChanged(id)
    }
}
defineType(GroupBox, {
    props: { title: 'string', label: { kind: 'var', initial: null },
        implicitLabelWidth: { kind: 'real', readonly: true }, implicitLabelHeight: { kind: 'real', readonly: true } },
})
GroupBox.$setters = withSetters(Pane, [[GroupBox.LABEL, (g, v) => g.$setLabel(v)]])

// ---- ToolBar: a pane that is a header or a footer ----
class ToolBar extends Pane {}
defineType(ToolBar, { props: { position: 'int' } })
Object.assign(ToolBar, { Header, Footer })

// ---- Page (QQuickPage): a title, a header and a footer around the content ----
class Page extends Pane {
    $setBar(id, item){
        let old = this.$v[id]
        if(old === item) return
        if(old && !(old.$f & engine.DESTROYED)){
            this.$unwatchImplicit(old)
            if(old.$parent === this) old.$setParent(null)
        }
        if(item) item.$delegateOf = this
        this.$plain(id, item || null)
        if(item){
            if(item.$parent !== this) item.$setParent(this)
            // QQuickPagePrivate: a ToolBar, TabBar or DialogButtonBox in the header or footer takes that position
            if(item.constructor.Header !== undefined && item.constructor.$type.propByName.has('position')) item.position = id === Page.HEADER ? Header : Footer
            this.$watchImplicit(item, id)
            let layout = () => this.$relayout()
            engine.connect(item, Item.SIGNAL_heightChanged, layout, null, engine.PRIORITY_NORMAL, this)
            engine.connect(item, Item.SIGNAL_visibleChanged, () => this.$delegateImplicitChanged(id), null, engine.PRIORITY_NORMAL, this)
        }
        this.$delegateImplicitChanged(id)
        this.$relayout()
    }
    $delegateImplicitChanged(id){
        if(id === Page.HEADER || id === Page.FOOTER){
            // a hidden header or footer has no implicit size (QQuickPagePrivate::getImplicitHeaderHeight)
            let item = this.$v[id]
            let shown = item && item.$v[Item.VISIBLE]
            let header = id === Page.HEADER
            this.$plain(header ? Page.IMPLICITHEADERWIDTH : Page.IMPLICITFOOTERWIDTH, shown ? item.$v[Item.IMPLICITWIDTH] : 0)
            this.$plain(header ? Page.IMPLICITHEADERHEIGHT : Page.IMPLICITFOOTERHEIGHT, shown ? item.$v[Item.IMPLICITHEIGHT] : 0)
            this.$relayout()
            return
        }
        super.$delegateImplicitChanged(id)
    }
    $resizeContent(){ this.$relayout() }
    // QQuickPagePrivate::relayout
    $relayout(){
        let v = this.$v
        let header = v[Page.HEADER], footer = v[Page.FOOTER], content = v[Control.CONTENTITEM]
        let hh = header && header.$v[Item.VISIBLE] ? header.$v[Item.HEIGHT] : 0
        let fh = footer && footer.$v[Item.VISIBLE] ? footer.$v[Item.HEIGHT] : 0
        let hsp = hh > 0 ? v[Control.SPACING] : 0, fsp = fh > 0 ? v[Control.SPACING] : 0
        if(content && !(content.$f & engine.DESTROYED)){
            content.$write(Item.Y, v[Control.TOPPADDING] + hh + hsp)
            content.$write(Item.X, v[Control.LEFTPADDING])
            content.$explicit |= 3
            content.$write(Item.WIDTH, v[Control.AVAILABLEWIDTH])
            content.$write(Item.HEIGHT, v[Control.AVAILABLEHEIGHT] - hh - fh - hsp - fsp)
        }
        if(header){
            header.$explicit |= 1
            header.$write(Item.WIDTH, v[Item.WIDTH])
        }
        if(footer){
            footer.$write(Item.Y, v[Item.HEIGHT] - footer.$v[Item.HEIGHT])
            footer.$explicit |= 1
            footer.$write(Item.WIDTH, v[Item.WIDTH])
        }
    }
    $write(id, value){
        super.$write(id, value)
        if(id === Control.SPACING) this.$relayout()
    }
}
defineType(Page, {
    props: {
        title: 'string', header: { kind: 'var', initial: null }, footer: { kind: 'var', initial: null },
        implicitHeaderWidth: { kind: 'real', readonly: true }, implicitHeaderHeight: { kind: 'real', readonly: true },
        implicitFooterWidth: { kind: 'real', readonly: true }, implicitFooterHeight: { kind: 'real', readonly: true },
    },
})
Page.$setters = withSetters(Pane, [
    [Page.HEADER, (p, v) => p.$setBar(Page.HEADER, v)],
    [Page.FOOTER, (p, v) => p.$setBar(Page.FOOTER, v)],
])

// ---- ToolSeparator, MenuSeparator ----
class ToolSeparator extends Control {
    constructor(parent = null){
        super(parent)
        this.$plain(ToolSeparator.ORIENTATION, Vertical)
        this.$plain(ToolSeparator.VERTICAL, true)
    }
}
defineType(ToolSeparator, { props: { orientation: 'int', horizontal: { kind: 'bool', readonly: true }, vertical: { kind: 'bool', readonly: true } } })
ToolSeparator.$setters = withSetters(Control, [[ToolSeparator.ORIENTATION, (t, v) => {
    t.$plain(ToolSeparator.ORIENTATION, v)
    t.$plain(ToolSeparator.HORIZONTAL, v === Horizontal)
    t.$plain(ToolSeparator.VERTICAL, v === Vertical)
}]])
class MenuSeparator extends Control {}
defineType(MenuSeparator, {})

// ---- Container (QQuickContainer): its items are an ObjectModel ----
class Container extends Control {
    constructor(parent = null){
        super(parent)
        this.$plain(Container.CONTENTMODEL, new ObjectModel(null))
        this.$plain(Container.CURRENTINDEX, -1)
        this.$contentSizeSet = { w: false, h: false }
    }
    $childAdded(child){
        if(child.$delegateOf === this || child === this.$v[Control.CONTENTITEM] || child === this.$v[Control.BACKGROUND]) return
        // a Repeater is transparent for the positioner (QQuickContainerPrivate::contentData_append): not an item of the
        // container; the delegates it makes in the container are
        if(child instanceof require('../Repeater').Repeater) return
        this.addItem(child)
    }
    $setDelegate(id, item, z){
        if(item) item.$delegateOf = this
        super.$setDelegate(id, item, z)
        if(id === Control.CONTENTITEM) this.$watchContentIndex()
    }
    // QQuickContainerPrivate::_q_currentIndexChanged: a content item with a currentIndex (a ListView) moves the container's
    $watchContentIndex(){
        let content = this.$v[Control.CONTENTITEM]
        let signal = content && content.constructor.SIGNAL_currentIndexChanged
        if(signal === undefined) return
        engine.connect(content, signal, () => {
            if(!this.$updatingCurrent) this.setCurrentIndex(content.currentIndex)
        }, null, engine.PRIORITY_NORMAL, this)
    }
    get contentChildren(){ return this.$v[Container.CONTENTMODEL].$items.slice() }
    get contentData(){ return this.contentChildren }
    get count(){
        engine.track(this.$v[Container.CONTENTMODEL], ObjectModel.SIGNAL_countChanged)
        return this.$v[Container.CONTENTMODEL].$items.length
    }
    itemAt(index){ return this.$v[Container.CONTENTMODEL].$items[index] || null }
    // QQuickContainerPrivate::insertItem: the item in the model
    $insert(index, item){
        let model = this.$v[Container.CONTENTMODEL]
        if(!item || model.$items.includes(item)) return
        item.$delegateOf = undefined
        item.$container = this
        model.insert(index, item)
        this.$itemAdded(item)
        // QQuickContainerPrivate::insertItem: the first item becomes current; the current index does not follow a move
        if(model.$items.length === 1 && this.$v[Container.CURRENTINDEX] === -1) this.setCurrentIndex(index)
        this.$countChanged()
    }
    $itemAdded(item){}
    $itemRemoved(item){}
    $countChanged(){
        // currentItem: the item at the current index (QQuickContainer::currentItem)
        this.$plain(Container.CURRENTITEM, this.itemAt(this.$v[Container.CURRENTINDEX]))
        engine.emit(this, Container.SIGNAL_countChanged)
        engine.emit(this, Container.SIGNAL_contentChildrenChanged)
    }
    addItem(item){ engine.batch(() => this.$insert(this.$v[Container.CONTENTMODEL].$items.length, item)) }
    insertItem(index, item){ engine.batch(() => this.$insert(Math.max(0, Math.min(index, this.$v[Container.CONTENTMODEL].$items.length)), item)) }
    moveItem(from, to){
        let model = this.$v[Container.CONTENTMODEL]
        let count = model.$items.length
        if(from < 0 || from >= count || to < 0 || to >= count || from === to) return
        engine.batch(() => {
            let current = this.$v[Container.CURRENTINDEX]
            model.move(from, to, 1)
            if(current === from) this.setCurrentIndex(to)
            else if(from < current && to >= current) this.setCurrentIndex(current - 1)
            else if(from > current && to <= current) this.setCurrentIndex(current + 1)
        })
    }
    takeItem(index){
        let model = this.$v[Container.CONTENTMODEL]
        let item = model.$items[index]
        if(!item) return null
        engine.batch(() => {
            let current = this.$v[Container.CURRENTINDEX]
            model.remove(index, 1)
            if(item.$parent === this.$v[Control.CONTENTITEM] || (item.$parent && item.$parent.$parent === this.$v[Control.CONTENTITEM])) item.$setParent(null)
            this.$itemRemoved(item)
            let count = model.$items.length
            if(current === index) this.setCurrentIndex(count ? Math.min(index, count - 1) : -1)
            else if(index < current) this.setCurrentIndex(current - 1)
            this.$countChanged()
        })
        return item
    }
    removeItem(item){
        let index = typeof item === 'number' ? item : this.$v[Container.CONTENTMODEL].$items.indexOf(item)
        let taken = this.takeItem(index)
        if(taken) taken.destroy()
    }
    setCurrentIndex(index){
        if(this.$v[Container.CURRENTINDEX] === index) return
        engine.batch(() => {
            this.$plain(Container.CURRENTINDEX, index)
            this.$plain(Container.CURRENTITEM, this.itemAt(index))
            this.$currentChanged()
            // the content item (a ListView) follows
            let content = this.$v[Control.CONTENTITEM]
            if(content && content.constructor.CURRENTINDEX !== undefined && content.currentIndex !== index){
                this.$updatingCurrent = true
                try { content.currentIndex = index } finally { this.$updatingCurrent = false }
            }
        })
    }
    $currentChanged(){}
    incrementCurrentIndex(){ let i = this.$v[Container.CURRENTINDEX]; if(i < this.count - 1) this.setCurrentIndex(i + 1) }
    decrementCurrentIndex(){ let i = this.$v[Container.CURRENTINDEX]; if(i > 0) this.setCurrentIndex(i - 1) }
}
defineType(Container, {
    props: {
        contentModel: { kind: 'var', readonly: true }, currentIndex: 'int', currentItem: { kind: 'var', readonly: true, initial: null },
        contentWidth: 'real', contentHeight: 'real',
    },
    signals: ['countChanged', 'contentChildrenChanged'],
})
Container.$setters = withSetters(Control, [[Container.CURRENTINDEX, (c, v) => c.setCurrentIndex(v)]])

// ---- TabBar (QQuickTabBar) ----
class TabBar extends Container {
    $itemAdded(item){
        // a button that is checked by a click makes its index current
        let signal = item.constructor.SIGNAL_checkedChanged
        if(signal !== undefined) engine.connect(item, signal, () => {
            if(item.checked) this.setCurrentIndex(this.$v[Container.CONTENTMODEL].$items.indexOf(item))
        }, null, engine.PRIORITY_NORMAL, this)
        let layout = () => this.$updateLayout()
        engine.connect(item, Item.SIGNAL_implicitWidthChanged, layout, null, engine.PRIORITY_NORMAL, this)
        engine.connect(item, Item.SIGNAL_implicitHeightChanged, layout, null, engine.PRIORITY_NORMAL, this)
        this.$updateLayout()
    }
    $itemRemoved(){ this.$updateLayout() }
    $currentChanged(){
        let current = this.itemAt(this.$v[Container.CURRENTINDEX])
        if(current && current.checked === false) current.checked = true
    }
    $setDelegate(id, item, z){
        super.$setDelegate(id, item, z)
        if(id === Control.CONTENTITEM && item){
            engine.connect(item, Item.SIGNAL_widthChanged, () => this.$updateLayout(), null, engine.PRIORITY_NORMAL, this)
            this.$updateLayout()
        }
    }
    $write(id, value){
        super.$write(id, value)
        if(id === Control.SPACING) this.$updateLayout()
    }
    // QQuickTabBarPrivate::updateLayout: the buttons without a width share the width the others leave; the content size
    $updateLayout(){
        let items = this.$v[Container.CONTENTMODEL].$items
        let content = this.$v[Control.CONTENTITEM]
        let spacing = this.$v[Control.SPACING]
        let reserved = 0, resizable = 0, maxHeight = 0, implicitWidth = 0
        for(let item of items){
            let own = item.$ownWidth !== undefined ? item.$ownWidth : !!(item.$explicit & 1)
            item.$ownWidth = own
            if(own) reserved += item.$v[Item.WIDTH]
            else resizable++
            maxHeight = Math.max(maxHeight, item.$v[Item.IMPLICITHEIGHT])
            implicitWidth += own ? item.$v[Item.WIDTH] : item.$v[Item.IMPLICITWIDTH]
        }
        let total = Math.max(0, items.length - 1) * spacing
        if(!this.$contentSizeSet.w) this.$plain(Container.CONTENTWIDTH, implicitWidth + total)
        if(!this.$contentSizeSet.h) this.$plain(Container.CONTENTHEIGHT, maxHeight)
        this.$plain(Control.IMPLICITCONTENTWIDTH, this.$v[Container.CONTENTWIDTH])
        this.$plain(Control.IMPLICITCONTENTHEIGHT, this.$v[Container.CONTENTHEIGHT])
        if(!content || !items.length) return
        let width = (content.$v[Item.WIDTH] - reserved - total) / Math.max(1, resizable)
        for(let item of items){
            if(!item.$ownWidth){
                item.$write(Item.WIDTH, width)
            }
            if(!(item.$explicit & 2)) item.$write(Item.HEIGHT, this.$v[Container.CONTENTHEIGHT])
        }
    }
    $updateImplicitContent(){ if(this.$v[Container.CONTENTMODEL]) this.$updateLayout() }
}
defineType(TabBar, { props: { position: 'int' } })
Object.assign(TabBar, { Header, Footer })

// ---- SwipeView (QQuickSwipeView): its pages the size of the view ----
class SwipeView extends Container {
    constructor(parent = null){
        super(parent)
        this.$plain(SwipeView.INTERACTIVE, true)
        this.$write(Control.FOCUSPOLICY, buttonFocusPolicy())
        this.$plain(SwipeView.ORIENTATION, Horizontal)
        this.$plain(SwipeView.HORIZONTAL, true)
    }
    $itemAdded(item){ this.$resizeItems() }
    $setDelegate(id, item, z){
        super.$setDelegate(id, item, z)
        if(id === Control.CONTENTITEM && item){
            let resize = () => this.$resizeItems()
            engine.connect(item, Item.SIGNAL_widthChanged, resize, null, engine.PRIORITY_NORMAL, this)
            engine.connect(item, Item.SIGNAL_heightChanged, resize, null, engine.PRIORITY_NORMAL, this)
            this.$resizeItems()
        }
    }
    // QQuickSwipeViewPrivate::resizeItems: each page as large as the content item
    $resizeItems(){
        let content = this.$v[Control.CONTENTITEM]
        if(!content) return
        for(let item of this.$v[Container.CONTENTMODEL].$items){
            item.$write(Item.WIDTH, content.$v[Item.WIDTH])
            item.$write(Item.HEIGHT, content.$v[Item.HEIGHT])
        }
    }
}
defineType(SwipeView, {
    props: { interactive: 'bool', orientation: 'int', horizontal: { kind: 'bool', readonly: true }, vertical: { kind: 'bool', readonly: true } },
})
SwipeView.$setters = withSetters(Container, [[SwipeView.ORIENTATION, (s, v) => {
    s.$plain(SwipeView.ORIENTATION, v)
    s.$plain(SwipeView.HORIZONTAL, v === Horizontal)
    s.$plain(SwipeView.VERTICAL, v === Vertical)
}]])

// the QML names of the types (the objectName of a Pane's content item)
for(let [Class, name] of [[Pane, 'Pane'], [Frame, 'Frame'], [GroupBox, 'GroupBox'], [ToolBar, 'ToolBar'], [Page, 'Page']]) Class.prototype.$controlName = name

// ---- the attached SwipeView.view / index / isCurrentItem / isNextItem / isPreviousItem of a page ----
class SwipeViewAttached extends require('../../Core/QtObject').GroupObject {
    $view(){
        engine.track(this.$owner, Item.SIGNAL_parentChanged)
        let owner = this.$owner
        if(owner && owner.$container instanceof SwipeView && owner.$container.$v[Container.CONTENTMODEL].$items.includes(owner)){
            engine.track(owner.$container, Container.SIGNAL_currentIndexChanged)
            engine.track(owner.$container, Container.SIGNAL_countChanged)
            return owner.$container
        }
        for(let it = this.$owner && this.$owner.$parent; it; it = it.$parent){
            if(it instanceof SwipeView){
                engine.track(it, Container.SIGNAL_currentIndexChanged)
                engine.track(it, Container.SIGNAL_countChanged)
                return it
            }
        }
        return null
    }
    get view(){ return this.$view() }
    get index(){ let v = this.$view(); return v ? v.$v[Container.CONTENTMODEL].$items.indexOf(this.$owner) : -1 }
    get isCurrentItem(){ let v = this.$view(); return !!v && this.index === v.$v[Container.CURRENTINDEX] && this.index >= 0 }
    get isNextItem(){ let v = this.$view(); return !!v && this.index >= 0 && this.index === v.$v[Container.CURRENTINDEX] + 1 }
    get isPreviousItem(){ let v = this.$view(); return !!v && this.index >= 0 && this.index === v.$v[Container.CURRENTINDEX] - 1 }
}
defineType(SwipeViewAttached, {})
{
    let prop = Item.$type.propByName.get('SwipeView')
    prop.group = SwipeViewAttached
    Object.defineProperty(Item.prototype, 'SwipeView', {
        get(){ return this.$v[prop.id] || (this.$v[prop.id] = new SwipeViewAttached(this)) },
        configurable: true,
    })
}

module.exports = { ContentItem, Pane, Frame, GroupBox, ToolBar, Page, ToolSeparator, MenuSeparator, Container, TabBar, SwipeView }
