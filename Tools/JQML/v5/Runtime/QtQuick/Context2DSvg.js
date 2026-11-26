// SVG path data as a QPainterPath (QGuiSvg::parsePath of Qt 6.12, qsvgpathparser): M, L, H, V, C, S, Q, T, A, Z
// absolute and relative; the arcs as cubic curves (pathArc). Invalid data: null.

function tokenize(d){
    let tokens = []
    let re = /([MmZzLlHhVvCcSsQqTtAa])|([+-]?(?:\d+\.?\d*|\.\d+)(?:[eE][+-]?\d+)?)|([\s,]+)|(.)/g
    let m
    while((m = re.exec(d))){
        if(m[1]) tokens.push(m[1])
        else if(m[2]) tokens.push(Number(m[2]))
        else if(m[4]) return null
    }
    return tokens
}

// pathArcSegment / pathArc: an SVG arc as cubic curves
function arcSegments(path, x0, y0, rx, ry, angle, largeArc, sweep, x, y){
    if(rx === 0 || ry === 0){
        path.lineTo(x, y)
        return
    }
    rx = Math.abs(rx)
    ry = Math.abs(ry)
    let sinTh = Math.sin(angle * Math.PI / 180), cosTh = Math.cos(angle * Math.PI / 180)
    let a00 = cosTh / rx, a01 = sinTh / rx, a10 = -sinTh / ry, a11 = cosTh / ry
    let px1 = a00 * x0 + a01 * y0, py1 = a10 * x0 + a11 * y0
    let px3 = a00 * x + a01 * y, py3 = a10 * x + a11 * y
    let d = (px3 - px1) * (px3 - px1) + (py3 - py1) * (py3 - py1)
    let sfactorSq = 1 / d - 0.25
    if(sfactorSq < 0) sfactorSq = 0
    let sfactor = Math.sqrt(sfactorSq)
    if(sweep === largeArc) sfactor = -sfactor
    let xc = 0.5 * (px1 + px3) - sfactor * (py3 - py1)
    let yc = 0.5 * (py1 + py3) + sfactor * (px3 - px1)
    let th0 = Math.atan2(py1 - yc, px1 - xc)
    let th1 = Math.atan2(py3 - yc, px3 - xc)
    let thArc = th1 - th0
    if(thArc < 0 && sweep) thArc += 2 * Math.PI
    else if(thArc > 0 && !sweep) thArc -= 2 * Math.PI
    let segments = Math.ceil(Math.abs(thArc / (Math.PI * 0.5 + 0.001)))
    for(let i = 0; i < segments; i++){
        let s0 = th0 + i * thArc / segments, s1 = th0 + (i + 1) * thArc / segments
        let b00 = cosTh * rx, b01 = -sinTh * ry, b10 = sinTh * rx, b11 = cosTh * ry
        let thHalf = 0.5 * (s1 - s0)
        let t = (8 / 3) * Math.sin(thHalf * 0.5) * Math.sin(thHalf * 0.5) / Math.sin(thHalf)
        let x1 = xc + Math.cos(s0) - t * Math.sin(s0), y1 = yc + Math.sin(s0) + t * Math.cos(s0)
        let x3 = xc + Math.cos(s1), y3 = yc + Math.sin(s1)
        let x2 = x3 + t * Math.sin(s1), y2 = y3 - t * Math.cos(s1)
        path.cubicTo(b00 * x1 + b01 * y1, b10 * x1 + b11 * y1, b00 * x2 + b01 * y2, b10 * x2 + b11 * y2, b00 * x3 + b01 * y3, b10 * x3 + b11 * y3)
    }
}

function parseSvgPath(d){
    const { PainterPath } = require('./Context2D')
    let tokens = tokenize(String(d))
    if(!tokens) return null
    let path = new PainterPath()
    let i = 0
    let cmd = null
    let x = 0, y = 0, startX = 0, startY = 0
    let ctrlX = 0, ctrlY = 0, lastCmd = null
    let numbers = (n) => {
        let out = []
        for(let k = 0; k < n; k++){
            if(typeof tokens[i] !== 'number') return null
            out.push(tokens[i++])
        }
        return out
    }
    while(i < tokens.length){
        if(typeof tokens[i] === 'string') cmd = tokens[i++]
        else if(cmd === null) return null
        let rel = cmd === cmd.toLowerCase()
        let c = cmd.toUpperCase()
        if(c === 'Z'){
            path.closeSubpath()
            x = startX
            y = startY
            lastCmd = 'Z'
            continue
        }
        let ox = rel ? x : 0, oy = rel ? y : 0
        let v
        switch(c){
            case 'M':
                if(!(v = numbers(2))) return null
                x = ox + v[0]; y = oy + v[1]
                path.moveTo(x, y)
                startX = x; startY = y
                // the next numbers are lines
                cmd = rel ? 'l' : 'L'
                break
            case 'L':
                if(!(v = numbers(2))) return null
                x = ox + v[0]; y = oy + v[1]
                path.lineTo(x, y)
                break
            case 'H':
                if(!(v = numbers(1))) return null
                x = ox + v[0]
                path.lineTo(x, y)
                break
            case 'V':
                if(!(v = numbers(1))) return null
                y = oy + v[0]
                path.lineTo(x, y)
                break
            case 'C':
                if(!(v = numbers(6))) return null
                path.cubicTo(ox + v[0], oy + v[1], ox + v[2], oy + v[3], ox + v[4], oy + v[5])
                ctrlX = ox + v[2]; ctrlY = oy + v[3]
                x = ox + v[4]; y = oy + v[5]
                break
            case 'S': {
                if(!(v = numbers(4))) return null
                let c1x = x, c1y = y
                if(lastCmd === 'C' || lastCmd === 'S'){ c1x = 2 * x - ctrlX; c1y = 2 * y - ctrlY }
                path.cubicTo(c1x, c1y, ox + v[0], oy + v[1], ox + v[2], oy + v[3])
                ctrlX = ox + v[0]; ctrlY = oy + v[1]
                x = ox + v[2]; y = oy + v[3]
                break
            }
            case 'Q':
                if(!(v = numbers(4))) return null
                path.quadTo(ox + v[0], oy + v[1], ox + v[2], oy + v[3])
                ctrlX = ox + v[0]; ctrlY = oy + v[1]
                x = ox + v[2]; y = oy + v[3]
                break
            case 'T': {
                if(!(v = numbers(2))) return null
                let cx = x, cy = y
                if(lastCmd === 'Q' || lastCmd === 'T'){ cx = 2 * x - ctrlX; cy = 2 * y - ctrlY }
                path.quadTo(cx, cy, ox + v[0], oy + v[1])
                ctrlX = cx; ctrlY = cy
                x = ox + v[0]; y = oy + v[1]
                break
            }
            case 'A': {
                if(!(v = numbers(7))) return null
                let ex = ox + v[5], ey = oy + v[6]
                arcSegments(path, x, y, v[0], v[1], v[2], !!v[3], !!v[4], ex, ey)
                x = ex; y = ey
                break
            }
            default:
                return null
        }
        lastCmd = c
    }
    return path
}

module.exports = { parseSvgPath }
