// QEasingCurve (qtbase src/corelib/tools/qeasingcurve.cpp, easing.cpp): progress 0..1 -> value
const PI = Math.PI

const quad = { in: t => t * t, out: t => -t * (t - 2) }
const cubic = { in: t => t * t * t, out: t => { t -= 1; return t * t * t + 1 } }
const quart = { in: t => t * t * t * t, out: t => { t -= 1; return -(t * t * t * t - 1) } }
const quint = { in: t => t * t * t * t * t, out: t => { t -= 1; return t * t * t * t * t + 1 } }
const sine = { in: t => 1 - Math.cos(t * PI / 2), out: t => Math.sin(t * PI / 2) }
const expo = {
    in: t => (t === 0 || t === 1) ? t : Math.pow(2, 10 * (t - 1)) - 0.001,
    out: t => t === 1 ? 1 : 1.001 * (-Math.pow(2, -10 * t) + 1),
}
const circ = { in: t => -(Math.sqrt(1 - t * t) - 1), out: t => { t -= 1; return Math.sqrt(1 - t * t) } }

const outIn = (curve) => (t) => t < 0.5 ? curve.out(2 * t) / 2 : curve.in(2 * t - 1) / 2 + 0.5

function inOutQuad(t){ t *= 2; if(t < 1) return t * t / 2; --t; return -0.5 * (t * (t - 2) - 1) }
function inOutCubic(t){ t *= 2; if(t < 1) return 0.5 * t * t * t; t -= 2; return 0.5 * (t * t * t + 2) }
function inOutQuart(t){ t *= 2; if(t < 1) return 0.5 * t * t * t * t; t -= 2; return -0.5 * (t * t * t * t - 2) }
function inOutQuint(t){ t *= 2; if(t < 1) return 0.5 * t * t * t * t * t; t -= 2; return 0.5 * (t * t * t * t * t + 2) }
function inOutSine(t){ return -0.5 * (Math.cos(PI * t) - 1) }
function inOutExpo(t){
    if(t === 0) return 0
    if(t === 1) return 1
    t *= 2
    if(t < 1) return 0.5 * Math.pow(2, 10 * (t - 1)) - 0.0005
    return 0.5 * 1.0005 * (-Math.pow(2, -10 * (t - 1)) + 2)
}
function inOutCirc(t){ t *= 2; if(t < 1) return -0.5 * (Math.sqrt(1 - t * t) - 1); t -= 2; return 0.5 * (Math.sqrt(1 - t * t) + 1) }

function elasticS(a, c, p){ return a < Math.abs(c) ? p / 4 : p / (2 * PI) * Math.asin(c / a) }
function inElasticHelper(t, b, c, d, a, p){
    if(t === 0) return b
    let tAdj = t / d
    if(tAdj === 1) return b + c
    let s
    if(a < Math.abs(c)){ a = c; s = p / 4 } else s = p / (2 * PI) * Math.asin(c / a)
    tAdj -= 1
    return -(a * Math.pow(2, 10 * tAdj) * Math.sin((tAdj * d - s) * (2 * PI) / p)) + b
}
function outElasticHelper(t, b, c, d, a, p){
    if(t === 0) return 0
    if(t === 1) return c
    let s
    if(a < c){ a = c; s = p / 4 } else s = p / (2 * PI) * Math.asin(c / a)
    return a * Math.pow(2, -10 * t) * Math.sin((t - s) * (2 * PI) / p) + c
}
function inOutElastic(t, a, p){
    if(t === 0) return 0
    t *= 2
    if(t === 2) return 1
    let s
    if(a < 1){ a = 1; s = p / 4 } else s = p / (2 * PI) * Math.asin(1 / a)
    if(t < 1) return -0.5 * (a * Math.pow(2, 10 * (t - 1)) * Math.sin((t - 1 - s) * (2 * PI) / p))
    return a * Math.pow(2, -10 * (t - 1)) * Math.sin((t - 1 - s) * (2 * PI) / p) * 0.5 + 1
}
function outInElastic(t, a, p){
    if(t < 0.5) return outElasticHelper(t * 2, 0, 0.5, 1, a, p)
    return inElasticHelper(2 * t - 1, 0.5, 0.5, 1, a, p)
}

function inBack(t, s){ return t * t * ((s + 1) * t - s) }
function outBack(t, s){ t -= 1; return t * t * ((s + 1) * t + s) + 1 }
function inOutBack(t, s){
    t *= 2
    if(t < 1){ s *= 1.525; return 0.5 * (t * t * ((s + 1) * t - s)) }
    t -= 2; s *= 1.525
    return 0.5 * (t * t * ((s + 1) * t + s) + 2)
}
function outInBack(t, s){ return t < 0.5 ? outBack(2 * t, s) / 2 : inBack(2 * t - 1, s) / 2 + 0.5 }

function outBounceHelper(t, c, a){
    if(t === 1) return c
    if(t < 4 / 11) return c * (7.5625 * t * t)
    if(t < 8 / 11){ t -= 6 / 11; return -a * (1 - (7.5625 * t * t + 0.75)) + c }
    if(t < 10 / 11){ t -= 9 / 11; return -a * (1 - (7.5625 * t * t + 0.9375)) + c }
    t -= 21 / 22
    return -a * (1 - (7.5625 * t * t + 0.984375)) + c
}
function inBounce(t, a){ return 1 - outBounceHelper(1 - t, 1, a) }
function outBounce(t, a){ return outBounceHelper(t, 1, a) }
function inOutBounce(t, a){ return t < 0.5 ? inBounce(2 * t, a) / 2 : (t === 1 ? 1 : outBounce(2 * t - 1, a) / 2 + 0.5) }
function outInBounce(t, a){ return t < 0.5 ? outBounceHelper(t * 2, 0.5, a) : 1 - outBounceHelper(2 - 2 * t, 0.5, a) }

// Cubic bezier segments [c1x, c1y, c2x, c2y, endX, endY, ...] from (0,0), solved for x by bisection
function bezier(points, t){
    let x0 = 0, y0 = 0
    for(let i = 0; i + 5 < points.length; i += 6){
        let x3 = points[i + 4], y3 = points[i + 5]
        if(t <= x3 || i + 6 >= points.length){
            let x1 = points[i], y1 = points[i + 1], x2 = points[i + 2], y2 = points[i + 3]
            let at = (a, b, c, d, s) => { let u = 1 - s; return u * u * u * a + 3 * u * u * s * b + 3 * u * s * s * c + s * s * s * d }
            let lo = 0, hi = 1, s = 0.5
            for(let k = 0; k < 40; k++){
                s = (lo + hi) / 2
                if(at(x0, x1, x2, x3, s) < t) lo = s
                else hi = s
            }
            return at(y0, y1, y2, y3, s)
        }
        x0 = x3
        y0 = y3
    }
    return t
}

const IN_OUT = [
    null,
    quad.in, quad.out, inOutQuad, outIn(quad),
    cubic.in, cubic.out, inOutCubic, outIn(cubic),
    quart.in, quart.out, inOutQuart, outIn(quart),
    quint.in, quint.out, inOutQuint, outIn(quint),
    sine.in, sine.out, inOutSine, outIn(sine),
    expo.in, expo.out, inOutExpo, outIn(expo),
    circ.in, circ.out, inOutCirc, outIn(circ),
]

// type, amplitude, overshoot, period, bezierCurve as on the QML easing group
function ease(type, t, amplitude = 1, overshoot = 1.70158, period = 0.3, curve = null){
    if(t <= 0) return 0
    if(t >= 1) return 1
    if(type > 0 && type < IN_OUT.length) return IN_OUT[type](t)
    switch(type){
        case 29: return inElasticHelper(t, 0, 1, 1, amplitude, period)
        case 30: return outElasticHelper(t, 0, 1, 1, amplitude, period)
        case 31: return inOutElastic(t, amplitude, period)
        case 32: return outInElastic(t, amplitude, period)
        case 33: return inBack(t, overshoot)
        case 34: return outBack(t, overshoot)
        case 35: return inOutBack(t, overshoot)
        case 36: return outInBack(t, overshoot)
        case 37: return inBounce(t, amplitude)
        case 38: return outBounce(t, amplitude)
        case 39: return inOutBounce(t, amplitude)
        case 40: return outInBounce(t, amplitude)
        case 43: return (Math.sin(t * PI * 2 - PI / 2) + 1) / 2     // SineCurve
        case 44: return (Math.cos(t * PI * 2 - PI / 2) + 1) / 2     // CosineCurve
        case 45: return curve && curve.length >= 6 ? bezier(curve, t) : t
        default: return t
    }
}

const Easing = {
    Linear: 0,
    InQuad: 1, OutQuad: 2, InOutQuad: 3, OutInQuad: 4,
    InCubic: 5, OutCubic: 6, InOutCubic: 7, OutInCubic: 8,
    InQuart: 9, OutQuart: 10, InOutQuart: 11, OutInQuart: 12,
    InQuint: 13, OutQuint: 14, InOutQuint: 15, OutInQuint: 16,
    InSine: 17, OutSine: 18, InOutSine: 19, OutInSine: 20,
    InExpo: 21, OutExpo: 22, InOutExpo: 23, OutInExpo: 24,
    InCirc: 25, OutCirc: 26, InOutCirc: 27, OutInCirc: 28,
    InElastic: 29, OutElastic: 30, InOutElastic: 31, OutInElastic: 32,
    InBack: 33, OutBack: 34, InOutBack: 35, OutInBack: 36,
    InBounce: 37, OutBounce: 38, InOutBounce: 39, OutInBounce: 40,
    InCurve: 41, OutCurve: 42, SineCurve: 43, CosineCurve: 44, BezierSpline: 45, TCBSpline: 46,
}

module.exports = { ease, Easing }
