const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { Item } = require('../Item')
const { Layout, hints } = require('./Layout')

// ---- StackLayout -------------------------------------------------------------------------------

// "currentIndex ... -1 for an empty layout, otherwise the default is 0"; only the current item is visible;
// children fill the layout (their fillWidth / fillHeight default to true)
class StackLayout extends Layout {
    $items(){
        return (this.$children || []).filter(child => child instanceof Item)
    }

    itemAt(index){ return this.$items()[index] || null }

    $arrange(){
        let items = this.$items()
        this.$write(StackLayout.COUNT, items.length)
        if(!this.$indexSet) this.$write(StackLayout.CURRENTINDEX, items.length ? 0 : -1)
        let current = this.$v[StackLayout.CURRENTINDEX]
        let width = 0, height = 0
        let geometry = []
        items.forEach((item, index) => {
            let hx = hints(item, true, this), hy = hints(item, false, this)
            width = Math.max(width, hx.preferred)
            height = Math.max(height, hy.preferred)
            item.visible = index === current
            geometry.push([item, hx, hy])
        })
        this.$write(Item.IMPLICITWIDTH, width)
        this.$write(Item.IMPLICITHEIGHT, height)
        let W = this.$v[Item.WIDTH], H = this.$v[Item.HEIGHT]
        let entry = geometry[current]
        if(entry){
            let [item, hx, hy] = entry
            let w = hx.fill ? Math.min(W, hx.max) : hx.preferred
            let h = hy.fill ? Math.min(H, hy.max) : hy.preferred
            this.$place(item, hx.before, hy.before, w - hx.before - hx.after, h - hy.before - hy.after)
        }
    }

    $updateVisibility(){
        let current = this.$v[StackLayout.CURRENTINDEX]
        this.$items().forEach((item, index) => { item.visible = index === current })
    }

    $assign(id, value){
        if(id === StackLayout.CURRENTINDEX) this.$indexSet = true
        super.$assign(id, value)
    }

    $bind(id, fn, scope, self){
        if(id === StackLayout.CURRENTINDEX) this.$indexSet = true
        return super.$bind(id, fn, scope, self)
    }
}

defineType(StackLayout, {
    props: {
        currentIndex: { kind: 'int', initial: -1, effect: (layout) => {
            if(layout.$f & engine.COMPLETED) layout.$updateVisibility()
            layout.$invalidate()
        } },
        count: 'int',
    },
})

module.exports = { StackLayout }
