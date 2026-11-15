const engine = require('../../Core/engine')
const { defineType } = require('../../Core/type')
const { Item } = require('../Item')
const { LayoutAttached } = require('./Layout')

// ---- LayoutItemProxy (qquicklayoutitemproxy.cpp) --------------------------------------------

// Layout properties the proxy takes from its target unless set on the proxy itself
const FORWARDED = ['minimumWidth', 'minimumHeight', 'preferredWidth', 'preferredHeight', 'maximumWidth', 'maximumHeight',
    'fillWidth', 'fillHeight', 'alignment', 'horizontalStretchFactor', 'verticalStretchFactor',
    'margins', 'leftMargin', 'topMargin', 'rightMargin', 'bottomMargin']

// target.$proxyData: the proxies of a target and the one in control
function proxyData(target){
    return target.$proxyData || (target.$proxyData = { proxies: [], controlling: null })
}

class LayoutItemProxy extends Item {
    constructor(parent = null){
        super(null)
        this.$target = null
        this.$links = []
        if(parent) this.$setParent(parent)
        let geometry = engine.PRIORITY_GEOMETRY
        for(let id of [Item.X, Item.Y, Item.WIDTH, Item.HEIGHT]){
            engine.connect(this, Item.$type.props[id].signal, () => this.$updatePos(), null, geometry)
        }
    }

    $write(id, value){
        let wasVisible = this.$v[Item.VISIBLE]
        super.$write(id, value)
        if(id === Item.VISIBLE && this.$v[Item.VISIBLE] !== wasVisible) this.$maybeTakeControl()
    }

    $setTarget(target){
        if(target === this.$target) return
        for(let link of this.$links) engine.unsubscribe(link)
        this.$links = []
        if(this.$target){
            let data = proxyData(this.$target)
            data.proxies = data.proxies.filter(p => p !== this)
            if(data.controlling === this) data.controlling = null
        }
        this.$target = target instanceof Item ? target : null
        let t = this.$target
        if(t){
            let data = proxyData(t)
            if(data.proxies.indexOf(this) < 0) data.proxies.push(this)
            if(!data.controlling) t.visible = false
            let attTarget = t.$v[Item.LAYOUT]
            if(attTarget){
                let attProxy = this.Layout
                let set = attProxy.$userSet
                for(let name of FORWARDED){
                    if(set && set.has(name)) continue
                    let prop = LayoutAttached.$type.propByName.get(name)
                    let forward = () => {
                        if(!(attProxy.$userSet && attProxy.$userSet.has(name))) attProxy.$write(prop.id, attTarget.$v[prop.id])
                    }
                    forward()
                    this.$links.push(engine.connect(attTarget, prop.signal, forward))
                }
            }
            let implicitW = () => this.$write(Item.IMPLICITWIDTH, t.$v[Item.IMPLICITWIDTH])
            let implicitH = () => this.$write(Item.IMPLICITHEIGHT, t.$v[Item.IMPLICITHEIGHT])
            implicitW()
            implicitH()
            this.$links.push(engine.connect(t, Item.$type.props[Item.IMPLICITWIDTH].signal, implicitW))
            this.$links.push(engine.connect(t, Item.$type.props[Item.IMPLICITHEIGHT].signal, implicitH))
        }
        if(this.$v[Item.VISIBLE]) this.$maybeTakeControl()
    }

    $maybeTakeControl(){
        let t = this.$target
        if(!t) return
        let data = proxyData(t)
        if(this.$v[Item.VISIBLE] && data.controlling !== this){
            if(!data.controlling && data.proxies.indexOf(this) >= 0){
                data.controlling = this
                t.visible = true
                t.$setParent(this)
                this.$updatePos()
            }
        }
        if(!this.$v[Item.VISIBLE] && data.controlling === this){
            if(t.$parent === this) t.$setParent(null)
            t.visible = false
            data.controlling = null
        }
    }

    // the target takes the proxy's geometry while the proxy is in control
    $updatePos(){
        let t = this.$target
        if(!t || !this.$v[Item.VISIBLE] || proxyData(t).controlling !== this) return
        t.$explicit |= 3
        t.$write(Item.WIDTH, this.$v[Item.WIDTH])
        t.$write(Item.HEIGHT, this.$v[Item.HEIGHT])
        t.$write(Item.X, 0)
        t.$write(Item.Y, 0)
    }

    $destroyNow(){
        let t = this.$target
        if(t && !(t.$f & engine.DESTROYED)){
            let data = proxyData(t)
            if(data.controlling === this){
                data.controlling = null
                t.$setParent(null)
            }
            data.proxies = data.proxies.filter(p => p !== this)
        }
        super.$destroyNow()
    }
}

defineType(LayoutItemProxy, {
    props: {
        target: { kind: 'var', effect(proxy, value){ proxy.$setTarget(value) } },
    },
})

module.exports = { LayoutItemProxy }
