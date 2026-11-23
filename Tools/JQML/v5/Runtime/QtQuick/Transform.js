// Item.transform of QtQuick (qquicktranslate.cpp, Qt 6.12): Translate, Scale, Rotation, Matrix4x4. An item takes them
// after its position and before its own scale and rotation (QQuickItemPrivate::itemToParentTransform); the first of the
// list is applied to the item first. Each one is drawn as CSS transform functions; a change redraws the items that use it.
const engine = require('../Core/engine')
const { defineType } = require('../Core/type')
const { QtObject, GroupObject } = require('../Core/QtObject')

const GEOMETRY = 1
// a point or an axis (QVector3D): x, y, z
class Vector3D extends GroupObject {
    $write(id, value){
        super.$write(id, value)
        let owner = this.$owner
        if(owner && owner.$changed) owner.$changed()
    }
}
defineType(Vector3D, { props: { x: 'real', y: 'real', z: 'real' } })

class Transform extends QtObject {
    constructor(parent = null){
        super(parent)
        this.$items = new Set()
    }
    // the items that use it are drawn again
    $changed(){ for(let item of this.$items) if(!(item.$f & engine.DESTROYED)) engine.markDirty(item, GEOMETRY) }
    $write(id, value){
        super.$write(id, value)
        this.$changed()
    }
    $css(){ return '' }
}
defineType(Transform, {})

// QQuickTranslate::applyTo: matrix->translate(x, y)
class Translate extends Transform {
    $css(){ return 'translate(' + this.$v[Translate.X] + 'px,' + this.$v[Translate.Y] + 'px)' }
}
defineType(Translate, { props: { x: 'real', y: 'real' } })

// QQuickScale::applyTo: translate(origin), scale(xScale, yScale, zScale), translate(-origin)
class Scale extends Transform {
    constructor(parent = null){
        super(parent)
        this.$plain(Scale.XSCALE, 1)
        this.$plain(Scale.YSCALE, 1)
        this.$plain(Scale.ZSCALE, 1)
    }
    $plain(id, value){ QtObject.prototype.$write.call(this, id, value) }
    $css(){
        let o = this.origin
        return 'translate(' + o.x + 'px,' + o.y + 'px) scale(' + this.$v[Scale.XSCALE] + ',' + this.$v[Scale.YSCALE] + ') translate(' + -o.x + 'px,' + -o.y + 'px)'
    }
}
defineType(Scale, { props: { origin: { group: Vector3D }, xScale: 'real', yScale: 'real', zScale: 'real' } })
Object.defineProperty(Scale.prototype, 'scale', {
    get(){ return this.xScale },
    set(value){ engine.batch(() => { this.xScale = value; this.yScale = value }) },
    configurable: true,
})

// QQuickRotation::applyTo: translate(origin), rotate(angle, axis), translate(-origin); the axis 0, 0, 1 by default
class Rotation extends Transform {
    constructor(parent = null){
        super(parent)
        let axis = this.axis
        GroupObject.prototype.$write.call(axis, Vector3D.Z, 1)
    }
    $css(){
        let o = this.origin, a = this.axis, angle = this.$v[Rotation.ANGLE]
        let turn = a.x === 0 && a.y === 0 ? 'rotate(' + angle + 'deg)' : 'rotate3d(' + a.x + ',' + a.y + ',' + a.z + ',' + angle + 'deg)'
        return 'translate(' + o.x + 'px,' + o.y + 'px) ' + turn + ' translate(' + -o.x + 'px,' + -o.y + 'px)'
    }
}
defineType(Rotation, { props: { origin: { group: Vector3D }, axis: { group: Vector3D }, angle: 'real' } })

// Matrix4x4 { matrix: Qt.matrix4x4(...) }: a matrix of 16 values, by rows (QMatrix4x4)
class Matrix4x4 extends Transform {
    $css(){
        let m = this.$v[Matrix4x4.MATRIX]
        if(!m) return ''
        let values = Array.isArray(m) ? m : ['m11', 'm12', 'm13', 'm14', 'm21', 'm22', 'm23', 'm24', 'm31', 'm32', 'm33', 'm34', 'm41', 'm42', 'm43', 'm44'].map(k => m[k])
        if(values.length !== 16 || values.some(v => typeof v !== 'number')) return ''
        // CSS matrix3d is by columns
        let columns = []
        for(let c = 0; c < 4; c++) for(let r = 0; r < 4; r++) columns.push(values[r * 4 + c])
        return 'matrix3d(' + columns.join(',') + ')'
    }
}
defineType(Matrix4x4, { props: { matrix: 'var' } })

// the transforms of an item, as Item.transform holds them (one or a list)
function transformsOf(value){
    if(!value) return []
    let list = Array.isArray(value) ? value : (typeof value.length === 'number' && !(value instanceof Transform) ? Array.from(value) : [value])
    return list.filter(t => t instanceof Transform)
}
// Item.transform set: the item follows its transforms
function setItemTransforms(item, value, old){
    for(let t of transformsOf(old)) t.$items.delete(item)
    for(let t of transformsOf(value)) t.$items.add(item)
    engine.markDirty(item, GEOMETRY)
}
// the CSS of the list: the first one applied first, so written last
function transformsCss(value){
    let list = transformsOf(value)
    let out = ''
    for(let i = list.length - 1; i >= 0; i--){
        let css = list[i].$css()
        if(css) out += ' ' + css
    }
    return out
}

// ---- 2D affine matrices [a, b, c, d, e, f]: x' = a x + c y + e, y' = b x + d y + f (QTransform) ----
const IDENTITY = [1, 0, 0, 1, 0, 0]
const mul = (m, n) => [m[0] * n[0] + m[2] * n[1], m[1] * n[0] + m[3] * n[1], m[0] * n[2] + m[2] * n[3], m[1] * n[2] + m[3] * n[3],
    m[0] * n[4] + m[2] * n[5] + m[4], m[1] * n[4] + m[3] * n[5] + m[5]]
const translateM = (x, y) => [1, 0, 0, 1, x, y]
const scaleM = (x, y) => [x, 0, 0, y, 0, 0]
const rotateM = (deg) => { let r = deg * Math.PI / 180, c = Math.cos(r), s = Math.sin(r); return [c, s, -s, c, 0, 0] }
const around = (o, m) => mul(mul(translateM(o.x, o.y), m), translateM(-o.x, -o.y))
// the matrix of a transform (a rotation about the x or y axis seen from the front: its cosine along the other axis)
function matrixOf(t){
    let v = t.$v
    if(t instanceof Translate) return translateM(v[Translate.X], v[Translate.Y])
    if(t instanceof Scale) return around(t.origin, scaleM(v[Scale.XSCALE], v[Scale.YSCALE]))
    if(t instanceof Rotation){
        let a = t.axis, angle = v[Rotation.ANGLE]
        if(a.x === 0 && a.y === 0) return around(t.origin, rotateM(a.z < 0 ? -angle : angle))
        let c = Math.cos(angle * Math.PI / 180)
        return around(t.origin, scaleM(a.y !== 0 ? c : 1, a.x !== 0 ? c : 1))
    }
    if(t instanceof Matrix4x4){
        let m = v[Matrix4x4.MATRIX]
        if(!m) return IDENTITY
        let g = Array.isArray(m) ? (i) => m[i] : (i) => m['m' + (Math.floor(i / 4) + 1) + (i % 4 + 1)]
        return [g(0), g(4), g(1), g(5), g(3), g(7)]
    }
    return IDENTITY
}
const ORIGIN_FRACTIONS = [[0, 0], [0.5, 0], [1, 0], [0, 0.5], [0.5, 0.5], [1, 0.5], [0, 1], [0.5, 1], [1, 1]]
// QQuickItemPrivate::itemToParentTransform: the position, Item.transform (the first applied first), then the scale and
// the rotation about the transform origin; null when it is only the position
function itemToParent(item, Item){
    let v = item.$v
    let list = v[Item.TRANSFORM] ? transformsOf(v[Item.TRANSFORM]) : []
    let scaled = v[Item.SCALE] !== 1 || v[Item.ROTATION] !== 0
    if(!list.length && !scaled) return null
    let m = translateM(v[Item.X], v[Item.Y])
    for(let i = list.length - 1; i >= 0; i--) m = mul(m, matrixOf(list[i]))
    if(scaled){
        let f = ORIGIN_FRACTIONS[v[Item.TRANSFORMORIGIN]] || ORIGIN_FRACTIONS[4]
        let o = { x: f[0] * v[Item.WIDTH], y: f[1] * v[Item.HEIGHT] }
        m = mul(m, around(o, mul(scaleM(v[Item.SCALE], v[Item.SCALE]), rotateM(v[Item.ROTATION]))))
    }
    return m
}
// the matrix from an item to the scene, or null when only positions are on the way
function itemToScene(item, Item){
    let m = null, x = 0, y = 0
    for(let it = item; it instanceof Item; it = it.$parent){
        let t = itemToParent(it, Item)
        if(!t){
            if(m) m = mul(translateM(it.$v[Item.X], it.$v[Item.Y]), m)
            else { x += it.$v[Item.X]; y += it.$v[Item.Y] }
            continue
        }
        if(!m) m = mul(t, translateM(x, y))
        else m = mul(t, m)
    }
    return m || (x || y ? { x, y } : { x: 0, y: 0 })
}
function invert(m){
    let det = m[0] * m[3] - m[1] * m[2]
    if(!det) return null
    let a = m[3] / det, b = -m[1] / det, c = -m[2] / det, d = m[0] / det
    return [a, b, c, d, -(a * m[4] + c * m[5]), -(b * m[4] + d * m[5])]
}
const apply = (m, x, y) => ({ x: m[0] * x + m[2] * y + m[4], y: m[1] * x + m[3] * y + m[5] })

module.exports = { mul, invert, apply, itemToScene, Transform, Translate, Scale, Rotation, Matrix4x4, Vector3D, setItemTransforms, transformsCss, transformsOf }
