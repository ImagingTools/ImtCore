const { Qt } = require('./Qt')

class MouseController {
    list = []
    pressedList = []
    oldList = []
    originX = 0
    originY = 0
    viewMode = false
    // timestamp = 0

    constructor(){
        window.onmousemove = (e)=>{
            // e.preventDefault()
            this.onMouseMove(e.pageX, e.pageY)
        }
        window.ondblclick = (e)=>{
            // e.preventDefault()
            this.onDoubleClick(e.pageX, e.pageY)
        }
        window.onmousedown = (e)=>{
            // e.preventDefault()
            this.onMouseDown(e.pageX, e.pageY)
        }
        window.onmouseup = (e)=>{
            // e.preventDefault()
            this.onMouseUp(e.pageX, e.pageY)
        }
        window.ontouchstart = (e)=>{
            // e.preventDefault()
            this.onMouseDown(e.changedTouches[0].pageX, e.changedTouches[0].pageY)
        }
        window.ontouchend = (e)=>{
            // e.preventDefault()
            this.onMouseUp(e.changedTouches[0].pageX, e.changedTouches[0].pageY)
        }
        window.ontouchmove = (e)=>{
            // e.preventDefault()
            this.onMouseMove(e.changedTouches[0].pageX, e.changedTouches[0].pageY)
        }

        window.onwheel = (e)=>{
            // e.preventDefault()
            this.onWheel(e.pageX, e.pageY, e.deltaX, e.deltaY)
        }
    }

    add(obj){
        if(this.list.indexOf(obj) < 0) this.list.push(obj)
    }

    remove(obj){
        let index = this.list.indexOf(obj)
        if(index >= 0) this.list.splice(index, 1)

        index = this.oldList.indexOf(obj)
        if(index >= 0) this.oldList.splice(index, 1)
    }

    get(x, y){
        let inner = []
        let outer = []
        let pressedInner = []
        let pressedOuter = []
        for(let i = this.list.length - 1; i >= 0; i--){
            if(this.list[i].getStatement('enabled')){
                let area = this.list[i].getDom().getBoundingClientRect()
                if(x >= area.left && x <= area.right && y >= area.top && y <= area.bottom){
                    inner.push(this.list[i])
                    if(this.pressedList.indexOf(this.list[i]) >= 0) pressedInner.push(this.list[i])
                } else {
                    outer.push(this.list[i])
                    if(this.pressedList.indexOf(this.list[i]) >= 0) pressedOuter.push(this.list[i])
                }
            }
            
        }
        return [inner, outer, pressedInner, pressedOuter]
    }

    stopPropogation(obj){
        this.stopPropogate = obj
    }

    onWheel(x, y, deltaX, deltaY){
        let res = this.get(x, y)
        let inner = res[0]
        let outer = res[1]
        let pressedInner = res[2]
        let pressedOuter = res[3]

        this.stopPropogation(null)
        for(let obj of inner){
            if(!this.stopPropogate) obj.onWheel(x, y, deltaX, deltaY)
        }
    }

    onDoubleClick(x, y, button){
        let res = this.get(x, y)
        let inner = res[0]
        let outer = res[1]
        let pressedInner = res[2]
        let pressedOuter = res[3]

        this.stopPropogation(null)
        this.viewMode = false
        for(let obj of inner){
            if(!this.stopPropogate) {
                obj.onDoubleClick(x, y, button)
            }  
        }
    }

    onMouseDown(x, y, button){
        let res = this.get(x, y)
        let inner = res[0]
        let outer = res[1]
        let pressedInner = res[2]
        let pressedOuter = res[3]
        this.originX = x
        this.originY = y
        // this.timestamp = (new Date()).getTime()
        // console.log('MouseController',this.stopPropogate, this.viewMode, inner)
        this.stopPropogation(null)
        this.viewMode = false
        for(let obj of inner){
            this.pressedList.push(obj)
            if(!this.stopPropogate || obj.$flickable) {  
                obj.onMouseDown(x, y, button)
            }  
        }
        // console.log(res)
    }
    onMouseUp(x, y, button){
        let res = this.get(x, y)
        let inner = res[0]
        let outer = res[1]
        let pressedInner = res[2]
        let pressedOuter = res[3]

        this.pressedList = []
        // this.timestamp = (new Date()).getTime() - this.timestamp
        

        // this.stopPropogation(null)
        if(this.stopPropogate) {
            this.stopPropogate.onMouseUp(x, y, button)
        } else {
            for(let obj of pressedInner){
                if(!this.stopPropogate) {
                    obj.onMouseUp(x, y, button)
                }  
            }
        }
        this.stopPropogation(null)
        this.viewMode = false
        // console.log(res)
    }
    checkView(x, y, obj){
        if(obj.$flickable){
            if(Math.abs(this.originX - x) >= 10 || Math.abs(this.originY - y) >= 10) this.viewMode = true
            return true
        } else {
            return false
        }
    }
    onMouseMove(x, y){
        let res = this.get(x, y)
        let inner = res[0]
        let outer = res[1]
        let pressed = [].concat(res[2], res[3])
        // let pressedInner = res[2]
        // let pressedOuter = res[3]
        
        let isPressed = pressed.length ? true : false
        if(!this.stopPropogate) document.body.style.cursor = 'default'
        // console.log('MouseController',this.stopPropogate, this.viewMode, isPressed, inner, pressed, this.oldList)

        if(!isPressed){
            for(let obj of this.oldList){
                if(inner.indexOf(obj) < 0) {
                    if(obj.$flickable) continue
                    obj.onMouseMove(x, y)
                }
            }
            this.oldList = inner
        } else {
            this.oldList = []
        }

  
        for(let obj of isPressed ? pressed : inner){
            if(!this.stopPropogate) {
                if(!isPressed && obj.$flickable) continue
                if(this.checkView(x, y, obj)) {
                    if(!this.viewMode) continue
                }
                obj.onMouseMove(x, y)
            }
            
            if(isPressed && this.checkView(x, y, obj)){
                if(this.stopPropogate) {
                    if(this.viewMode && this.stopPropogate.$mousearea && !this.stopPropogate.getPropertyValue('preventStealing')){
                        this.stopPropogation(obj)
                        obj.onMouseMove(x, y)
                        // break
                    }
                } else {
                    if(this.viewMode){
                        this.stopPropogation(obj)
                        obj.onMouseMove(x, y)
                        // break
                    }
                    
                }
            }
            
        }

        if(this.stopPropogate) {
            this.stopPropogate.onMouseMove(x, y)
        }
        

        
        
    }
}
class KeyboardController {

}
class TextFontController {
    constructor(){
        let canvas = document.createElement('canvas')
        this.ctx = canvas.getContext('2d')
        // this.parser = new DOMParser()
    }

    measureText(text, pixelSize, fontFamily, maxWidth, wordWrap){
        this.ctx.font = `${pixelSize}px ${fontFamily ? fontFamily : 'auto'}`
        
        if(wordWrap && maxWidth){
            let resWidth = 0
            let resHeight = 0
            // let nodes = this.parser.parseFromString(text, 'text/html').body.childNodes
            let words = wordWrap && wordWrap === 1 ? text.split(" ") : text
            let lines = []
            let currentLine = words[0]
        
            for (let i = 1; i < words.length; i++) {
                let word = words[i]
                let width = this.ctx.measureText(currentLine + " " + word).width
                if (width < maxWidth) {
                    currentLine += " " + word
                } else {
                    lines.push(currentLine)
                    let metrics = this.ctx.measureText(currentLine)
                    resWidth = Math.max(resWidth, metrics.width)
                    resHeight += metrics.fontBoundingBoxAscent + metrics.fontBoundingBoxDescent
                    currentLine = word
                }
            }
            let metrics = this.ctx.measureText(currentLine)
            resWidth = Math.max(resWidth, metrics.width)
            resHeight += metrics.fontBoundingBoxAscent + metrics.fontBoundingBoxDescent
            lines.push(currentLine)
            return {
                text: lines.join('\n'),
                width: resWidth,
                height: resHeight
            }
        } else {
            let metrics = this.ctx.measureText(text)
            return {
                text: text,
                width: metrics.width,
                height: metrics.fontBoundingBoxAscent + metrics.fontBoundingBoxDescent
            }
        }
        
    }
}
class AnimationController {
    animations = []

    update(){
        if(this.animations.length){
            if(!this.timer) {
                this.timer = setInterval(()=>{
                    for(let animation of this.animations){
                        animation.tick()
                    }
                }, 1000 / 60)
            }
        } else {
            if(this.timer) {
                clearInterval(this.timer)
                delete this.timer
            }
        }
    }

    add(obj){
        if(this.animations.indexOf(obj) < 0) this.animations.push(obj)
        this.update()
    }

    remove(obj){
        let index = this.animations.indexOf(obj)
        if(index >= 0) this.animations.splice(index, 1)
        this.update()
    }
}


module.exports.MouseController = MouseController
module.exports.KeyboardController = KeyboardController
module.exports.TextFontController = TextFontController
module.exports.AnimationController = AnimationController
