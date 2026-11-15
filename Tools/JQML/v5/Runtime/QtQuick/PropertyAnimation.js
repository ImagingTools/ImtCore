const engine = require('../Core/engine')
const { coerceColor } = require('../Core/color')
const { defineType } = require('../Core/type')
const { Animation, readProperty, resolveProperty, writeProperty } = require('./Animation')
const { EasingGroup } = require('./EasingGroup')

function parseColor(value){
    let hex = coerceColor(value === undefined || value === null ? '#000000' : String(value))
    let a = 255, i = 1
    if(hex.length === 9){
        a = parseInt(hex.slice(1, 3), 16)
        i = 3
    }
    return [parseInt(hex.slice(i, i + 2), 16), parseInt(hex.slice(i + 2, i + 4), 16), parseInt(hex.slice(i + 4, i + 6), 16), a]
}

function hex2(v){
    return Math.max(0, Math.min(255, Math.round(v))).toString(16).padStart(2, '0')
}

function interpolateColor(from, to, t){
    let a = parseColor(from), b = parseColor(to)
    let c = a.map((v, i) => v + (b[i] - v) * t)
    return c[3] >= 255 ? '#' + hex2(c[0]) + hex2(c[1]) + hex2(c[2]) : '#' + hex2(c[3]) + hex2(c[0]) + hex2(c[1]) + hex2(c[2])
}

// interpolator by animation type (NumberAnimation: real, ColorAnimation: color) or by property type
function interpolate(kind, from, to, t){
    if(kind === 'color') return interpolateColor(from, to, t)
    if(kind === 'real' || kind === 'int') return Number(from) + (Number(to) - Number(from)) * t
    if(typeof from === 'number' && typeof to === 'number') return from + (to - from) * t
    if(typeof to === 'string' && /^#[0-9a-f]{6,8}$/i.test(to) && typeof from === 'string') return interpolateColor(from, to, t)
    return t < 1 ? from : to
}

function convert(kind, value){
    if(value === undefined) return value
    if(kind === 'real' || kind === 'int') return Number(value)
    if(kind === 'color') return coerceColor(String(value))
    return value
}

// ---- PropertyAnimation ----------------------------------------------------------------------

class PropertyAnimation extends Animation {
    constructor(parent = null){
        super(parent)
        this.$actions = []
        this.$fromIsSourced = false
        this.$fromIsDefined = false
        this.$toIsDefined = false
    }

    $interpolatorKind(){ return null }

    $duration(){ return this.$v[PropertyAnimation.DURATION] }

    $targets(){
        let targets = []
        let target = this.$v[PropertyAnimation.TARGET]
        if(target) targets.push(target)
        let list = this.$v[PropertyAnimation.TARGETS]
        if(list){
            for(let t of list) if(t && targets.indexOf(t) < 0) targets.push(t)
        }
        return targets
    }

    $properties(){
        let props = []
        let property = this.$v[PropertyAnimation.PROPERTY]
        if(property) props.push(property)
        let list = this.$v[PropertyAnimation.PROPERTIES]
        if(list){
            for(let p of String(list).split(',')){
                p = p.trim()
                if(p && props.indexOf(p) < 0) props.push(p)
            }
        }
        return props
    }

    // QQuickPropertyAnimation::createTransitionActions
    $transition(actions, modified, defaultTarget){
        let props = this.$properties()
        let targets = this.$targets()
        let hasSelectors = props.length > 0 || targets.length > 0
        let kind = this.$interpolatorKind()
        let defaults = this.$defaultProperty
        if(defaults && !hasSelectors){
            props.push(defaults.path)
            targets.push(defaults.target)
        }
        if(defaultTarget && !targets.length) targets.push(defaultTarget)
        let result = []
        if(this.$toIsDefined){
            for(let name of props){
                for(let target of targets){
                    let p = resolveProperty(target, name)
                    if(!p){
                        console.warn('Cannot animate non-existent property "' + name + '"')
                        continue
                    }
                    let k = kind || p.kind
                    result.push({ p, kind: k, from: this.$fromIsDefined ? convert(k, this.$v[PropertyAnimation.FROM]) : undefined, to: convert(k, this.$v[PropertyAnimation.TO]) })
                    for(let action of actions){
                        if(action.p.obj === p.obj && action.p.id === p.id){
                            modified.push(action)
                            break
                        }
                    }
                }
            }
        } else {
            for(let action of actions){
                let p = action.p
                let byTarget = !targets.length || targets.indexOf(p.target) >= 0 || targets.indexOf(p.obj) >= 0
                let byName = props.indexOf(p.path) >= 0 || props.indexOf(p.name) >= 0
                if(!byTarget || !byName) continue
                let k = kind || p.kind
                result.push({
                    p, kind: k,
                    from: this.$fromIsDefined ? convert(k, this.$v[PropertyAnimation.FROM]) : undefined,
                    to: this.$toIsDefined ? convert(k, this.$v[PropertyAnimation.TO]) : convert(k, action.to),
                })
                modified.push(action)
                action.from = action.to
            }
        }
        this.$actions = result
        this.$fromIsSourced = false
    }

    // QQuickAnimationPropertyUpdater::setValue
    $updateCurrentTime(time){
        let dura = this.$duration()
        let progress = dura > 0 ? time / dura : 1
        let easing = this.$v[PropertyAnimation.EASING]
        let v = easing ? easing.$apply(progress) : progress
        if(this.$reverse) v = 1 - v
        for(let action of this.$actions){
            if(action.p.obj.$f & engine.DESTROYED) continue
            if(v === 1){
                writeProperty(action.p, action.to)
            } else {
                if(!this.$fromIsSourced && !this.$fromIsDefined) action.from = convert(action.kind, readProperty(action.p))
                writeProperty(action.p, interpolate(action.kind, action.from, action.to, v))
            }
        }
        this.$fromIsSourced = true
    }
}

const fromDefined = (anim) => { anim.$fromIsDefined = true }
const toDefined = (anim) => { anim.$toIsDefined = true }

defineType(PropertyAnimation, {
    props: {
        duration: { kind: 'int', initial: 250 },
        from: { kind: 'var', effect: fromDefined },
        to: { kind: 'var', effect: toDefined },
        target: 'var',
        targets: 'var',
        property: 'string',
        properties: 'string',
        exclude: 'var',
        easing: { group: EasingGroup },
    },
})

module.exports = { PropertyAnimation }
