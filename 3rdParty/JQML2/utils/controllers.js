const { Qt } = require('./Qt')

class MouseController {
    list = []
    originX = 0
    originY = 0
    moveX = 0
    moveY = 0
    pressedMouseArea = []
    pressed = []
    oldList = []
    timestamp = 0

    constructor(){
        window.addEventListener('mousemove', (e)=>{
            // e.stopPropagation()
            this.onMouseMove(e.pageX, e.pageY, e)
        })
        
        window.addEventListener('dblclick', (e)=>{
            // e.stopPropagation()
            e.preventDefault()
        })
        window.addEventListener('mousedown', (e)=>{
            // e.stopPropagation()
            this.onMouseDown(e.pageX, e.pageY, e.button, e)
        })
        window.addEventListener('mouseup', (e)=>{
            // e.stopPropagation()
            this.onMouseUp(e.pageX, e.pageY, e.button, e)
        })
        window.addEventListener('contextmenu', (e)=>{
            e.preventDefault()
            // e.stopPropagation()
            this.onMouseDown(e.pageX, e.pageY, e.button, e)
        })
        window.addEventListener('touchstart', (e)=>{
            // e.preventDefault()
            // e.stopPropagation()
            this.onMouseDown(e.changedTouches[0].pageX, e.changedTouches[0].pageY, 0, e)
        })
        window.addEventListener('touchend', (e)=>{
            // e.preventDefault()
            // e.stopPropagation()
            this.onMouseUp(e.changedTouches[0].pageX, e.changedTouches[0].pageY, 0, e)
        })
        window.addEventListener('touchmove', (e)=>{
            // e.preventDefault()
            // e.stopPropagation()
            this.onMouseMove(e.changedTouches[0].pageX, e.changedTouches[0].pageY, e)
        })
        window.addEventListener('wheel', (e)=>{
            // e.stopPropagation()
            this.onWheel(e.pageX, e.pageY, e.deltaX, e.deltaY)
        })
    }

    add(obj){
        if(this.list.indexOf(obj) < 0) this.list.push(obj)
    }

    remove(obj){
        let index = this.list.indexOf(obj)
        if(index >= 0) this.list.splice(index, 1)
    }

    get(x, y){
        for(let obj of this.list){
            if(obj instanceof MouseArea || obj instanceof Flickable){
                obj.setStyle({
                    pointerEvents: 'auto',
                    touchAction: 'auto',
                })
            }
        }
        let elements = document.elementsFromPoint(x, y)
        let inner = []

        for(let element of elements){
            if(element.id && UIDList[element.id] && (UIDList[element.id] instanceof MouseArea || UIDList[element.id] instanceof TextEdit || UIDList[element.id] instanceof TextInput || UIDList[element.id] instanceof Map || (UIDList[element.id] instanceof Flickable && UIDList[element.id].getPropertyValue('interactive'))) && UIDList[element.id].getPropertyValue('enabled') && UIDList[element.id].getPropertyValue('visible')){
                inner.push(UIDList[element.id])
            }
        }

        for(let obj of this.list){
            if(obj instanceof MouseArea || obj instanceof Flickable){
                obj.setStyle({
                    pointerEvents: 'none',
                    touchAction: 'none',
                })
            }
        }

        // for(let i = this.list.length-1; i >= 0; i--){
        //     if(this.list[i].getPropertyValue('enabled') && this.list[i].getPropertyValue('visible')){
        //         let rect = this.list[i].getDom().getBoundingClientRect()
        //         if(rect.left < x && rect.top < y && rect.right >= x && rect.bottom >= y) inner.push(this.list[i])
        //     }
        // }

        return inner
    }

    checkView(x, y, obj){
        if(obj instanceof Flickable){
            if(Math.abs(this.originX - x) >= 10 || Math.abs(this.originY - y) >= 10) this.viewMode = true
            return true
        } else {
            return false
        }
    }

    onWheel(x, y, deltaX, deltaY){
        let inner = this.get(x, y)
        for(let obj of inner){
            if(!obj.onWheel(x, y, deltaX, deltaY)) break
        }
    }

    onMouseDown(x, y, button, e){
        this.target = null
        this.dblClicked = false
        let timestamp = (new Date()).getTime()
        if(timestamp - this.timestamp < 300) this.dblClicked = true
        this.timestamp = timestamp
        this.originX = x
        this.originY = y
        this.moveX = x
        this.moveY = y
        this.pressedMouseArea = []
        let inner = this.get(x, y)
        // let testInner = mainRoot.$getForMouseEvent(x, y)
        this.pressed = inner
        let view = null


        for(let i = 0; i < this.oldList.length; i++){
            if(this.oldList[i] instanceof MouseArea){
                this.pressed[i].getProperty('pressed').value = false
            }
        }

        this.oldList = []
        
        if(inner[0] && inner[0] instanceof TextInput || inner[0] instanceof TextEdit || inner[0] instanceof Map){
            if(inner[0].getPropertyValue('enabled') && inner[0].getPropertyValue('visible')) {
                this.target = inner[0]
                return
            }
        } 
        // for(let i = 1; i < inner.length; i++){
        //     if(inner[i] instanceof TextInput || inner[i] instanceof TextEdit){
        //         if(inner[i].getPropertyValue('enabled') && inner[i].getPropertyValue('visible') && inner[i].getPropertyValue('activeFocus')) {
        //             this.target = inner[i]
        //             return
        //         }
        //     }
        // }
        for(let i = 0; i < inner.length; i++){
            if(inner[i] instanceof TextInput || inner[i] instanceof TextEdit){
                if(inner[i].getPropertyValue('enabled') && inner[i].getPropertyValue('visible') && inner[i].getPropertyValue('activeFocus')) {
                    this.target = inner[i]
                    return
                }
            }
            
            if(!view && inner[i] instanceof Flickable && inner[i].getPropertyValue('enabled') && inner[i].getPropertyValue('visible') && inner[i].getPropertyValue('interactive')){
                this.target = inner[i]
                break
            }
            
            if(inner[i] instanceof MouseArea){
                if(inner[i].getPropertyValue('enabled') && inner[i].getPropertyValue('visible') && inner[i].availableButton(button)){
                    
                    let rect = inner[i].getDom().getBoundingClientRect()
                    let norm = inner[i].normalizeXY(x - rect.x, y - rect.y)
                    inner[i].mouse.x = norm.x
                    inner[i].mouse.y = norm.y
                    
                    inner[i].getStatement('mouseX').reset(norm.x)
                    inner[i].getStatement('mouseY').reset(norm.y)

                    if(inner[i].$signals.entered) {
                        inner[i].$signals.entered()
                    }
        
                    inner[i].mouse.accepted = false
                    inner[i].getProperty('pressed').value = true
                    if(inner[i].getProperty('pressed').notify) {
                        // this.target = inner[i]
                        inner[i].mouse.accepted = true
                        inner[i].getProperty('pressed').notify()
                    } else {
                        inner[i].mouse.accepted = true
                    }

                    

                    if(inner[i].mouse.accepted) {
                        this.target = inner[i]
                        this.pressedMouseArea = [inner[i]]
                        for(let k = i + 1; k < inner.length; k++){
                            if(inner[k] instanceof TextInput || inner[k] instanceof TextEdit){
                                if(inner[k].getPropertyValue('enabled') && inner[k].getPropertyValue('visible') && inner[k].getPropertyValue('activeFocus')) {
                                    this.target = inner[k]
                                    break
                                }
                            }
                            if(inner[k] instanceof MouseArea && inner[k].getPropertyValue('enabled') && inner[k].getPropertyValue('visible') && inner[k].availableButton(button))
                            this.pressedMouseArea.push(inner[k])
                        }
                        break
                    }
                    
                }
            }
        }

        // if(!this.target){
        //     this.target = view
        // }

        if(!this.target || !(this.target instanceof TextInput || this.target instanceof TextInput || this.target instanceof Map)){
            e.preventDefault()
        }
    }
    onMouseUp(x, y, button, e){
        for(let i = 0; i < this.pressed.length; i++){
            if(this.pressed[i] instanceof MouseArea) this.pressed[i].getProperty('pressed').value = false
        }

        if(this.target){
            if(this.target instanceof MouseArea){
                e.preventDefault()
                this.target.getProperty('pressed').value = false
                let rect = this.target.getDom().getBoundingClientRect()
                let norm = this.target.normalizeXY(x - rect.x, y - rect.y)
                this.target.mouse.x = norm.x
                this.target.mouse.y = norm.y
                this.target.getStatement('mouseX').reset(norm.x)
                this.target.getStatement('mouseY').reset(norm.y)
                
        
                if(this.target.$signals.released) {
                    this.target.$signals.released()
                }

                let wasDblClicked = false

                if(this.dblClicked){
                    this.timestamp = 0
                    this.target.mouse.accepted = false
                    if(this.target.$signals.doubleClicked) {
                        this.target.$signals.doubleClicked()
                        wasDblClicked = true
                    }
                    if(this.target.getPropertyValue('propagateComposedEvents') && !this.target.mouse.accepted)
                    for(let i = 1; i < this.pressedMouseArea.length; i++){
                        if(this.pressedMouseArea[i].$signals.doubleClicked){
                            this.pressedMouseArea[i].mouse.accepted = false
                            this.pressedMouseArea[i].$signals.doubleClicked()
                            wasDblClicked = true
                            if(!this.pressedMouseArea[i].getPropertyValue('propagateComposedEvents') || this.pressedMouseArea[i].mouse.accepted) break
                        }
                    }
                }

                if(!wasDblClicked){
                    this.target.mouse.accepted = true
                    if(this.target.$signals.clicked) {
                        this.target.$signals.clicked()
                        if(this.target.getPropertyValue('propagateComposedEvents') && !this.target.mouse.accepted)
                        for(let i = 1; i < this.pressedMouseArea.length; i++){
                            if(this.pressedMouseArea[i].$signals.clicked){
                                this.pressedMouseArea[i].mouse.accepted = true
                                this.pressedMouseArea[i].$signals.clicked()
                                if(!this.pressedMouseArea[i].getPropertyValue('propagateComposedEvents') || this.pressedMouseArea[i].mouse.accepted) break
                            }
                        }
                    }
                }
            
                

                if(this.target.$signals.exited) {
                    this.target.$signals.exited()
                }
                
                delete this.target.$entered
            }
        } else {
            let wasDblClicked = false
            if(this.dblClicked){
                this.timestamp = 0
                for(let i = 0; i < this.pressed.length; i++){
                    if(this.pressed[i] instanceof TextInput || this.pressed[i] instanceof TextEdit){
                        break
                    } else if(this.pressed[i] instanceof MouseArea && this.pressed[i].availableButton(button)){
                        if(this.pressed[i].$signals.doubleClicked){
                            this.pressed[i].mouse.accepted = true
                            this.pressed[i].$signals.doubleClicked()
                            wasDblClicked = true
                            if(!this.pressed[i].getPropertyValue('propagateComposedEvents') || this.pressed[i].mouse.accepted) break
                        }
                    }
                }
            }
            if(!wasDblClicked){
                for(let i = 0; i < this.pressed.length; i++){
                    if(this.pressed[i] instanceof TextInput || this.pressed[i] instanceof TextEdit){
                        break
                    } else if(this.pressed[i] instanceof MouseArea && this.pressed[i].availableButton(button)){
                        if(this.pressed[i].$signals.clicked){
                            this.pressed[i].mouse.accepted = true
                            this.pressed[i].$signals.clicked()
                            if(!this.pressed[i].getPropertyValue('propagateComposedEvents') || this.pressed[i].mouse.accepted) break
                        }
                    }
                }
            }
            
        }
        
        this.pressed = []
        this.pressedMouseArea = []
        this.target = null
    }
    onMouseMove(x, y, e){
        let dx = this.moveX - x
        let dy = this.moveY - y
        this.moveX = x
        this.moveY = y

        let inner = this.get(x, y)

        

        if(this.target){
            if(this.target instanceof Map){
                return
            }
            if(this.target instanceof MouseArea){
                let rect = this.target.getDom().getBoundingClientRect()
                let norm = this.target.normalizeXY(x - rect.x, y - rect.y)
                this.target.mouse.x = norm.x
                this.target.mouse.y = norm.y
                this.target.getStatement('mouseX').reset(norm.x)
                this.target.getStatement('mouseY').reset(norm.y)
    
                document.body.style.cursor = this.target.getPropertyValue('cursorShape')
    
                if((this.target.getPropertyValue('pressed') || this.target.getPropertyValue('hoverEnabled')) && this.target.$signals.positionChanged) this.target.$signals.positionChanged()
    
                if(inner.indexOf(this.target) >= 0){
                    this.target.getProperty('containsMouse').reset(true)
                } else {
                    this.target.getProperty('containsMouse').reset(false)
                }
                
                if(this.target.$signals && this.target.$signals.entered && !this.target.$entered) {
                    this.target.$signals.entered()
                }
                this.target.$entered = true
                
                    
                if(!this.target.getPropertyValue('preventStealing')){
                    if(Math.abs(this.originX - x) >= 10 || Math.abs(this.originY - y) >= 10){
                        for(let i = 0; i < this.pressed.length; i++){
                            if(this.pressed[i] instanceof Flickable && this.pressed[i].getPropertyValue('enabled') && this.pressed[i].getPropertyValue('visible') && this.pressed[i].getPropertyValue('interactive')) {
                                if((Math.abs(this.originX - x) >= 10 && this.pressed[i].getPropertyValue('width') < this.pressed[i].getPropertyValue('contentItem').getPropertyValue('width')) || 
                                (Math.abs(this.originY - y) >= 10 && this.pressed[i].getPropertyValue('height') < this.pressed[i].getPropertyValue('contentItem').getPropertyValue('height'))){
                                    this.target.getProperty('pressed').value = false
                                    this.target = this.pressed[i]
                                    break
                                }
                                
                            }
                        }
                    } else {
                        return
                    }
                } else {
                    return
                }
            }
        } else {
            e.preventDefault()

            document.body.style.cursor = 'default'
            let contains = false
            
            for(let i = this.list.length-1; i >= 0; i--){
                if(this.list[i].UID && this.list[i] instanceof MouseArea && this.list[i].getPropertyValue('hoverEnabled')){
                    let rect = this.list[i].getDom().getBoundingClientRect()
                    let norm = this.list[i].normalizeXY(x - rect.x, y - rect.y)
                    this.list[i].mouse.x = norm.x
                    this.list[i].mouse.y = norm.y
                    this.list[i].getStatement('mouseX').reset(norm.x)
                    this.list[i].getStatement('mouseY').reset(norm.y)
    
                    if(!contains && inner.indexOf(this.list[i]) >= 0){
                        contains = true
                        if((this.list[i].getPropertyValue('pressed') || this.list[i].getPropertyValue('hoverEnabled')) && this.list[i].$signals.positionChanged) this.list[i].$signals.positionChanged()
                        
                        this.list[i].getProperty('containsMouse').reset(true)
                        if(this.list[i].$signals && this.list[i].$signals.entered && !this.list[i].$entered) {
                            this.list[i].$signals.entered()
                        }
                        this.list[i].$entered = true
                    } else {
                        this.list[i].getProperty('containsMouse').reset(false) 
                        if(this.list[i].$signals && this.list[i].$signals.exited && this.list[i].$entered) {  
                            this.list[i].$signals.exited()
                        }
                        delete this.list[i].$entered
                    }
                    
                }
            }

            for(let i = 0; i < inner.length; i++){
                if(inner[i] instanceof TextInput || inner[i] instanceof TextEdit){
                    document.body.style.cursor = 'text'
                    break
                } else if(inner[i] instanceof MouseArea && inner[i].$properties.cursorShape && inner[i].getPropertyValue('hoverEnabled')){
                    document.body.style.cursor = inner[i].getPropertyValue('cursorShape')
                    break
                }
            }
        }
        

        if(this.target && this.target instanceof Flickable){
            if(this.target instanceof ListView){
                if(this.target.getPropertyValue('enabled') && this.target.getPropertyValue('visible') && this.target.getPropertyValue('interactive')) {
                    if(this.target.getPropertyValue('orientation') === ListView.Vertical){
                        if(this.target.getPropertyValue('contentHeight') <= this.target.getPropertyValue('height')){
                            this.target.getStatement('contentY').reset(0)
            
                        } else if(this.target.getPropertyValue('contentY') + (dy) > this.target.getPropertyValue('originY') && this.target.getPropertyValue('contentY') + (dy) < this.target.getPropertyValue('contentHeight') + this.target.getPropertyValue('originY') - this.target.getPropertyValue('height')){
                            this.target.getStatement('contentY').reset(this.target.getPropertyValue('contentY') + (dy))
                            return
                        } else {
                            if(this.target.getPropertyValue('contentY') + (dy) <= this.target.getPropertyValue('originY')) {
                                this.target.getStatement('contentY').reset(this.target.getPropertyValue('originY'))
                                // MouseController.stopPropogation(null)
                            }
                            if(this.target.getPropertyValue('contentY') + (dy) >= this.target.getPropertyValue('contentHeight') + this.target.getPropertyValue('originY') - this.target.getPropertyValue('height')) {
                                this.target.getStatement('contentY').reset(this.target.getPropertyValue('contentHeight') + this.target.getPropertyValue('originY') - this.target.getPropertyValue('height'))
                                // MouseController.stopPropogation(null)
                            }
            
                        }
                    } else {
                        if(this.target.getPropertyValue('contentWidth') <= this.target.getPropertyValue('width')){
                            this.target.getStatement('contentX').reset(0)
            
                        } else if(this.target.getPropertyValue('contentX') + (dx) > this.target.getPropertyValue('originX') && this.target.getPropertyValue('contentX') + (dx) < this.target.getPropertyValue('contentWidth') + this.target.getPropertyValue('originX') - this.target.getPropertyValue('width')){
                            this.target.getStatement('contentX').reset(this.target.getPropertyValue('contentX') + (dx))
                            return
                        } else {
                            if(this.target.getPropertyValue('contentX') + (dx) <= this.target.getPropertyValue('originX')) {
                                this.target.getStatement('contentX').reset(this.target.getPropertyValue('originX'))
                                // MouseController.stopPropogation(null)
                            }
                            if(this.target.getPropertyValue('contentX') + (dx) >= this.target.getPropertyValue('contentWidth') + this.target.getPropertyValue('originX') - this.target.getPropertyValue('width')) {
                                this.target.getStatement('contentX').reset(this.target.getPropertyValue('contentWidth') + this.target.getPropertyValue('originX') - this.target.getPropertyValue('width'))
                                // MouseController.stopPropogation(null)
                            }
            
                        }
                    }
                    
                }
            } else if(this.target instanceof Flickable){
                if(this.target.getPropertyValue('enabled') && this.target.getPropertyValue('visible') && this.target.getPropertyValue('interactive')) {
                    if(this.target.getPropertyValue('contentX') + (dx) > 0 && this.target.getPropertyValue('contentX') + (dx) < this.target.getPropertyValue('contentItem').getPropertyValue('width') - this.target.getPropertyValue('width')){
                        this.target.getStatement('contentX').reset(this.target.getPropertyValue('contentX') + (dx))
                    } else {
                        if(this.target.getPropertyValue('contentX') + (dx) <= 0) this.target.getStatement('contentX').reset(0)
                        if(this.target.getPropertyValue('contentX') + (dx) >= this.target.getPropertyValue('contentItem').getPropertyValue('width')) this.target.getStatement('contentX').reset(this.target.getPropertyValue('contentItem').getPropertyValue('width') - this.target.getPropertyValue('width'))
                    }
        
                    if(this.target.getPropertyValue('contentY') + (dy) > 0 && this.target.getPropertyValue('contentY') + (dy) < this.target.getPropertyValue('contentItem').getPropertyValue('height') - this.target.getPropertyValue('height')){
                        this.target.getStatement('contentY').reset(this.target.getPropertyValue('contentY') + (dy))
                    } else {
                        if(this.target.getPropertyValue('contentY') + (dy) <= 0) this.target.getStatement('contentY').reset(0)
                        if(this.target.getPropertyValue('contentY') + (dy) >= this.target.getPropertyValue('contentItem').getPropertyValue('height')) this.target.getStatement('contentY').reset(this.target.getPropertyValue('contentItem').getPropertyValue('height') - this.target.getPropertyValue('height'))
                    }
                }
            }
        }
    }

    // onMouseMove(x, y){
    //     let ax = this.moveX - x
    //     let ay = this.moveY - y
    //     this.moveX = x
    //     this.moveY = y
    //     let inner = this.get(x, y)

    //     document.body.style.cursor = 'default'

    //     // console.log('MouseController::: oldList =',this.oldList)
    //     while(this.oldList.length){
    //         let obj = this.oldList.shift()
    //         if(obj.$mousearea && obj.getPropertyValue('hoverEnabled')) {
    //             // let normXY = this.normalizeXY(x, y, obj)
    //             obj.onMouseMove(x, y, false)

    //         }
    //     }
    //     // for(let obj of this.oldList){
    //     //     if(obj.$mousearea && obj.getPropertyValue('hoverEnabled')) {
    //     //         obj.onMouseMove(x, y, false)

    //     //     }
    //     // }

    //     for(let obj of inner){
    //         if(obj.$textinput){
    //             obj.onMouseMove(x, y)
    //             break
    //         }
    //     }
        
    //     this.oldList = inner


    //     let pressedMouseArea = this.pressedMouseAreaInner ? this.pressedMouseAreaInner : this.pressedMouseAreaOuter

    //     // console.log(pressedMouseArea)

    //     if(pressedMouseArea && !pressedMouseArea.getPropertyValue('preventStealing') && this.flickList.length){
    //         let mx = false
    //         let my = false
    //         for(let f of this.flickList){
    //             if(f.getPropertyValue('width') < f.getPropertyValue('contentItem').getPropertyValue('width')) mx = true
    //             if(f.getPropertyValue('height') < f.getPropertyValue('contentItem').getPropertyValue('height')) my = true
    //         }
    //         if((Math.abs(this.originX - x) >= 10 && mx) || (Math.abs(this.originY - y) >= 10 && my)){
    //             pressedMouseArea = null
    //             this.pressedMouseAreaInner = null
    //             this.pressedMouseAreaOuter = null
    //         }
    //     }

    //     if(pressedMouseArea){
    //         // let normXY = this.normalizeXY(x, y, pressedMouseArea)
    //         if(pressedMouseArea.onMouseMove(x, y, true)){
    //             this.pressedMouseAreaInner = pressedMouseArea
    //             this.pressedMouseAreaOuter = null
    //         } else {
    //             this.pressedMouseAreaOuter = pressedMouseArea
    //             this.pressedMouseAreaInner = null
    //         }
    //     } else {
    //         if(this.flickList.length){
    //             while(this.flickList.length){
    //                 if(this.flickList[0].onMouseMove(ax, ay)) {
    //                     this.flickList.shift()
    //                 } else {
    //                     break
    //                 }
    //             }
    //         } else {
    //             for(let obj of inner){
    //                 // let normXY = this.normalizeXY(x, y, obj)
    //                 if(obj.$mousearea && obj.getPropertyValue('hoverEnabled')) {
    //                     obj.onMouseMove(x, y, false)
        
    //                 }
    //             }
    //         }
            
    //     }
        
        
    // }
}
class KeyboardController {
    shortcuts = []

    constructor(){
        window.onkeydown = (e)=>{

            for(let target of mainRoot.$activeFocusedElements.length ? mainRoot.$activeFocusedElements : mainRoot.$focusedElements){
                if(target){
                    if(e.key === 'Enter' || e.key === 'Return'){
                        if(target instanceof TextInput || target instanceof TextEdit) {
                            if(target instanceof TextInput) e.preventDefault()
                            target.onKeyDown(e.key)
                            return
                        } else {
                            continue
                        }
                        
                    }
                    if(e.key === 'Tab'){
                        e.preventDefault()
                        if(target instanceof TextEdit) {
                            target.onKeyDown(e.key)
                            return
                        }
                    }
                    
                    let obj = target
                    // let parent = target.parent
                    // let find = false
                    // while(parent && !find){
                    //     if(parent instanceof FocusScope){
                    //         find = true
                    //     } else {
                    //         parent = parent.parent
                    //     }
                    // }

                    // let obj = target
                    // if(!target.$properties.KeyNavigation && parent && parent.UID){
                    //     obj = parent
                    // }

                    if(obj.$properties.KeyNavigation){
                    
                        if(e.key === 'Shift' || e.key === 'Control' || e.key === 'Alt') return

                        if(e.key === 'ArrowLeft') {
                            if(obj.KeyNavigation.left){
                                e.preventDefault()
                                obj.KeyNavigation.left.forceActiveFocus()
                                return
                            }
                        }
                        if(e.key === 'ArrowRight') {
                            if(obj.KeyNavigation.right){
                                e.preventDefault()
                                obj.KeyNavigation.right.forceActiveFocus()
                                return
                            }
                        }
                        if(e.key === 'ArrowUp') {
                            if(obj.KeyNavigation.up){
                                e.preventDefault()
                                obj.KeyNavigation.up.forceActiveFocus()
                                return
                            }
                        }
                        if(e.key === 'ArrowDown') {
                            if(obj.KeyNavigation.down){
                                e.preventDefault()
                                obj.KeyNavigation.down.forceActiveFocus()
                                return
                            }
                        }
                        if(e.key === 'Tab' && e.shiftKey) {
                            if(obj.KeyNavigation.backtab){
                                e.preventDefault()
                                obj.KeyNavigation.backtab.forceActiveFocus()
                                return
                            }
                        } else if(e.key === 'Tab') {
                            if(obj.KeyNavigation.tab){
                                e.preventDefault()
                                obj.KeyNavigation.tab.forceActiveFocus()
                                return
                            }
                        }
                    }

                    if(obj.$signals['Keys.pressed']){
                        obj.$signals['Keys.pressed'](e)
                        return
                    }
                    if(e.key === 'ArrowLeft' && obj.$signals['Keys.leftPressed']){
                        obj.$signals['Keys.leftPressed'](e)
                        return
                    }
                    if(e.key === 'ArrowRight' && obj.$signals['Keys.rightPressed']){
                        obj.$signals['Keys.rightPressed'](e)
                        return
                    }
                    if(e.key === 'ArrowUp' && obj.$signals['Keys.upPressed']){
                        obj.$signals['Keys.upPressed'](e)
                        return
                    }
                    if(e.key === 'ArrowDown' && obj.$signals['Keys.downPressed']){
                        obj.$signals['Keys.downPressed'](e)
                        return
                    }
                    if(e.key === 'Return' && obj.$signals['Keys.returnPressed']){
                        obj.$signals['Keys.returnPressed'](e)
                        return
                    }
                    if(e.key === 'Enter' && obj.$signals['Keys.enterPressed']){
                        obj.$signals['Keys.enterPressed'](e)
                        return
                    }
                    if(e.key === 'Space' && obj.$signals['Keys.spacePressed']){
                        obj.$signals['Keys.spacePressed'](e)
                        return
                    }
                }
            }

            // if(!(target instanceof TextInput) && !(target instanceof TextEdit)){
                    
                if(e.key === 'Shift' || e.key === 'Control' || e.key === 'Alt') return
                let key = e.key
                if(e.key === 'ArrowLeft') key = 'Left'
                if(e.key === 'ArrowRight') key = 'Right'
                if(e.key === 'ArrowUp') key = 'Up'
                if(e.key === 'ArrowDown') key = 'Down'
                if(e.key === 'Return') key = 'Enter'
                if(e.code === 'Space') key = 'Space'


                let currentShortcuts = []
                for(let shortcut of this.shortcuts){
                    if(shortcut.getPropertyValue('enabled')){
                        if(shortcut.getPropertyValue('sequence')){
                            let stateKeys = {
                                altKey: false,
                                ctrlKey: false,
                                shiftKey: false,
                                key: ''
                            }
                            let skeys = shortcut.getPropertyValue('sequence').split('+')
                            for(let skey of skeys){
                                if(skey === 'Shift') {
                                    stateKeys.shiftKey = true
                                } else if(skey === 'Alt') {
                                    stateKeys.altKey = true
                                } else if(skey === 'Ctrl') {
                                    stateKeys.ctrlKey = true
                                } else {
                                    stateKeys.key = skey === 'Return' ? 'Enter' : skey
                                }
                            }
                            if(stateKeys.key === (key.length === 1 ? key.toUpperCase() : key) && stateKeys.shiftKey === e.shiftKey && stateKeys.ctrlKey === e.ctrlKey && stateKeys.altKey === e.altKey) {
                                currentShortcuts.push(shortcut)
                            } 
                        } else if(shortcut.getPropertyValue('sequences').length) {
                            let i = 0
                            let find = false
                            while(i < shortcut.getPropertyValue('sequences').length && !find){
                                let stateKeys = {
                                    altKey: false,
                                    ctrlKey: false,
                                    shiftKey: false,
                                    key: ''
                                }
                                let skeys = shortcut.getPropertyValue('sequences')[i].split('+')
                                for(let skey of skeys){
                                    if(skey === 'Shift') {
                                        stateKeys.shiftKey = true
                                    } else if(skey === 'Alt') {
                                        stateKeys.altKey = true
                                    } else if(skey === 'Ctrl') {
                                        stateKeys.ctrlKey = true
                                    } else {
                                        stateKeys.key = skey === 'Return' ? 'Enter' : skey
                                    }
                                }
                                if(stateKeys.key === (key.length === 1 ? key.toUpperCase() : key) && stateKeys.shiftKey === e.shiftKey && stateKeys.ctrlKey === e.ctrlKey && stateKeys.altKey === e.altKey) {
                                    currentShortcuts.push(shortcut)
                                    find = true
                                }
                                i++
                            }
                        }
                    }
                    
                }
                if(currentShortcuts.length >= 1){
                    e.preventDefault()
                    if(currentShortcuts.length === 1){
                        if(currentShortcuts[0].$signals.activated) currentShortcuts[0].$signals.activated()
                    } else {
                        if(currentShortcuts[currentShortcuts.length-1].$signals.activatedAmbiguously) currentShortcuts[currentShortcuts.length-1].$signals.activatedAmbiguously()
                        Core.shortcuts.splice(Core.shortcuts.indexOf(currentShortcuts[currentShortcuts.length-1]), 1)
                        Core.shortcuts.unshift(currentShortcuts[currentShortcuts.length-1])
                    }
                    return
                }
                
            
        }
    }

    add(obj){
        if(this.shortcuts.indexOf(obj) < 0) this.shortcuts.push(obj)
    }

    remove(obj){
        let index = this.shortcuts.indexOf(obj)
        if(index >= 0) this.shortcuts.splice(index, 1)
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
                    currentLine += wordWrap && wordWrap === 1 ? " " + word : word
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

class TransactionController {
    transactions = []

    begin(){
        this.transactions.push([])
    }

    add(func){
        if(this.transactions.length) this.transactions[this.transactions.length-1].push(func)
    }

    end(){
        let transaction = this.transactions.pop()
        if(transaction){
            while(transaction.legnth){
                transaction.shift()()
            }
        }
    }
}


module.exports.MouseController = MouseController
module.exports.KeyboardController = KeyboardController
module.exports.TextFontController = TextFontController
module.exports.AnimationController = AnimationController
module.exports.TransactionController = TransactionController
