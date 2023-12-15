const { Qt } = require('./Qt')

class MouseController {
    list = []
    flickList = []
    oldList = []
    originX = 0
    originY = 0

    pressedMouseAreaInner = null
    pressedMouseAreaOuter = null

    constructor(){
        window.onmousemove = (e)=>{
            // e.preventDefault()
            this.onMouseMove(e.pageX, e.pageY)
        }
        window.ondblclick = (e)=>{
            // e.preventDefault()
            this.onDoubleClick(e.pageX, e.pageY, e.button)
        }
        window.onmousedown = (e)=>{
            e.preventDefault()
            this.onMouseDown(e.pageX, e.pageY, e.button)
        }
        window.onmouseup = (e)=>{
            e.preventDefault()
            this.onMouseUp(e.pageX, e.pageY, e.button)
        }
        window.oncontextmenu = (e)=>{
            e.preventDefault()
            this.onMouseDown(e.pageX, e.pageY, e.button)
        }
        window.ontouchstart = (e)=>{
            e.preventDefault()
            this.onMouseDown(e.changedTouches[0].pageX, e.changedTouches[0].pageY)
        }
        window.ontouchend = (e)=>{
            e.preventDefault()
            this.onMouseUp(e.changedTouches[0].pageX, e.changedTouches[0].pageY)
        }
        window.ontouchmove = (e)=>{
            e.preventDefault()
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
        if(this.pressedMouseAreaInner === obj) this.pressedMouseAreaInner = null
        if(this.pressedMouseAreaOuter === obj) this.pressedMouseAreaOuter = null

        let index = this.list.indexOf(obj)
        if(index >= 0) this.list.splice(index, 1)

        index = this.oldList.indexOf(obj)
        if(index >= 0) this.oldList.splice(index, 1)

        index = this.flickList.indexOf(obj)
        if(index >= 0) this.flickList.splice(index, 1)
    }

    get(x, y){
        let elements = document.elementsFromPoint(x, y)
        let inner = []

        for(let element of elements){
            if(element.id && UIDList[element.id] && (UIDList[element.id].$mousearea || UIDList[element.id].$flickable || UIDList[element.id].$textinput) && UIDList[element.id].getPropertyValue('enabled') && UIDList[element.id].getPropertyValue('visible')){
                inner.push(UIDList[element.id])
            }
        }

        return inner

        // let inner = []
        // let outer = []
        // let pressedInner = []
        // let pressedOuter = []
        // for(let i = this.list.length - 1; i >= 0; i--){
        //     if(this.list[i].getPropertyValue('enabled') && this.list[i].getPropertyValue('visible')){
        //         let area = this.list[i].getDom().getBoundingClientRect()
        //         if(x >= area.left && x <= area.right && y >= area.top && y <= area.bottom){
        //             inner.push(this.list[i])
        //             if(this.pressedList.indexOf(this.list[i]) >= 0) pressedInner.push(this.list[i])
        //         } else {
        //             outer.push(this.list[i])
        //             if(this.pressedList.indexOf(this.list[i]) >= 0) pressedOuter.push(this.list[i])
        //         }
        //     }
            
        // }
        // return [inner, outer, pressedInner, pressedOuter]
    }

    stopPropogation(obj){
        this.stopPropogate = obj
    }

    onWheel(x, y, deltaX, deltaY){
        let inner = this.get(x, y)
        for(let obj of inner){
            if(!obj.onWheel(x, y, deltaX, deltaY)) break
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
        let inner = this.get(x, y)

        this.originX = x
        this.originY = y

        this.pressedMouseAreaInner = null
        this.pressedMouseAreaOuter = null

        this.flickList = []

        for(let i = 0; i < inner.length; i++){
            if(inner[i].$textinput && inner[i].onMouseDown(x, y, button)){
                break
            }

            if(inner[i].$mousearea && !this.pressedMouseAreaInner){
                if(inner[i].onMouseDown(x, y, button)){
                    for(let j = i+1; j < inner.length; j++){
                        if(!inner[j].onMouseDown(x, y, button)){
                            this.pressedMouseAreaInner = inner[j]
                            break
                        }
                    }
                } else {
                    this.pressedMouseAreaInner = inner[i]
                }
            }

            // if(inner[i].$textinput && !textinput){
            //     textinput = inner[i]
            // }

            if(inner[i].$flickable){
                inner[i].onMouseDown(x, y, button)
                this.flickList.push(inner[i])
            }

            
        }

        // if(textinput){
        //     textinput.onMouseDown(x, y, button)
        // }
        // console.log(this.pressedList)
    }
    onMouseUp(x, y, button){
        let inner = this.get(x, y)

        if(this.pressedMouseAreaInner){
            this.pressedMouseAreaInner.onMouseUp(x, y, button)
        }

        for(let obj of this.flickList){
            obj.onMouseUp(x, y, button)
        }

        this.pressedMouseAreaInner = null
        this.pressedMouseAreaOuter = null
        this.flickList = []
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
        let inner = this.get(x, y)

        document.body.style.cursor = 'default'

        for(let obj of this.oldList){
            if(obj.$mousearea && obj.getPropertyValue('hoverEnabled')) {
                obj.onMouseMove(x, y, false)

            }
        }

        for(let obj of inner){
            if(obj.$textinput){
                obj.onMouseMove(x, y)
                break
            }
        }
        
        this.oldList = inner


        let pressedMouseArea = this.pressedMouseAreaInner ? this.pressedMouseAreaInner : this.pressedMouseAreaOuter


        if(pressedMouseArea && !pressedMouseArea.getPropertyValue('preventStealing')){
            if(Math.abs(this.originX - x) >= 10 || Math.abs(this.originY - y) >= 10){
                pressedMouseArea = null
                this.pressedMouseAreaInner = null
                this.pressedMouseAreaOuter = null
            }
        }

        if(pressedMouseArea){
            if(pressedMouseArea.onMouseMove(x, y, true)){
                this.pressedMouseAreaInner = pressedMouseArea
                this.pressedMouseAreaOuter = null
            } else {
                this.pressedMouseAreaOuter = pressedMouseArea
                this.pressedMouseAreaInner = null
            }
        } else {
            if(this.flickList.length){
                while(this.flickList.length){
                    if(this.flickList[0].onMouseMove(x, y)) {
                        this.flickList.shift()
                    } else {
                        break
                    }
                }
            } else {
                for(let obj of inner){
                    if(obj.$mousearea && obj.getPropertyValue('hoverEnabled')) {
                        obj.onMouseMove(x, y, false)
        
                    }
                }
            }
            
        }
        
        
    }
}
class KeyboardController {
    constructor(){
        window.onkeydown = (e)=>{
            if(!(mainRoot.$focusedElement instanceof TextInput) && !(mainRoot.$focusedElement instanceof TextEdit)){
                
                if(e.key === 'Shift' || e.key === 'Control' || e.key === 'Alt') return
                let key = e.key
                if(e.key === 'ArrowLeft') key = 'Left'
                if(e.key === 'ArrowRight') key = 'Right'
                if(e.key === 'ArrowUp') key = 'Up'
                if(e.key === 'ArrowDown') key = 'Down'
                if(e.key === 'Return') key = 'Enter'


                // let currentShortcuts = []
                // for(let shortcut of this.shortcuts){
                //     if(shortcut.enabled){
                //         if(shortcut.sequence){
                //             let stateKeys = {
                //                 altKey: false,
                //                 ctrlKey: false,
                //                 shiftKey: false,
                //                 key: ''
                //             }
                //             let skeys = shortcut.sequence.split('+')
                //             for(let skey of skeys){
                //                 if(skey === 'Shift') {
                //                     stateKeys.shiftKey = true
                //                 } else if(skey === 'Alt') {
                //                     stateKeys.altKey = true
                //                 } else if(skey === 'Ctrl') {
                //                     stateKeys.ctrlKey = true
                //                 } else {
                //                     stateKeys.key = skey === 'Return' ? 'Enter' : skey
                //                 }
                //             }
                //             if(stateKeys.key === key && stateKeys.shiftKey === e.shiftKey && stateKeys.ctrlKey === e.ctrlKey && stateKeys.altKey === e.altKey) {
                //                 currentShortcuts.push(shortcut)
                //             } 
                //         } else if(shortcut.sequences.length) {
                //             let i = 0
                //             let find = false
                //             while(i < shortcut.sequences.length && !find){
                //                 let stateKeys = {
                //                     altKey: false,
                //                     ctrlKey: false,
                //                     shiftKey: false,
                //                     key: ''
                //                 }
                //                 let skeys = shortcut.sequences[i].split('+')
                //                 for(let skey of skeys){
                //                     if(skey === 'Shift') {
                //                         stateKeys.shiftKey = true
                //                     } else if(skey === 'Alt') {
                //                         stateKeys.altKey = true
                //                     } else if(skey === 'Ctrl') {
                //                         stateKeys.ctrlKey = true
                //                     } else {
                //                         stateKeys.key = skey === 'Return' ? 'Enter' : skey
                //                     }
                //                 }
                //                 if(stateKeys.key === key && stateKeys.shiftKey === e.shiftKey && stateKeys.ctrlKey === e.ctrlKey && stateKeys.altKey === e.altKey) {
                //                     currentShortcuts.push(shortcut)
                //                     find = true
                //                 }
                //                 i++
                //             }
                //         }
                //     }
                    
                // }
                // if(currentShortcuts.length >= 1){
                //     e.preventDefault()
                //     if(currentShortcuts.length === 1){
                //         currentShortcuts[0].activated()
                //     } else {
                //         currentShortcuts[currentShortcuts.length-1].activatedAmbiguously()
                //         Core.shortcuts.splice(Core.shortcuts.indexOf(currentShortcuts[currentShortcuts.length-1]), 1)
                //         Core.shortcuts.unshift(currentShortcuts[currentShortcuts.length-1])
                //     }
                //     return
                // }
                
            } else {
                if(e.key === 'Enter' || e.key === 'Return'){
                    e.preventDefault()
                    mainRoot.$focusedElement.onKeyDown(e.key)
                }
            }
            
            if(mainRoot.$focusedElement && mainRoot.$focusedElement.$properties.KeyNavigation){
                if(e.key === 'Shift' || e.key === 'Control' || e.key === 'Alt') return

                if(e.key === 'ArrowLeft') {
                    if(mainRoot.$focusedElement.KeyNavigation.left){
                        e.preventDefault()
                        mainRoot.$focusedElement.KeyNavigation.left.focus = true
                    }
                }
                if(e.key === 'ArrowRight') {
                    if(mainRoot.$focusedElement.KeyNavigation.right){
                        e.preventDefault()
                        mainRoot.$focusedElement.KeyNavigation.right.focus = true
                    }
                }
                if(e.key === 'ArrowUp') {
                    if(mainRoot.$focusedElement.KeyNavigation.up){
                        e.preventDefault()
                        mainRoot.$focusedElement.KeyNavigation.up.focus = true
                    }
                }
                if(e.key === 'ArrowDown') {
                    if(mainRoot.$focusedElement.KeyNavigation.down){
                        e.preventDefault()
                        mainRoot.$focusedElement.KeyNavigation.down.focus = true
                    }
                }
                if(e.key === 'Tab' && e.shiftKey) {
                    if(mainRoot.$focusedElement.KeyNavigation.backtab){
                        e.preventDefault()
                        mainRoot.$focusedElement.KeyNavigation.backtab.focus = true
                    }
                } else if(e.key === 'Tab') {
                    if(mainRoot.$focusedElement.KeyNavigation.tab){
                        e.preventDefault()
                        mainRoot.$focusedElement.KeyNavigation.tab.focus = true
                    }
                }
            }
        }
    }
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
