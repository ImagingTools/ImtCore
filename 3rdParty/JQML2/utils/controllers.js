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
            this.onMouseMove(e.pageX, e.pageY, e)
        })
        
        window.addEventListener('dblclick', (e)=>{
            e.preventDefault()
        })
        window.addEventListener('mousedown', (e)=>{
            this.onMouseDown(e.pageX, e.pageY, e.button, e)
        })
        window.addEventListener('mouseup', (e)=>{
            if(e.button !== 2) this.onMouseUp(e.pageX, e.pageY, e.button, e)
        })
        window.addEventListener('contextmenu', (e)=>{
            e.preventDefault()
            this.onMouseUp(e.pageX, e.pageY, e.button, e)
        })
        window.addEventListener('touchstart', (e)=>{
            this.onMouseDown(e.changedTouches[0].pageX, e.changedTouches[0].pageY, 0, e)
        })
        window.addEventListener('touchend', (e)=>{
            this.onMouseUp(e.changedTouches[0].pageX, e.changedTouches[0].pageY, 0, e)
        })
        window.addEventListener('touchmove', (e)=>{
            this.onMouseMove(e.changedTouches[0].pageX, e.changedTouches[0].pageY, e)
        })
        window.addEventListener('wheel', (e)=>{
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
            if(obj instanceof MouseArea){
                if(obj.$signals.wheel){
                    obj.$signals.wheel.accepted = true
                    let rect = obj.getDom().getBoundingClientRect()
                    obj.$signals.wheel.x = x - rect.x
                    obj.$signals.wheel.y = y - rect.y
                    obj.$signals.wheel.angleDelta = {
                        x: deltaX / 8,
                        y: -deltaY / 8
                    }
                    obj.$signals.wheel()
                    if(obj.$signals.wheel.accepted) return
                }
            }if(obj instanceof ListView){
                if(obj.getPropertyValue('enabled') && obj.getPropertyValue('visible') && obj.getPropertyValue('interactive')) {
                    if(obj.getPropertyValue('orientation') === ListView.Vertical){
                        if(obj.getPropertyValue('contentHeight') <= obj.getPropertyValue('height')){
                            obj.getStatement('contentY').reset(0)
                        } else if(obj.getPropertyValue('contentY') + (deltaY) > obj.getPropertyValue('originY') && obj.getPropertyValue('contentY') + (deltaY) < obj.getPropertyValue('contentHeight') + obj.getPropertyValue('originY') - obj.getPropertyValue('height')){
                            obj.getStatement('contentY').reset(obj.getPropertyValue('contentY') + (deltaY))
                            return
                        } else {
                            if(obj.getPropertyValue('contentY') + (deltaY) <= obj.getPropertyValue('originY')) {
                                obj.getStatement('contentY').reset(obj.getPropertyValue('originY'))
                            }
                            if(obj.getPropertyValue('contentY') + (deltaY) >= obj.getPropertyValue('contentHeight') + obj.getPropertyValue('originY') - obj.getPropertyValue('height')) {
                                obj.getStatement('contentY').reset(obj.getPropertyValue('contentHeight') + obj.getPropertyValue('originY') - obj.getPropertyValue('height'))
                            }
            
                        }
                    }
                    
                }
            } else if(obj instanceof Flickable){
                if(obj.getPropertyValue('enabled') && obj.getPropertyValue('visible') && obj.getPropertyValue('interactive')) {
                    if(obj.getPropertyValue('contentY') + (deltaY) > 0 && obj.getPropertyValue('contentY') + (deltaY) < obj.getPropertyValue('contentItem').getPropertyValue('height') - obj.getPropertyValue('height')){
                        obj.getStatement('contentY').reset(obj.getPropertyValue('contentY') + (deltaY))
                        return
                    } else {
                        if(obj.getPropertyValue('contentY') + (deltaY) <= 0) obj.getStatement('contentY').reset(0)
                        if(obj.getPropertyValue('contentY') + (deltaY) >= obj.getPropertyValue('contentItem').getPropertyValue('height') - obj.getPropertyValue('height')) obj.getStatement('contentY').reset(obj.getPropertyValue('contentItem').getPropertyValue('height') - obj.getPropertyValue('height'))
                    }
                }
            } else if(obj instanceof Map){
                let currentZoomLevel = obj.getPropertyValue('zoomLevel')
                if(deltaY > 0){
                    obj.getProperty('zoomLevel').reset(currentZoomLevel-0.1)
                } else if(deltaY < 0){
                    obj.getProperty('zoomLevel').reset(currentZoomLevel+0.1)
                }
                return
            }
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
        this.pressed = inner
        let view = null


        for(let i = 0; i < this.oldList.length; i++){
            if(this.oldList[i] instanceof MouseArea){
                this.pressed[i].getProperty('pressed').value = false
            }
        }

        this.oldList = []
        
        if(button === 0 && inner[0] && (inner[0] instanceof TextInput || inner[0] instanceof TextEdit || inner[0] instanceof Map)){
            if(inner[0].getPropertyValue('enabled') && inner[0].getPropertyValue('visible')) {
                this.target = inner[0]
                return
            }
        } 

        for(let i = 0; i < inner.length; i++){
            if(button === 0 && (inner[i] instanceof TextInput || inner[i] instanceof TextEdit)){
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
                        inner[i].mouse.accepted = true
                        inner[i].getProperty('pressed').notify()
                    } else {
                        inner[i].mouse.accepted = true
                    }

                    

                    if(inner[i].mouse.accepted) {
                        this.target = inner[i]
                        this.pressedMouseArea = [inner[i]]
                        for(let k = i + 1; k < inner.length; k++){
                            // if(button === 0 && (inner[k] instanceof TextInput || inner[k] instanceof TextEdit)){
                            //     if(inner[k].getPropertyValue('enabled') && inner[k].getPropertyValue('visible') && inner[k].getPropertyValue('activeFocus')) {
                            //         this.target = inner[k]
                            //         break
                            //     }
                            // }
                            if(inner[k] instanceof MouseArea && inner[k].getPropertyValue('enabled') && inner[k].getPropertyValue('visible') && inner[k].availableButton(button))
                            this.pressedMouseArea.push(inner[k])
                        }
                        break
                    }
                    
                }
            }
        }

        if(!this.target || !(this.target instanceof TextInput || this.target instanceof TextEdit || this.target instanceof Map)){
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

                if(x >= rect.left && x <= rect.right && y >= rect.top && y <= rect.bottom){
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
                    if(button === 0 && (this.pressed[i] instanceof TextInput || this.pressed[i] instanceof TextEdit)){
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
                    if(button === 0 && (this.pressed[i] instanceof TextInput || this.pressed[i] instanceof TextEdit)){
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
                        let parent = this.target.getPropertyValue('parent')
                        while(parent){
                            if(this.pressed.indexOf(parent) && parent instanceof Flickable && parent.getPropertyValue('enabled') && parent.getPropertyValue('visible') && parent.getPropertyValue('interactive')) {
                                if((Math.abs(this.originX - x) >= 10 && parent.getPropertyValue('width') < parent.getPropertyValue('contentItem').getPropertyValue('width')) || 
                                (Math.abs(this.originY - y) >= 10 && parent.getPropertyValue('height') < parent.getPropertyValue('contentItem').getPropertyValue('height'))){
                                    this.target.getProperty('pressed').value = false
                                    this.target = parent
                                    break
                                }
                                
                            }
                            parent = parent.getPropertyValue('parent')
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
            let accepted = false
            let wasInner = false
            let wasCursor = false
            
            for(let i = this.list.length-1; i >= 0; i--){
                if(!wasCursor && this.list[i].UID && (this.list[i] instanceof TextInput || this.list[i] instanceof TextEdit)){
                    if(inner.indexOf(this.list[i]) >= 0){
                        if(!this.list[i].getPropertyValue('readOnly')) document.body.style.cursor = 'text'
                        wasCursor = true
                    }
                    
                }
                if(this.list[i].UID && this.list[i] instanceof MouseArea && this.list[i].getPropertyValue('hoverEnabled')){
                    let rect = this.list[i].getDom().getBoundingClientRect()
                    let norm = this.list[i].normalizeXY(x - rect.x, y - rect.y)
                    this.list[i].mouse.x = norm.x
                    this.list[i].mouse.y = norm.y
                    this.list[i].getStatement('mouseX').reset(norm.x)
                    this.list[i].getStatement('mouseY').reset(norm.y)
    
                    if(inner.indexOf(this.list[i]) >= 0){
                        
                            this.list[i].mouse.accepted = false
                            if(!accepted && (this.list[i].getPropertyValue('pressed') || this.list[i].getPropertyValue('hoverEnabled')) && this.list[i].$signals.positionChanged) {
                                this.list[i].$signals.positionChanged()
                                accepted = this.list[i].mouse.accepted

                                this.list[i].getProperty('containsMouse').reset(true)
                                if(this.list[i].$signals && this.list[i].$signals.entered && !this.list[i].$entered) {
                                    this.list[i].$signals.entered()
                                }
                                this.list[i].$entered = true
                                if(!wasCursor){
                                    document.body.style.cursor = this.list[i].getPropertyValue('cursorShape')
                                    wasCursor = true
                                }
                                
                                wasInner = true
                            } else {
                                if(!wasInner){
                                    this.list[i].getProperty('containsMouse').reset(true)
                                    if(this.list[i].$signals && this.list[i].$signals.entered && !this.list[i].$entered) {
                                        this.list[i].$signals.entered()
                                    }
                                    this.list[i].$entered = true
                                    if(!wasCursor){
                                        document.body.style.cursor = this.list[i].getPropertyValue('cursorShape')
                                        wasCursor = true
                                    }
                                    
                                    wasInner = true
                                }
                            }

                            
                    } else {
                        this.list[i].getProperty('containsMouse').reset(false) 
                        if(this.list[i].$signals && this.list[i].$signals.exited && this.list[i].$entered) {  
                            this.list[i].$signals.exited()
                        }
                        delete this.list[i].$entered
                    }
                    
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

}
class KeyboardController {
    shortcuts = []

    constructor(){
        window.onkeydown = (e)=>{
            let elements = mainRoot.$activeFocusedElements.slice()
            for(let el of mainRoot.$focusedElements){
                if(elements.indexOf(el) < 0) elements.push(el)
            }
        
            for(let target of elements){
                if(target){
                    if(e.key === 'Enter' || e.key === 'Return'){
                        if(target instanceof TextInput || target instanceof TextEdit) {
                            if(target instanceof TextInput) e.preventDefault()
                            target.onKeyDown(e.key)
                            return
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

                    let parent = obj
                    while(parent){
                        if(parent.$properties.KeyNavigation){
                    
                            if(e.key === 'Shift' || e.key === 'Control' || e.key === 'Alt') return
    
                            if(e.key === 'ArrowLeft') {
                                if(parent.KeyNavigation.left){
                                    e.preventDefault()
                                    parent.KeyNavigation.left.getProperty('focus').reset(true)
                                    return
                                }
                            }
                            if(e.key === 'ArrowRight') {
                                if(parent.KeyNavigation.right){
                                    e.preventDefault()
                                    parent.KeyNavigation.right.getProperty('focus').reset(true)
                                    return
                                }
                            }
                            if(e.key === 'ArrowUp') {
                                if(parent.KeyNavigation.up){
                                    e.preventDefault()
                                    parent.KeyNavigation.up.getProperty('focus').reset(true)
                                    return
                                }
                            }
                            if(e.key === 'ArrowDown') {
                                if(parent.KeyNavigation.down){
                                    e.preventDefault()
                                    parent.KeyNavigation.down.getProperty('focus').reset(true)
                                    return
                                }
                            }
                            if(e.key === 'Tab' && e.shiftKey) {
                                if(parent.KeyNavigation.backtab){
                                    e.preventDefault()
                                    parent.KeyNavigation.backtab.getProperty('focus').reset(true)
                                    return
                                }
                            } else if(e.key === 'Tab') {
                                if(parent.KeyNavigation.tab){
                                    e.preventDefault()
                                    parent.KeyNavigation.tab.getProperty('focus').reset(true)
                                    return
                                }
                            }
                        }
                        parent = parent.getPropertyValue('parent')
                    }
                    

                    if(obj.$signals['Keys.pressed']){
                        e.accepted = false
                        obj.$signals['Keys.pressed'](e)
                        if(e.accepted) return
                    }
                    if(e.key === 'ArrowLeft' && obj.$signals['Keys.leftPressed']){
                        e.accepted = false
                        obj.$signals['Keys.leftPressed'](e)
                        if(e.accepted) return
                    }
                    if(e.key === 'ArrowRight' && obj.$signals['Keys.rightPressed']){
                        e.accepted = false
                        obj.$signals['Keys.rightPressed'](e)
                        if(e.accepted) return
                    }
                    if(e.key === 'ArrowUp' && obj.$signals['Keys.upPressed']){
                        e.accepted = false
                        obj.$signals['Keys.upPressed'](e)
                        if(e.accepted) return
                    }
                    if(e.key === 'ArrowDown' && obj.$signals['Keys.downPressed']){
                        e.accepted = false
                        obj.$signals['Keys.downPressed'](e)
                        if(e.accepted) return
                    }
                    if(e.key === 'Return' && obj.$signals['Keys.returnPressed']){
                        e.accepted = false
                        obj.$signals['Keys.returnPressed'](e)
                        if(e.accepted) return
                    }
                    if(e.key === 'Enter' && obj.$signals['Keys.enterPressed']){
                        e.accepted = false
                        obj.$signals['Keys.enterPressed'](e)
                        if(e.accepted) return
                    }
                    if(e.key === 'Space' && obj.$signals['Keys.spacePressed']){
                        e.accepted = false
                        obj.$signals['Keys.spacePressed'](e)
                        if(e.accepted) return
                    }
                }
            }

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
                    currentShortcuts.splice(currentShortcuts.indexOf(currentShortcuts[currentShortcuts.length-1]), 1)
                    currentShortcuts.unshift(currentShortcuts[currentShortcuts.length-1])
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
        this.container = document.createElement('div')
        this.container.style.position = 'absolute'
        this.container.style.display = 'inline'
        this.container.style.opacity = 0
        this.container.style.lineHeight = 'normal'
        document.body.appendChild(this.container)
    }

    measureText(text, font, maxWidth, wrapMode, textFormat){
        this.container.style.fontFamily = font.getPropertyValue('family')
        this.container.style.fontSize = font.getPropertyValue('pixelSize')+'px'
        this.container.style.fontWeight = font.getPropertyValue('bold') ? 'bold' : 'normal'
        this.container.style.fontStyle = font.getPropertyValue('italic') ? 'italic' : 'normal'
        this.container.style.textDecoration = font.getPropertyValue('underline') ? 'underline' : 'unset'
        if(maxWidth){
            this.container.style.maxWidth = maxWidth+'px'
            switch(wrapMode){
                case Text.NoWrap: this.container.style.whiteSpace = 'pre'; this.container.style.wordBreak = 'unset'; break;
                case Text.WordWrap: this.container.style.whiteSpace ='pre-wrap'; this.container.style.wordBreak = 'break-word'; break;
                case Text.WrapAnywhere: this.container.style.whiteSpace ='pre-wrap'; this.container.style.wordBreak = 'break-all'; break;
                case Text.Wrap: this.container.style.whiteSpace ='pre-wrap'; this.container.style.wordBreak = 'break-word'; break;
                case Text.WrapAtWordBoundaryOrAnywhere: this.container.style.whiteSpace ='pre-wrap'; this.container.style.wordBreak = 'break-word'; break;
            }
        } else {
            this.container.style.maxWidth = 'unset'
            this.container.style.whiteSpace = 'pre'; 
            this.container.style.wordBreak = 'unset';
        }
        
        if(textFormat === undefined || textFormat === Text.PlainText || textFormat === Text.AutoText){
            this.container.innerText = text
        } else {
            this.container.innerHTML = text
        }
        

        // if(accuracy){
            let rect = this.container.getBoundingClientRect()
            return rect
        // } else {
        //     return {
        //         width: this.container.clientWidth,
        //         height: this.container.clientHeight,
        //     }
        // }
        
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

class ImageController {
    cache = {}

    load(url, onLoaded, onError) {
        let item = this.cache[url]
        if(!item) {
            item = {
                data: '',
                width: 0,
                height: 0,
                status: 0,
                onLoaded: new QSignal('img'),
                onError: new QSignal(),
            }
            this.cache[url] = item
        }
        
        if(item.status === 2) {
            if(onLoaded) onLoaded(item)
            return
        } else if(item.status === 1) {
            if(onLoaded) item.onLoaded.connect(onLoaded)
            if(onError) item.onError.connect(onError)
            return
        } else if(item.status === -1){
            if(onError) onError()
            return
        } else {
            if(onLoaded) item.onLoaded.connect(onLoaded)
            if(onError) item.onError.connect(onError)
            
            item.status = 1
            let xhr = new XMLHttpRequest()
            xhr.onload = ()=>{
                if(xhr.status === 200){
                    let reader = new FileReader()
                    reader.onloadend = ()=>{
                        let img = new OriginImage()
                        img.onload = ()=>{
                            item.data = reader.result,
                            item.width = img.naturalWidth,
                            item.height = img.naturalHeight,
                            img.remove()

                            item.status = 2
                            item.onLoaded(item)
                        }
                        img.src = reader.result
                    }
                    reader.readAsDataURL(xhr.response)
                } else {
                    item.status = -1
                    item.onError()
                }
                
            }
            xhr.open('GET', url)
            xhr.responseType = 'blob'
            xhr.send()
        }

    }
}


module.exports.MouseController = MouseController
module.exports.KeyboardController = KeyboardController
module.exports.TextFontController = TextFontController
module.exports.AnimationController = AnimationController
module.exports.ImageController = ImageController
