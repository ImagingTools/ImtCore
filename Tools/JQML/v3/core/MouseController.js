const QtEnums = require("../Qt/enums")

class QmlMouseEvent {
    target = null
    path = []

    accepted = true
    button = QtEnums.LeftButton
    buttons = QtEnums.LeftButton
    flags = 0
    modifiers = QtEnums.NoModifier
    wasHeld = false
    originX = 0
    originY = 0
    x = 0
    y = 0

    moveX = 0
    moveY = 0
    pressed = false
    startX = 0
    startY = 0

    wasDragX = false
    wasDragY = false

    constructor(options = {}){
        for(let key in options){
            if(key in this) this[key] = options[key]
        }
    }

    getRelativePoint(obj){
        let rect = obj.__DOM.getBoundingClientRect()

        return {
            x: this.x - rect.x,
            y: this.y - rect.y,
        }
    }

    relative(obj){
        let rotation = 0
        let parent = obj
        while(parent){
            rotation += parent.rotation
            parent = parent.parent
        }

        let rad = -rotation*Math.PI/180

        let rect = obj.__DOM.getBoundingClientRect()

        let x = this.originX - rect.x
        let y = this.originY - rect.y

        if(rad !== 0){
            this.x = x*Math.cos(rad) - (y - rect.height / 2)*Math.sin(rad)
            this.y = x*Math.sin(rad) + (y + rect.height / 2)*Math.cos(rad)
        } else {
            this.x = x
            this.y = y
        }
        
    }

    fillButton(event){
        switch(event.button){
			case 0: this.button = QtEnums.LeftButton; break;
			case 1: this.button = QtEnums.MiddleButton; break;
			case 2: this.button = QtEnums.RightButton; break;
		}	
    }
}

class QmlWheelEvent {
    target = null
    path = []

    accepted = false
    modifiers = QtEnums.NoModifier
    angleDelta = {
        x: 0,
        y: 0,
    }
    originX = 0
    originY = 0
    x = 0
    y = 0

    constructor(options = {}){
        for(let key in options){
            if(key in this) this[key] = options[key]
        }
    }

    relative(obj){
        let rect = obj.__DOM.getBoundingClientRect()
        this.x = this.originX - rect.x
        this.y = this.originY - rect.y
    }

    getRelativePoint(obj){
        let rect = obj.__DOM.getBoundingClientRect()

        return {
            x: this.x - rect.x,
            y: this.y - rect.y,
        }
    }

}


module.exports = {
    objects: new Set(),
    dropAreas: new Set(),
    activePointerHandlers: new Set(),

    buttons: {
        0: {
            timeStamp: 0,
            target: null,
        },
        1: {
            timeStamp: 0,
            target: null,
        },
        2: {
            timeStamp: 0,
            target: null,
        },
        3: {
            timeStamp: 0,
            target: null,
        },
        4: {
            timeStamp: 0,
            target: null,
        },
    },
    
    event: null,

    entered: [],
    enteredSet: new Set(),
    hovered: [],

    __hitTestDepth: 0,

    add: function(obj){
        this.objects.add(obj)
        this.__markHitTarget(obj)
    },
    addDropArea: function(obj){
        this.dropAreas.add(obj)
        this.__markHitTarget(obj)
    },
    removeDropArea: function(obj){
        this.dropAreas.delete(obj)
        this.__unmarkHitTarget(obj)
    },
    remove: function(obj){
        this.objects.delete(obj)
        this.__unmarkHitTarget(obj)
        if(this.event){
            let index = this.event.path.indexOf(obj)
            while(index >= 0){
                this.event.path.splice(index, 1)
                index = this.event.path.indexOf(obj)
            }
        }
    },

    __markHitTarget: function(obj){
        if(!obj) return
        let dom = obj.__DOM
        if(!dom && typeof obj.__getDOM === 'function') dom = obj.__getDOM()
        if(dom) dom.classList.add('jq-mouse')
    },

    __unmarkHitTarget: function(obj){
        if(!obj || !obj.__DOM) return
        if(this.objects.has(obj) || this.dropAreas.has(obj)) return
        obj.__DOM.classList.remove('jq-mouse')
    },

    __beginHitTest: function(){
        if(this.__hitTestDepth++ === 0){
            document.documentElement.classList.add('jq-hittest')
        }
    },

    __endHitTest: function(){
        if(this.__hitTestDepth === 0) return
        this.__hitTestDepth--
        if(this.__hitTestDepth === 0){
            document.documentElement.classList.remove('jq-hittest')
        }
    },

    __elementsFromPoint: function(x, y){
        this.__beginHitTest()
        try {
            return document.elementsFromPoint(x, y)
        } finally {
            this.__endHitTest()
        }
    },

    getObjectsFromPoint: function(x, y){
        let elements = this.__elementsFromPoint(x, y)
        let result = []

        for(let i = 0; i < elements.length; i++){
            let qml = elements[i].qml
            if(qml && this.objects.has(qml) && !qml.__destroyed){
                result.push(qml)
            }
        }

        return result
    },

    getDropAreaFromPoint: function(x, y){
        let elements = this.__elementsFromPoint(x, y)

        for(let i = 0; i < elements.length; i++){
            let qml = elements[i].qml
            if(qml && this.dropAreas.has(qml) && !qml.__destroyed && qml.enabled && qml.visible){
                return qml
            }
        }

        return null
    },

    __getMouseAreaDragTarget: function(mouseArea){
        if(!mouseArea || mouseArea.__destroyed) return null

        let drag = mouseArea.drag
        if(!drag || typeof drag !== 'object') return null

        return drag.target || null
    },

    __setMouseAreaDragActive: function(mouseArea, active){
        if(!mouseArea || mouseArea.__destroyed) return

        let drag = mouseArea.drag
        if(!drag || typeof drag !== 'object') return

        drag.active = active
    },

    __getDragHotSpot: function(dragTarget){
        let hotX = 0
        let hotY = 0

        if(dragTarget && dragTarget.Drag && dragTarget.Drag.hotSpot){
            hotX = Number(dragTarget.Drag.hotSpot.x)
            hotY = Number(dragTarget.Drag.hotSpot.y)
            if(!Number.isFinite(hotX)) hotX = 0
            if(!Number.isFinite(hotY)) hotY = 0
        }

        return { x: hotX, y: hotY }
    },

    __findDropAreaForDragTarget: function(dragTarget){
        if(!dragTarget || !dragTarget.__DOM) return null

        let rect = dragTarget.__DOM.getBoundingClientRect()
        let hotSpot = this.__getDragHotSpot(dragTarget)

        let points = [
            [rect.left + hotSpot.x, rect.top + hotSpot.y],
            [rect.left + hotSpot.x, rect.bottom - hotSpot.y],
            [rect.right - hotSpot.x, rect.top + hotSpot.y],
            [rect.right - hotSpot.x, rect.bottom - hotSpot.y],
        ]

        for(let p of points){
            let dropArea = this.getDropAreaFromPoint(p[0], p[1])
            if(dropArea && typeof dropArea.__acceptsDragTarget === 'function' && dropArea.__acceptsDragTarget(dragTarget)){
                return dropArea
            }
        }

        return null
    },

    __updateDropAreasForDragTarget: function(dragTarget){
        if(!dragTarget) return

        let activeDropArea = this.__findDropAreaForDragTarget(dragTarget)

        if(activeDropArea && typeof activeDropArea.__enterOrMove === 'function'){
            activeDropArea.__enterOrMove(dragTarget)
        }

        for(let dropArea of this.dropAreas){
            if(!dropArea || dropArea.__destroyed || dropArea === activeDropArea) continue
            if(typeof dropArea.__exit === 'function'){
                dropArea.__exit(dragTarget)
            }
        }
    },

    __finishDropAreasForDragTarget: function(dragTarget){
        if(!dragTarget) return

        let activeDropArea = this.__findDropAreaForDragTarget(dragTarget)

        for(let dropArea of this.dropAreas){
            if(!dropArea || dropArea.__destroyed) continue
            if(typeof dropArea.__exit === 'function'){
                dropArea.__exit(dragTarget)
            }
        }

        if(activeDropArea && typeof activeDropArea.__drop === 'function'){
            activeDropArea.__drop(dragTarget)
        }
    },

    __updateMouseAreaDrag: function(mouseArea, mouse){
        if(!mouseArea || !mouse || !mouse.pressed) return

        let drag = mouseArea.drag
        if(!drag || typeof drag !== 'object') return

        let dragTarget = this.__getMouseAreaDragTarget(mouseArea)
        if(!dragTarget) return

        this.__setMouseAreaDragActive(mouseArea, true)

        let axis = Number(drag.axis)
        if(!Number.isFinite(axis)) axis = 2

        let dragTargetActive = !(dragTarget.Drag && dragTarget.Drag.active !== undefined) || !!dragTarget.Drag.active
        if(dragTargetActive){
            if(axis === 2 || axis === 0){
                dragTarget.x -= mouse.moveX
                if(Number.isFinite(Number(drag.minimumX)) && dragTarget.x < Number(drag.minimumX)) dragTarget.x = Number(drag.minimumX)
                if(Number.isFinite(Number(drag.maximumX)) && dragTarget.x > Number(drag.maximumX)) dragTarget.x = Number(drag.maximumX)
            }

            if(axis === 2 || axis === 1){
                dragTarget.y -= mouse.moveY
                if(Number.isFinite(Number(drag.minimumY)) && dragTarget.y < Number(drag.minimumY)) dragTarget.y = Number(drag.minimumY)
                if(Number.isFinite(Number(drag.maximumY)) && dragTarget.y > Number(drag.maximumY)) dragTarget.y = Number(drag.maximumY)
            }
        }

        this.__updateDropAreasForDragTarget(dragTarget)
    },

    __finishMouseAreaDrag: function(mouseArea){
        if(!mouseArea || mouseArea.__destroyed) return

        let dragTarget = this.__getMouseAreaDragTarget(mouseArea)
        this.__setMouseAreaDragActive(mouseArea, false)

        if(dragTarget){
            this.__finishDropAreasForDragTarget(dragTarget)
        }
    },

    init: function(){   
        window.addEventListener('mousemove', (e)=>{
            let event = this.event ? this.event : new QmlMouseEvent({path: this.getObjectsFromPoint(e.pageX, e.pageY)})
            
            let modifiers = QtEnums.NoModifier
            if(e.shiftKey) {
                modifiers |= QtEnums.ShiftModifier
            }
            if(e.altKey) {
                modifiers |= QtEnums.AltModifier
            }
            if(e.ctrlKey) {
                modifiers |= QtEnums.ControlModifier
            }

            event.modifiers = modifiers
            event.moveX = event.originX - e.pageX
            event.moveY = event.originY - e.pageY
            event.originX = e.pageX
            event.originY = e.pageY

            JQApplication.resetCursor('unset')

            if(event.pressed){
                if(Math.abs(event.startX - event.originX) > 15) event.wasDragX = true
                if(Math.abs(event.startY - event.originY) > 15) event.wasDragY = true
            }

            let i = 0
            while(i < this.entered.length){
                let obj = this.entered[i]
                if(obj.__destroyed) {
                    this.entered.splice(i, 1)
                    this.enteredSet.delete(obj)
                    continue
                }

                event.relative(obj)
                if(event.x < 0 || event.y < 0 || event.x >= obj.width || event.y >= obj.height) {
                    if(typeof obj.__onMouseLeave === 'function') obj.__onMouseLeave(event)
                    this.entered.splice(i, 1)
                    this.enteredSet.delete(obj)
                } else {
                    i++
                }
            }

            i = 0
            while(i < event.path.length){
                let obj = event.path[i]
                if(obj.__destroyed) {
                    event.path.splice(i, 1)
                    continue
                }

                event.relative(obj)
                if(event.x >= 0 && event.y >= 0 && event.x < obj.width && event.y < obj.height){
                    if(!this.enteredSet.has(obj)) {
                        this.entered.push(obj)
                        this.enteredSet.add(obj)
                        if(typeof obj.__onMouseEnter === 'function') obj.__onMouseEnter(event)
                    }
                }
                if(typeof obj.__onMouseMove === 'function') obj.__onMouseMove(event)

                i++
            }

            let activeHandlers = []
            for(let handler of this.activePointerHandlers){
                if(!handler || handler.__destroyed || !handler.active){
                    this.activePointerHandlers.delete(handler)
                    continue
                }
                if(event.path.indexOf(handler) < 0){
                    activeHandlers.push(handler)
                }
            }

            for(let handler of activeHandlers){
                if(typeof handler.__onMouseMove === 'function') handler.__onMouseMove(event)
            }

        })

        window.addEventListener('click', (e)=>{
            if(this.event) this.event.fillButton(e)
            this.click(e)
        })
        window.addEventListener('dblclick', (e)=>{
            e.preventDefault()
        })
        window.addEventListener('mousedown', (e)=>{
            this.event = new QmlMouseEvent({pressed: true})

            let modifiers = QtEnums.NoModifier
            if(e.shiftKey) {
                modifiers |= QtEnums.ShiftModifier
            }
            if(e.altKey) {
                modifiers |= QtEnums.AltModifier
            }
            if(e.ctrlKey) {
                modifiers |= QtEnums.ControlModifier
            }

            this.event.modifiers = modifiers
            this.event.fillButton(e)
            this.event.originX = e.pageX
            this.event.originY = e.pageY
            this.event.startX = e.pageX
            this.event.startY = e.pageY
            this.event.path = this.getObjectsFromPoint(e.pageX, e.pageY)

            for(let obj of this.event.path){
                this.event.accepted = true
                this.event.relative(obj)
                if(typeof obj.__onMouseDown === 'function') obj.__onMouseDown(this.event)
                if(obj.active && this.activePointerHandlers.indexOf && false){}
                if(obj.active && this.activePointerHandlers.add){
                    this.activePointerHandlers.add(obj)
                }
            }
        })
        window.addEventListener('mouseup', (e)=>{
            if(this.event){
                let modifiers = QtEnums.NoModifier
                if(e.shiftKey) {
                    modifiers |= QtEnums.ShiftModifier
                }
                if(e.altKey) {
                    modifiers |= QtEnums.AltModifier
                }
                if(e.ctrlKey) {
                    modifiers |= QtEnums.ControlModifier
                }

                this.event.modifiers = modifiers

                this.event.fillButton(e)
                this.event.originX = e.pageX
                this.event.originY = e.pageY

                for(let handler of this.activePointerHandlers){
                    if(!handler || handler.__destroyed) continue
                    if(typeof handler.__onMouseUp === 'function') handler.__onMouseUp(this.event)
                }
                
                if(this.event.target) {
                    this.event.relative(this.event.target)
                    if(typeof this.event.target.__onMouseUp === 'function') this.event.target.__onMouseUp(this.event)
                    this.__finishMouseAreaDrag(this.event.target)
                }

                this.activePointerHandlers.clear()
            }
        })
        window.addEventListener('contextmenu', (e)=>{
            let modifiers = QtEnums.NoModifier
            if(e.shiftKey) {
                modifiers |= QtEnums.ShiftModifier
            }
            if(e.altKey) {
                modifiers |= QtEnums.AltModifier
            }
            if(e.ctrlKey) {
                modifiers |= QtEnums.ControlModifier
            }

            this.event.modifiers = modifiers
            this.event.fillButton(e)
            e.preventDefault()
            this.click(e)
        })
        // window.addEventListener('touchstart', (e)=>{

        // })
        // window.addEventListener('touchend', (e)=>{

        // })
        // window.addEventListener('touchmove', (e)=>{

        // })
        window.addEventListener('wheel', (e)=>{
            this.event = new QmlWheelEvent()

            let modifiers = QtEnums.NoModifier
            if(e.shiftKey) {
                modifiers |= QtEnums.ShiftModifier
            }
            if(e.altKey) {
                modifiers |= QtEnums.AltModifier
            }
            if(e.ctrlKey) {
                modifiers |= QtEnums.ControlModifier
            }

            this.event.modifiers = modifiers
            this.event.originX = e.pageX
            this.event.originY = e.pageY
            this.event.angleDelta.x = e.deltaX / 8
            this.event.angleDelta.y = -e.deltaY / 8
            this.event.path = this.getObjectsFromPoint(e.pageX, e.pageY)

            for(let obj of this.event.path){
                if(!this.event.accepted || !this.event.target){
                    this.event.relative(obj)
                    if(typeof obj.__onWheel === 'function') obj.__onWheel(this.event)
                }
            }

            this.event = null
        })
    },

    click: function(e){
        if(this.event && this.event.target){
            if(this.event.target.__destroyed) return
            
            let event = this.event
            JQApplication.beginUpdate()

            event.originX = e.pageX
            event.originY = e.pageY
            event.relative(event.target)

            let _button = this.buttons[event.button]

            if(e.timeStamp - _button.timeStamp > 300 || _button.target !== event.target){
                _button.timeStamp = e.timeStamp
                _button.target = event.target
                event.target.__onMouseClick(event)
                this.__propagateComposedEvent(event, event.target, 'click')
            } else {
                event.target.__onMouseDblClick(event)
                this.__propagateComposedEvent(event, event.target, 'dblclick')
            }
            
            event.target.__onMouseLeave(event)

            JQApplication.endUpdate()
        }

        this.event = null
    },

    __propagateComposedEvent: function(event, origin, kind){
        if(!event || !origin || origin.__destroyed) return
        if(!origin.propagateComposedEvents || event.accepted) return

        let path = event.path && event.path.length ? event.path : this.getObjectsFromPoint(event.originX, event.originY)
        let start = 0
        for(let i = 0; i < path.length; i++){
            if(path[i] === origin){
                start = i + 1
                break
            }
        }

        for(let i = start; i < path.length; i++){
            let obj = path[i]
            if(!obj || obj === origin || obj.__destroyed) continue

            if(kind === 'click'){
                if(typeof obj.__emitComposedClick !== 'function') continue
                event.relative(obj)
                obj.__emitComposedClick(event)
            } else {
                if(typeof obj.__emitComposedDoubleClick !== 'function') continue
                event.relative(obj)
                obj.__emitComposedDoubleClick(event)
            }

            if(event.accepted) return
        }
    },
}