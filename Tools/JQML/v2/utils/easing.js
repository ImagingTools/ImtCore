// QML Easing curves implementation
// See: https://doc.qt.io/qt-6/qml-qtquick-propertyanimation.html#easing-prop

const PI = Math.PI
const HALF_PI = PI / 2

function linear(t){ return t }

// Quad
function inQuad(t){ return t*t }
function outQuad(t){ return t*(2-t) }
function inOutQuad(t){ return t<0.5 ? 2*t*t : -1+(4-2*t)*t }
function outInQuad(t){ return t<0.5 ? 0.5*outQuad(2*t) : 0.5+0.5*inQuad(2*t-1) }

// Cubic
function inCubic(t){ return t*t*t }
function outCubic(t){ let u=t-1; return u*u*u+1 }
function inOutCubic(t){ return t<0.5 ? 4*t*t*t : (t-1)*(2*t-2)*(2*t-2)+1 }
function outInCubic(t){ return t<0.5 ? 0.5*outCubic(2*t) : 0.5+0.5*inCubic(2*t-1) }

// Quart
function inQuart(t){ return t*t*t*t }
function outQuart(t){ let u=t-1; return 1-u*u*u*u }
function inOutQuart(t){ let u=t-1; return t<0.5 ? 8*t*t*t*t : 1-8*u*u*u*u }
function outInQuart(t){ return t<0.5 ? 0.5*outQuart(2*t) : 0.5+0.5*inQuart(2*t-1) }

// Quint
function inQuint(t){ return t*t*t*t*t }
function outQuint(t){ let u=t-1; return 1+u*u*u*u*u }
function inOutQuint(t){ let u=t-1; return t<0.5 ? 16*t*t*t*t*t : 1+16*u*u*u*u*u }
function outInQuint(t){ return t<0.5 ? 0.5*outQuint(2*t) : 0.5+0.5*inQuint(2*t-1) }

// Sine
function inSine(t){ return 1-Math.cos(t*HALF_PI) }
function outSine(t){ return Math.sin(t*HALF_PI) }
function inOutSine(t){ return 0.5*(1-Math.cos(PI*t)) }
function outInSine(t){ return t<0.5 ? 0.5*outSine(2*t) : 0.5+0.5*inSine(2*t-1) }

// Expo
function inExpo(t){ return t===0 ? 0 : Math.pow(2, 10*(t-1)) }
function outExpo(t){ return t===1 ? 1 : 1-Math.pow(2, -10*t) }
function inOutExpo(t){ if(t===0||t===1) return t; return t<0.5 ? 0.5*Math.pow(2,20*t-10) : 1-0.5*Math.pow(2,-20*t+10) }
function outInExpo(t){ return t<0.5 ? 0.5*outExpo(2*t) : 0.5+0.5*inExpo(2*t-1) }

// Circ
function inCirc(t){ return 1-Math.sqrt(1-t*t) }
function outCirc(t){ let u=t-1; return Math.sqrt(1-u*u) }
function inOutCirc(t){ if(t<0.5){ return 0.5*(1-Math.sqrt(1-4*t*t)) } let u=2*t-2; return 0.5*(Math.sqrt(1-u*u)+1) }
function outInCirc(t){ return t<0.5 ? 0.5*outCirc(2*t) : 0.5+0.5*inCirc(2*t-1) }

// Elastic
function inElastic(t, a, p){
    if(t===0||t===1) return t
    let s; if(a < 1){ a=1; s=p/4 } else { s=p/(2*PI)*Math.asin(1/a) }
    t-=1; return -(a*Math.pow(2,10*t)*Math.sin((t-s)*(2*PI)/p))
}
function outElastic(t, a, p){
    if(t===0||t===1) return t
    let s; if(a < 1){ a=1; s=p/4 } else { s=p/(2*PI)*Math.asin(1/a) }
    return a*Math.pow(2,-10*t)*Math.sin((t-s)*(2*PI)/p)+1
}
function inOutElastic(t, a, p){
    if(t===0||t===1) return t
    t=t*2; let s; if(a < 1){ a=1; s=p/4 } else { s=p/(2*PI)*Math.asin(1/a) }
    if(t<1){ t-=1; return -0.5*(a*Math.pow(2,10*t)*Math.sin((t-s)*(2*PI)/p)) }
    t-=1; return a*Math.pow(2,-10*t)*Math.sin((t-s)*(2*PI)/p)*0.5+1
}
function outInElastic(t, a, p){ return t<0.5 ? 0.5*outElastic(2*t,a,p) : 0.5+0.5*inElastic(2*t-1,a,p) }

// Back
function inBack(t, o){ return t*t*((o+1)*t-o) }
function outBack(t, o){ let u=t-1; return u*u*((o+1)*u+o)+1 }
function inOutBack(t, o){ o*=1.525; if(t<0.5){ let u=2*t; return 0.5*(u*u*((o+1)*u-o)) } let u=2*t-2; return 0.5*(u*u*((o+1)*u+o)+2) }
function outInBack(t, o){ return t<0.5 ? 0.5*outBack(2*t,o) : 0.5+0.5*inBack(2*t-1,o) }

// Bounce
function outBounce(t, a){
    if(t===1) return 1
    if(t < (4/11)){
        return 7.5625*t*t
    } else if(t < (8/11)){
        t -= (6/11); return -a*(1-(7.5625*t*t+0.75))+1
    } else if(t < (10/11)){
        t -= (9/11); return -a*(1-(7.5625*t*t+0.9375))+1
    } else {
        t -= (21/22); return -a*(1-(7.5625*t*t+0.984375))+1
    }
}
function inBounce(t, a){ return 1-outBounce(1-t, a) }
function inOutBounce(t, a){ return t<0.5 ? 0.5*inBounce(2*t,a) : 0.5+0.5*outBounce(2*t-1,a) }
function outInBounce(t, a){ return t<0.5 ? 0.5*outBounce(2*t,a) : 0.5+0.5*inBounce(2*t-1,a) }

// Lookup table indexed by Easing type enum
const easingTable = [
    linear,        // 0 Linear
    inQuad,        // 1
    outQuad,       // 2
    inOutQuad,     // 3
    outInQuad,     // 4
    inCubic,       // 5
    outCubic,      // 6
    inOutCubic,    // 7
    outInCubic,    // 8
    inQuart,       // 9
    outQuart,      // 10
    inOutQuart,    // 11
    outInQuart,    // 12
    inQuint,       // 13
    outQuint,      // 14
    inOutQuint,    // 15
    outInQuint,    // 16
    inSine,        // 17
    outSine,       // 18
    inOutSine,     // 19
    outInSine,     // 20
    inExpo,        // 21
    outExpo,       // 22
    inOutExpo,     // 23
    outInExpo,     // 24
    inCirc,        // 25
    outCirc,       // 26
    inOutCirc,     // 27
    outInCirc,     // 28
    inElastic,     // 29
    outElastic,    // 30
    inOutElastic,  // 31
    outInElastic,  // 32
    inBack,        // 33
    outBack,       // 34
    inOutBack,     // 35
    outInBack,     // 36
    inBounce,      // 37
    outBounce,     // 38
    inOutBounce,   // 39
    outInBounce,   // 40
]

/**
 * Compute eased value for progress t in [0,1].
 * @param {number} t - linear progress 0..1
 * @param {{type:number, amplitude:number, overshoot:number, period:number}} easing
 * @returns {number} eased progress 0..1
 */
function $easingFunc(t, easing){
    if(!easing || easing.type === 0) return t
    const fn = easingTable[easing.type]
    if(!fn) return t
    // Elastic needs amplitude + period; Back needs overshoot; Bounce needs amplitude
    const type = easing.type
    if(type >= 29 && type <= 32){
        return fn(t, easing.amplitude, easing.period)
    } else if(type >= 33 && type <= 36){
        return fn(t, easing.overshoot)
    } else if(type >= 37 && type <= 40){
        return fn(t, easing.amplitude)
    }
    return fn(t)
}

module.exports.$easingFunc = $easingFunc
