// QML value types point / size / rect: plain fields, printed like Qt prints them (QPointF(1, 2))
class ValueType {}

class QPointF extends ValueType {
    constructor(x = 0, y = 0){
        super()
        this.x = x
        this.y = y
    }
    toString(){ return 'QPointF(' + this.x + ', ' + this.y + ')' }
}

class QSizeF extends ValueType {
    constructor(width = -1, height = -1){
        super()
        this.width = width
        this.height = height
    }
    toString(){ return 'QSizeF(' + this.width + ', ' + this.height + ')' }
}

class QRectF extends ValueType {
    constructor(x = 0, y = 0, width = 0, height = 0){
        super()
        this.x = x
        this.y = y
        this.width = width
        this.height = height
    }
    get left(){ return this.x }
    get top(){ return this.y }
    get right(){ return this.x + this.width }
    get bottom(){ return this.y + this.height }
    toString(){ return 'QRectF(' + this.x + ', ' + this.y + ', ' + this.width + ', ' + this.height + ')' }
}

// console.log(Qt.size(1, 2)) prints the value type, as QML does
function installConsole(){
    if(typeof console === 'undefined' || console.$jq2) return
    console.$jq2 = true
    for(let method of ['log', 'debug', 'info', 'warn', 'error']){
        let original = console[method]
        if(typeof original !== 'function') continue
        console[method] = function(...args){
            for(let i = 0; i < args.length; i++){
                if(args[i] instanceof ValueType) args[i] = String(args[i])
            }
            return original.apply(console, args)
        }
    }
}

// ---- vector2d / vector3d / vector4d / quaternion / matrix4x4 (QQuickVector3DValueType & co., qquickvaluetypes.cpp) --------
// The components are floats, as QVector3D keeps them; toString prints them as %g

const F = Math.fround

// printf %g: six significant digits, no trailing zeros, an exponent below 1e-4 and from 1e6
function formatG(x){
    if(!Number.isFinite(x)) return Number.isNaN(x) ? 'nan' : (x < 0 ? '-inf' : 'inf')
    if(x === 0) return Object.is(x, -0) ? '-0' : '0'
    let exp = Math.floor(Math.log10(Math.abs(x)))
    let p = Number(x.toPrecision(6))
    if(p !== 0) exp = Math.floor(Math.log10(Math.abs(p)))
    if(exp < -4 || exp >= 6){
        let [m, e] = p.toExponential(5).split('e')
        m = m.indexOf('.') >= 0 ? m.replace(/0+$/, '').replace(/\.$/, '') : m
        let n = Math.abs(Number(e))
        return m + 'e' + (Number(e) < 0 ? '-' : '+') + (n < 10 ? '0' + n : String(n))
    }
    return String(p)
}

const fuzzyCompare = (a, b) => Math.abs(a - b) * 100000 <= Math.min(Math.abs(a), Math.abs(b))
const fuzzyIsNull = (a) => Math.abs(a) <= 0.00001

class QVector2D extends ValueType {
    constructor(x = 0, y = 0){ super(); this.x = F(x); this.y = F(y) }
    toString(){ return 'QVector2D(' + formatG(this.x) + ', ' + formatG(this.y) + ')' }
    dotProduct(v){ return F(this.x * v.x + this.y * v.y) }
    times(v){ return typeof v === 'number' ? new QVector2D(this.x * v, this.y * v) : new QVector2D(this.x * v.x, this.y * v.y) }
    plus(v){ return new QVector2D(this.x + v.x, this.y + v.y) }
    minus(v){ return new QVector2D(this.x - v.x, this.y - v.y) }
    length(){ return F(Math.hypot(this.x, this.y)) }
    normalized(){ let l = Math.hypot(this.x, this.y); return l ? new QVector2D(this.x / l, this.y / l) : new QVector2D() }
    toVector3d(){ return new QVector3D(this.x, this.y, 0) }
    toVector4d(){ return new QVector4D(this.x, this.y, 0, 0) }
    fuzzyEquals(v, epsilon){ return vectorFuzzy([this.x, this.y], [v.x, v.y], epsilon) }
}

class QVector3D extends ValueType {
    constructor(x = 0, y = 0, z = 0){ super(); this.x = F(x); this.y = F(y); this.z = F(z) }
    toString(){ return 'QVector3D(' + formatG(this.x) + ', ' + formatG(this.y) + ', ' + formatG(this.z) + ')' }
    crossProduct(v){ return new QVector3D(this.y * v.z - this.z * v.y, this.z * v.x - this.x * v.z, this.x * v.y - this.y * v.x) }
    dotProduct(v){ return F(this.x * v.x + this.y * v.y + this.z * v.z) }
    // QMatrix4x4 * this (map with w = 1), a component product with a vector, or a scale
    times(v){
        if(v instanceof QMatrix4x4) return v.$map3(this)
        if(typeof v === 'number') return new QVector3D(this.x * v, this.y * v, this.z * v)
        return new QVector3D(this.x * v.x, this.y * v.y, this.z * v.z)
    }
    plus(v){ return new QVector3D(this.x + v.x, this.y + v.y, this.z + v.z) }
    minus(v){ return new QVector3D(this.x - v.x, this.y - v.y, this.z - v.z) }
    length(){ return F(Math.hypot(this.x, this.y, this.z)) }
    normalized(){ let l = Math.hypot(this.x, this.y, this.z); return l ? new QVector3D(this.x / l, this.y / l, this.z / l) : new QVector3D() }
    toVector2d(){ return new QVector2D(this.x, this.y) }
    toVector4d(){ return new QVector4D(this.x, this.y, this.z, 0) }
    fuzzyEquals(v, epsilon){ return vectorFuzzy([this.x, this.y, this.z], [v.x, v.y, v.z], epsilon) }
}

class QVector4D extends ValueType {
    constructor(x = 0, y = 0, z = 0, w = 0){ super(); this.x = F(x); this.y = F(y); this.z = F(z); this.w = F(w) }
    toString(){ return 'QVector4D(' + [this.x, this.y, this.z, this.w].map(formatG).join(', ') + ')' }
    dotProduct(v){ return F(this.x * v.x + this.y * v.y + this.z * v.z + this.w * v.w) }
    times(v){
        if(v instanceof QMatrix4x4) return v.$map4(this, true)
        if(typeof v === 'number') return new QVector4D(this.x * v, this.y * v, this.z * v, this.w * v)
        return new QVector4D(this.x * v.x, this.y * v.y, this.z * v.z, this.w * v.w)
    }
    plus(v){ return new QVector4D(this.x + v.x, this.y + v.y, this.z + v.z, this.w + v.w) }
    minus(v){ return new QVector4D(this.x - v.x, this.y - v.y, this.z - v.z, this.w - v.w) }
    length(){ return F(Math.hypot(this.x, this.y, this.z, this.w)) }
    normalized(){
        let l = Math.hypot(this.x, this.y, this.z, this.w)
        return l ? new QVector4D(this.x / l, this.y / l, this.z / l, this.w / l) : new QVector4D()
    }
    toVector2d(){ return new QVector2D(this.x, this.y) }
    toVector3d(){ return new QVector3D(this.x, this.y, this.z) }
    fuzzyEquals(v, epsilon){ return vectorFuzzy([this.x, this.y, this.z, this.w], [v.x, v.y, v.z, v.w], epsilon) }
}

// QQuickVector3DValueType::fuzzyEquals: within epsilon a component; without one qFuzzyCompare (qFuzzyIsNull at zero)
function vectorFuzzy(a, b, epsilon){
    if(epsilon !== undefined) return a.every((x, i) => Math.abs(x - b[i]) <= Number(epsilon))
    return a.every((x, i) => (fuzzyIsNull(x) && fuzzyIsNull(b[i])) || fuzzyCompare(x, b[i]))
}

class QQuaternion extends ValueType {
    constructor(scalar = 1, x = 0, y = 0, z = 0){ super(); this.scalar = F(scalar); this.x = F(x); this.y = F(y); this.z = F(z) }
    toString(){ return 'QQuaternion(' + [this.scalar, this.x, this.y, this.z].map(formatG).join(', ') + ')' }
    dotProduct(q){ return F(this.scalar * q.scalar + this.x * q.x + this.y * q.y + this.z * q.z) }
    // the Hamilton product, the rotation of a vector3d, or a scale
    times(q){
        if(typeof q === 'number') return new QQuaternion(this.scalar * q, this.x * q, this.y * q, this.z * q)
        if(q instanceof QVector3D){
            let p = this.times(new QQuaternion(0, q.x, q.y, q.z)).times(this.conjugated())
            return new QVector3D(p.x, p.y, p.z)
        }
        let a = this
        return new QQuaternion(a.scalar * q.scalar - a.x * q.x - a.y * q.y - a.z * q.z, a.scalar * q.x + a.x * q.scalar + a.y * q.z - a.z * q.y,
            a.scalar * q.y - a.x * q.z + a.y * q.scalar + a.z * q.x, a.scalar * q.z + a.x * q.y - a.y * q.x + a.z * q.scalar)
    }
    plus(q){ return new QQuaternion(this.scalar + q.scalar, this.x + q.x, this.y + q.y, this.z + q.z) }
    minus(q){ return new QQuaternion(this.scalar - q.scalar, this.x - q.x, this.y - q.y, this.z - q.z) }
    length(){ return F(Math.hypot(this.scalar, this.x, this.y, this.z)) }
    normalized(){ let l = Math.hypot(this.scalar, this.x, this.y, this.z); return l ? this.times(1 / l) : new QQuaternion(0, 0, 0, 0) }
    conjugated(){ return new QQuaternion(this.scalar, -this.x, -this.y, -this.z) }
    inverted(){
        let n = this.scalar * this.scalar + this.x * this.x + this.y * this.y + this.z * this.z
        return n ? new QQuaternion(this.scalar / n, -this.x / n, -this.y / n, -this.z / n) : new QQuaternion(0, 0, 0, 0)
    }
    toVector4d(){ return new QVector4D(this.x, this.y, this.z, this.scalar) }
    // QQuaternion::toEulerAngles: pitch (x), yaw (y), roll (z) in degrees
    toEulerAngles(){
        let q = this.normalized()
        let xx = q.x * q.x, xy = q.x * q.y, xz = q.x * q.z, xw = q.x * q.scalar
        let yy = q.y * q.y, yz = q.y * q.z, yw = q.y * q.scalar, zz = q.z * q.z, zw = q.z * q.scalar
        let sinp = -2 * (yz - xw)
        let pitch, yaw, roll
        if(Math.abs(sinp) >= 1){
            pitch = Math.sign(sinp) * Math.PI / 2
            yaw = 2 * Math.atan2(q.y, q.scalar)
            roll = 0
        } else {
            pitch = Math.asin(sinp)
            yaw = Math.atan2(2 * (xz + yw), 1 - 2 * (xx + yy))
            roll = Math.atan2(2 * (xy + zw), 1 - 2 * (xx + zz))
        }
        let deg = 180 / Math.PI
        return new QVector3D(pitch * deg, yaw * deg, roll * deg)
    }
    fuzzyEquals(q, epsilon){ return vectorFuzzy([this.scalar, this.x, this.y, this.z], [q.scalar, q.x, q.y, q.z], epsilon) }
}

// QMatrix4x4: m11..m44 by row; Qt.matrix4x4(m11, m12, ..., m44) gives them in that order
class QMatrix4x4 extends ValueType {
    constructor(values){
        super()
        this.$m = (values || [1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1]).map(v => F(v))
    }
    toString(){ return 'QMatrix4x4(' + this.$m.map(formatG).join(', ') + ')' }
    $at(row, column){ return this.$m[row * 4 + column] }
    row(i){ let m = this.$m; return new QVector4D(m[i * 4], m[i * 4 + 1], m[i * 4 + 2], m[i * 4 + 3]) }
    column(j){ let m = this.$m; return new QVector4D(m[j], m[4 + j], m[8 + j], m[12 + j]) }
    times(v){
        if(v instanceof QMatrix4x4){
            let r = []
            for(let i = 0; i < 4; i++) for(let j = 0; j < 4; j++){
                let sum = 0
                for(let k = 0; k < 4; k++) sum += this.$at(i, k) * v.$at(k, j)
                r.push(sum)
            }
            return new QMatrix4x4(r)
        }
        if(v instanceof QVector4D) return this.$map4(v, false)
        if(v instanceof QVector3D) return this.$map3(v)
        return new QMatrix4x4(this.$m.map(x => x * v))
    }
    // this * (x, y, z, 1), divided by w
    $map3(v){
        let r = [0, 1, 2, 3].map(i => this.$at(i, 0) * v.x + this.$at(i, 1) * v.y + this.$at(i, 2) * v.z + this.$at(i, 3))
        return r[3] === 1 || r[3] === 0 ? new QVector3D(r[0], r[1], r[2]) : new QVector3D(r[0] / r[3], r[1] / r[3], r[2] / r[3])
    }
    // this * v (a column), or v * this (a row) when the vector is on the left
    $map4(v, vectorFirst){
        let c = [v.x, v.y, v.z, v.w]
        let r = [0, 1, 2, 3].map(i => [0, 1, 2, 3].reduce((sum, k) => sum + (vectorFirst ? c[k] * this.$at(k, i) : this.$at(i, k) * c[k]), 0))
        return new QVector4D(r[0], r[1], r[2], r[3])
    }
    plus(m){ return new QMatrix4x4(this.$m.map((x, i) => x + m.$m[i])) }
    minus(m){ return new QMatrix4x4(this.$m.map((x, i) => x - m.$m[i])) }
    transposed(){ let r = []; for(let i = 0; i < 4; i++) for(let j = 0; j < 4; j++) r.push(this.$at(j, i)); return new QMatrix4x4(r) }
    determinant(){ return F(det4(this.$m)) }
    inverted(){
        let m = this.$m, d = det4(m)
        if(d === 0) return new QMatrix4x4()
        let r = []
        for(let i = 0; i < 4; i++) for(let j = 0; j < 4; j++) r.push(cofactor(m, j, i) / d + 0)
        return new QMatrix4x4(r)
    }
    fuzzyEquals(m, epsilon){ return vectorFuzzy(this.$m, m.$m, epsilon) }
}
for(let i = 1; i <= 4; i++) for(let j = 1; j <= 4; j++){
    Object.defineProperty(QMatrix4x4.prototype, 'm' + i + j, {
        get(){ return this.$m[(i - 1) * 4 + (j - 1)] },
        set(value){ this.$m[(i - 1) * 4 + (j - 1)] = F(value) },
    })
}

function minor3(m, skipRow, skipColumn){
    let a = []
    for(let i = 0; i < 4; i++) if(i !== skipRow) for(let j = 0; j < 4; j++) if(j !== skipColumn) a.push(m[i * 4 + j])
    return a[0] * (a[4] * a[8] - a[5] * a[7]) - a[1] * (a[3] * a[8] - a[5] * a[6]) + a[2] * (a[3] * a[7] - a[4] * a[6])
}
const cofactor = (m, row, column) => ((row + column) % 2 ? -1 : 1) * minor3(m, row, column)
const det4 = (m) => m[0] * cofactor(m, 0, 0) + m[1] * cofactor(m, 0, 1) + m[2] * cofactor(m, 0, 2) + m[3] * cofactor(m, 0, 3)

// ---- font (Qt.font, QQuickFontValueType) ----------------------------------------------------------------------------
// The properties the specifier set are its own (an assignment to item.font takes those); the rest read their defaults
class QFont extends ValueType {
    toString(){
        // QFont::toString: family, pointSizeF, pixelSize, styleHint, weight, style, underline, strikeOut, fixedPitch,
        // 0, capitalization, letterSpacingType, letterSpacing, wordSpacing, stretch, styleStrategy, styleName, ...
        let pixel = this.$pixel === true
        return 'QFont(' + [this.family, pixel ? -1 : this.pointSize, pixel ? this.pixelSize : -1, 5, this.weight, this.italic ? 1 : 0,
            this.underline ? 1 : 0, this.strikeout ? 1 : 0, 0, 0, this.capitalization, 0, this.letterSpacing, this.wordSpacing, 0, 1, this.styleName,
            0, 0].join(',') + ')'
    }
}
const FONT_DEFAULTS = { family: '', pointSize: 9, pixelSize: 12, weight: 400, bold: false, italic: false, underline: false,
    strikeout: false, capitalization: 0, letterSpacing: 0, wordSpacing: 0, kerning: true, preferShaping: true, styleName: '',
    hintingPreference: 0 }
for(let key of Object.keys(FONT_DEFAULTS)){
    Object.defineProperty(QFont.prototype, key, { get(){ return FONT_DEFAULTS[key] }, set(value){ Object.defineProperty(this, key, { value, writable: true, enumerable: true, configurable: true }) }, configurable: true })
}

// Qt.font(specifier): the known keys of the object (QQuickFontValueType::create), bold as weight 700
function makeFont(specifier){
    let font = new QFont()
    for(let key of Object.keys(specifier)){
        if(!Object.prototype.hasOwnProperty.call(FONT_DEFAULTS, key)) continue
        font[key] = specifier[key]
    }
    if(specifier.bold === true && !('weight' in specifier)) font.weight = 700
    if(specifier.pixelSize > 0 && !('pointSize' in specifier)){
        Object.defineProperty(font, '$pixel', { value: true })
        font.pointSize = specifier.pixelSize * 72 / 96
    }
    return font
}

module.exports = { ValueType, QPointF, QSizeF, QRectF, QVector2D, QVector3D, QVector4D, QQuaternion, QMatrix4x4, QFont, makeFont, formatG, installConsole }
