const PropertyAnimation = require("./PropertyAnimation")
const Animation = require("./Animation")

function parseColor(c){
    if(c === undefined || c === null) return [0,0,0,1]
    if(typeof c === 'object' && 'r' in c) return [(c.r||0)*255, (c.g||0)*255, (c.b||0)*255, c.a == null ? 1 : c.a]
    if(typeof c !== 'string') return [0,0,0,1]
    let s = c.trim()
    if(s[0] === '#'){
        s = s.slice(1)
        if(s.length === 3) s = s.split('').map(x => x+x).join('')
        if(s.length === 4) s = s.split('').map(x => x+x).join('')
        const n = parseInt(s, 16)
        if(s.length === 6) return [(n>>16)&255, (n>>8)&255, n&255, 1]
        if(s.length === 8) return [(n>>>24)&255, (n>>16)&255, (n>>8)&255, (n&255)/255]
    }
    const m = s.match(/rgba?\(([^)]+)\)/i)
    if(m){
        const p = m[1].split(',').map(x => parseFloat(x))
        return [p[0]||0, p[1]||0, p[2]||0, p[3] == null ? 1 : p[3]]
    }
    return [0,0,0,1]
}

function lerp(a, b, t){ return a + (b-a)*t }
function formatColor(c){
    return `rgba(${Math.round(c[0])}, ${Math.round(c[1])}, ${Math.round(c[2])}, ${c[3]})`
}

class ColorAnimation extends PropertyAnimation {

    __elapsed = 0

    restart(){
        this.__elapsed = 0
        super.restart()
    }

    __tick(){
        if(!this.target || !this.running || this.paused) return

        const propsStr = this.properties || this.property
        if(!propsStr) return
        const props = propsStr.split(',').map(s => s.trim()).filter(Boolean)
        const duration = Math.max(1, this.duration)
        this.__elapsed += 1000/60
        const progress = Math.min(1, this.__elapsed / duration)

        for(const prop of props){
            let from = this.from
            let to = this.to
            if(from === undefined) from = this.target[prop]
            if(to === undefined) to = this.target[prop]
            const f = parseColor(from), t = parseColor(to)
            const cur = [lerp(f[0],t[0],progress), lerp(f[1],t[1],progress), lerp(f[2],t[2],progress), lerp(f[3],t[3],progress)]
            try { this.target[prop] = formatColor(cur) } catch(e) {}
        }

        if(progress >= 1){
            this.__elapsed = 0
            const loops = this.__loopCounter + 1
            if(loops < this.loops || this.loops === Animation.Infinite){
                this.__loopCounter = loops
            } else {
                this.running = false
                this.stopped()
                this.finished()
            }
        }
    }
}

module.exports = ColorAnimation
