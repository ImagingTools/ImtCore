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
    isClick = false
    isClick = false
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

    event: null,

    // Qt defaults (QPlatformTheme): used when the browser gives no click count, as for touch
    __doubleClickInterval: 400,
    __doubleClickDistance: 5,
    __lastPress: null,

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

    __startMouseAreaDrag: function(mouseArea, mouse){
        mouseArea.__dragMoved = false
        mouseArea.__dragOverThreshold = false
        mouseArea.__stealMouse = mouseArea.preventStealing
        mouseArea.__keepMouseGrab = mouseArea.__stealMouse
        mouseArea.__dragStartScene = { x: mouse.originX, y: mouse.originY }
        this.__setMouseAreaDragActive(mouseArea, false)
    },

    // Qt: QQuickMouseArea::mouseMoveEvent, the drag part
    __updateMouseAreaDrag: function(mouseArea, mouse){
        if(!mouseArea || !mouse || !mouseArea.__dragStartScene) return

        let drag = mouseArea.drag
        let dragTarget = this.__getMouseAreaDragTarget(mouseArea)
        if(!dragTarget) return

        if(!mouseArea.__dragMoved){
            mouseArea.__dragTargetStart = { x: dragTarget.x, y: dragTarget.y }
        }

        let axis = Number(drag.axis)
        let dragX = (axis & 1) !== 0
        let dragY = (axis & 2) !== 0
        let start = mouseArea.__dragTargetStart
        let deltaX = mouse.originX - mouseArea.__dragStartScene.x
        let deltaY = mouse.originY - mouseArea.__dragStartScene.y

        if(mouseArea.__keepMouseGrab && mouseArea.__stealMouse && mouseArea.__dragOverThreshold && !drag.active){
            this.__setMouseAreaDragActive(mouseArea, true)
        }

        let targetX = dragTarget.x
        let targetY = dragTarget.y
        let dragPosX = targetX
        let dragPosY = targetY
        let boundedX = targetX
        let boundedY = targetY
        if(dragX){
            dragPosX = start.x + deltaX
            boundedX = Math.max(Number(drag.minimumX), Math.min(dragPosX, Number(drag.maximumX)))
        }
        if(dragY){
            dragPosY = start.y + deltaY
            boundedY = Math.max(Number(drag.minimumY), Math.min(dragPosY, Number(drag.maximumY)))
        }

        if(drag.active){
            dragTarget.x = boundedX
            dragTarget.y = boundedY
        }

        let threshold = Number(drag.threshold)
        let overThresholdX = Math.abs(dragPosX - start.x) > threshold
        let overThresholdY = Math.abs(dragPosY - start.y) > threshold
        if(!mouseArea.__dragOverThreshold && ((targetX !== boundedX && overThresholdX) || (targetY !== boundedY && overThresholdY))){
            mouseArea.__dragOverThreshold = true
            if(drag.smoothed) mouseArea.__dragStartScene = { x: mouse.originX, y: mouse.originY }
        }

        if(!mouseArea.__keepMouseGrab && mouseArea.__dragOverThreshold){
            mouseArea.__keepMouseGrab = true
            mouseArea.__stealMouse = true
        }

        mouseArea.__dragMoved = true

        if(drag.active) this.__updateDropAreasForDragTarget(dragTarget)
    },

    // As in Qt Quick, hover goes to the topmost hover-enabled MouseArea under
    // the cursor (and its ancestors); hover-enabled MouseAreas beneath it, e.g.
    // rows covered by a dialog overlay, must not react to the cursor.
    __findHoverBlocker: function(event){
        for(let obj of event.path){
            if(!obj || obj.__destroyed || typeof obj.__acceptsHover !== 'function' || !obj.__acceptsHover()) continue

            event.relative(obj)
            if(event.x >= 0 && event.y >= 0 && event.x < obj.width && event.y < obj.height) return obj
        }

        return null
    },

    __isHoverBlocked: function(obj, blocker){
        if(!blocker || obj === blocker || typeof obj.__acceptsHover !== 'function') return false

        let parent = blocker.parent
        while(parent){
            if(parent === obj) return false
            parent = parent.parent
        }

        return true
    },

    __finishMouseAreaDrag: function(mouseArea){
        if(!mouseArea || mouseArea.__destroyed) return

        let dragTarget = this.__getMouseAreaDragTarget(mouseArea)
        this.__setMouseAreaDragActive(mouseArea, false)
        mouseArea.__dragStartScene = null
        mouseArea.__dragOverThreshold = false
        mouseArea.__stealMouse = false
        if(!mouseArea.preventStealing) mouseArea.__keepMouseGrab = false

        if(dragTarget){
            this.__finishDropAreasForDragTarget(dragTarget)
        }
    },

    __touchPointerId: null,
    __lastTouchTime: -Infinity,
    __touchSuppressInterval: 1000,

    __isTouchPointer: function(e){
        return e.pointerType === 'touch' || e.pointerType === 'pen'
    },

    // Browsers emulate mouse events (and a click) after a touch or pen
    // interaction. They are already handled through pointer events.
    __isEmulatedMouseEvent: function(){
        if(this.__touchPointerId !== null) return true

        return performance.now() - this.__lastTouchTime < this.__touchSuppressInterval
    },

    __isEditableElement: function(element){
        if(!element || !element.tagName) return false
        if(element.isContentEditable) return true

        let tagName = element.tagName.toUpperCase()

        return tagName === 'INPUT' || tagName === 'TEXTAREA' || tagName === 'SELECT'
    },

    __clearHover: function(skipObj){
        let event = new QmlMouseEvent()
        let entered = this.entered

        this.entered = []
        this.enteredSet.clear()

        for(let obj of entered){
            if(!obj || obj.__destroyed || obj === skipObj) continue
            event.target = obj
            if(typeof obj.__onMouseLeave === 'function') obj.__onMouseLeave(event)
        }
    },

    __cancelPress: function(){
        let event = this.event
        if(!event || !(event instanceof QmlMouseEvent)){
            this.event = null
            return
        }

        for(let handler of this.activePointerHandlers){
            if(!handler || handler.__destroyed) continue
            if(typeof handler.__onMouseCanceled === 'function') handler.__onMouseCanceled(event)
        }
        this.activePointerHandlers.clear()

        let target = event.target
        if(target && !target.__destroyed){
            if(typeof target.__onMouseCanceled === 'function') target.__onMouseCanceled(event)
            this.__finishMouseAreaDrag(target)
        }

        this.event = null
    },

    __onMouseMove: function(e){
        let lastPress = this.__lastPress
        if(lastPress && (Math.abs(e.clientX - lastPress.x) > this.__doubleClickDistance || Math.abs(e.clientY - lastPress.y) > this.__doubleClickDistance)){
            this.__lastPress = null
        }

        let event = this.event ? this.event : new QmlMouseEvent({path: this.getObjectsFromPoint(e.clientX, e.clientY)})
        
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
        event.moveX = event.originX - e.clientX
        event.moveY = event.originY - e.clientY
        event.originX = e.clientX
        event.originY = e.clientY

        JQApplication.resetCursor('unset')

        if(event.pressed){
            if(Math.abs(event.startX - event.originX) > 15) event.wasDragX = true
            if(Math.abs(event.startY - event.originY) > 15) event.wasDragY = true
        }

        let hoverBlocker = event.target ? null : this.__findHoverBlocker(event)

        const pointerInside = (obj) => {
            event.relative(obj)
            return event.x >= 0 && event.y >= 0 && event.x < obj.width && event.y < obj.height
        }

        // Physical exits must precede entries into unrelated areas.
        let i = 0
        while(i < this.entered.length){
            let obj = this.entered[i]
            if(obj.__destroyed) {
                this.entered.splice(i, 1)
                this.enteredSet.delete(obj)
                continue
            }

            if(!pointerInside(obj)) {
                if(typeof obj.__onMouseLeave === 'function') obj.__onMouseLeave(event)
                this.entered.splice(i, 1)
                this.enteredSet.delete(obj)
            } else {
                i++
            }
        }

        // Enter the new topmost area before dispatching blocked exits.
        // This keeps hover-dependent close buttons enabled during handoff.
        i = 0
        while(i < event.path.length){
            let obj = event.path[i]
            if(obj.__destroyed) {
                event.path.splice(i, 1)
                continue
            }

            if(!this.__isHoverBlocked(obj, hoverBlocker) && pointerInside(obj) && !this.enteredSet.has(obj)) {
                this.entered.push(obj)
                this.enteredSet.add(obj)
                if(typeof obj.__onMouseEnter === 'function') obj.__onMouseEnter(event)
            }

            i++
        }

        i = 0
        while(i < this.entered.length){
            let obj = this.entered[i]
            if(obj.__destroyed) {
                this.entered.splice(i, 1)
                this.enteredSet.delete(obj)
                continue
            }

            if(!pointerInside(obj) || this.__isHoverBlocked(obj, hoverBlocker)) {
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

            if(!this.__isHoverBlocked(obj, hoverBlocker) && typeof obj.__onMouseMove === 'function') {
                event.relative(obj)
                obj.__onMouseMove(event)
            }

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
    },

    __onMouseDown: function(e){
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
        this.event.originX = e.clientX
        this.event.originY = e.clientY
        this.event.startX = e.clientX
        this.event.startY = e.clientY
        this.event.path = this.getObjectsFromPoint(e.clientX, e.clientY)

        let isDoubleClick = this.__isDoubleClick(e, this.event.button)

        for(let obj of this.event.path){
            this.event.accepted = true
            this.event.relative(obj)
            if(typeof obj.__onMouseDown === 'function') obj.__onMouseDown(this.event)
            if(obj.active && this.activePointerHandlers.add){
                this.activePointerHandlers.add(obj)
            }
        }

        // Qt: the double click event follows the second press
        let target = this.event.target
        if(isDoubleClick && target && !target.__destroyed && typeof target.__onMouseDoubleClick === 'function'){
            this.event.relative(target)
            target.__onMouseDoubleClick(this.event)
        }
    },

    // Qt (QGuiApplicationPrivate::processMouseEvent): a second press of the same button in time and place; the press after a double click starts again
    __isDoubleClick: function(e, button){
        if(e.detail > 0) return e.detail % 2 === 0

        let now = performance.now()
        let last = this.__lastPress
        let isDoubleClick = !!last && last.button === button && now - last.time < this.__doubleClickInterval
            && Math.abs(e.clientX - last.x) <= this.__doubleClickDistance && Math.abs(e.clientY - last.y) <= this.__doubleClickDistance

        this.__lastPress = isDoubleClick ? null : { time: now, button: button, x: e.clientX, y: e.clientY }
        return isDoubleClick
    },

    __onMouseUp: function(e){
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
            this.event.originX = e.clientX
            this.event.originY = e.clientY

            for(let handler of this.activePointerHandlers){
                if(!handler || handler.__destroyed) continue
                if(typeof handler.__onMouseUp === 'function') handler.__onMouseUp(this.event)
            }
            
            // Qt has no separate click event: the release emits released and clicked
            let target = this.event.target
            if(target && !target.__destroyed) {
                JQApplication.beginUpdate()
                this.event.relative(target)
                if(typeof target.__onMouseUp === 'function') target.__onMouseUp(this.event)
                this.__finishMouseAreaDrag(target)
                JQApplication.endUpdate()
            }

            this.activePointerHandlers.clear()
            this.event = null
        }
    },

    __onPointerDown: function(e){
        if(!this.__isTouchPointer(e) || !e.isPrimary || this.__touchPointerId !== null) return

        this.__touchPointerId = e.pointerId
        this.__lastTouchTime = performance.now()

        // Keeps native focus and caret handling for text inputs.
        if(!this.__isEditableElement(e.target)) e.preventDefault()

        // No hover before the press for touch: enter happens with the press itself.
        this.__onMouseDown(e)
    },

    __onPointerMove: function(e){
        if(!this.__isTouchPointer(e) || e.pointerId !== this.__touchPointerId) return

        this.__lastTouchTime = performance.now()

        if(this.event) this.__onMouseMove(e)
    },

    __onPointerUp: function(e){
        if(!this.__isTouchPointer(e) || e.pointerId !== this.__touchPointerId) return

        this.__touchPointerId = null
        this.__lastTouchTime = performance.now()

        if(this.event) this.__onMouseMove(e)
        this.__onMouseUp(e)

        // No hover after a touch
        this.__clearHover(null)
    },

    __onPointerCancel: function(e){
        if(!this.__isTouchPointer(e) || e.pointerId !== this.__touchPointerId) return

        this.__touchPointerId = null
        this.__lastTouchTime = performance.now()

        this.__cancelPress()
        this.__clearHover(null)
    },

    init: function(){
        window.addEventListener('pointerdown', (e)=>{
            this.__onPointerDown(e)
        })
        window.addEventListener('pointermove', (e)=>{
            this.__onPointerMove(e)
        })
        window.addEventListener('pointerup', (e)=>{
            this.__onPointerUp(e)
        })
        window.addEventListener('pointercancel', (e)=>{
            this.__onPointerCancel(e)
        })
        window.addEventListener('mousemove', (e)=>{
            if(this.__isEmulatedMouseEvent()) return
            this.__onMouseMove(e)
        })
        window.addEventListener('dblclick', (e)=>{
            e.preventDefault()
        })
        window.addEventListener('mousedown', (e)=>{
            if(this.__isEmulatedMouseEvent()) return
            this.__onMouseDown(e)
        })
        window.addEventListener('mouseup', (e)=>{
            if(this.__isEmulatedMouseEvent()) return
            this.__onMouseUp(e)
        })
        window.addEventListener('contextmenu', (e)=>{
            // The press and the release of the right button are handled as any other
            e.preventDefault()
        })
        window.addEventListener('wheel', (e)=>{
            let event = new QmlWheelEvent()

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
            event.originX = e.clientX
            event.originY = e.clientY
            event.angleDelta.x = -e.deltaX / 8
            event.angleDelta.y = -e.deltaY / 8
            event.path = this.getObjectsFromPoint(e.clientX, e.clientY)

            for(let obj of event.path){
                if(!event.accepted || !event.target){
                    event.relative(obj)
                    if(typeof obj.__onWheel === 'function') obj.__onWheel(event)
                }
            }

        })
    },

    __composedSignals: {
        click: 'clicked',
        dblclick: 'doubleClicked',
        pressAndHold: 'pressAndHold',
    },

    // Qt: QQuickMouseAreaPrivate::propagate, every MouseArea at the point but the origin, topmost first
    __propagateComposedEvent: function(event, origin, kind){
        if(!event || !origin || origin.__destroyed) return
        if(!origin.propagateComposedEvents || event.accepted) return

        let originRect = origin.__DOM.getBoundingClientRect()
        let sceneX = originRect.left + event.x
        let sceneY = originRect.top + event.y
        let signalName = this.__composedSignals[kind]

        for(let obj of this.getObjectsFromPoint(sceneX, sceneY)){
            if(obj === origin || obj.__destroyed || typeof obj.__emitComposed !== 'function') continue

            let rect = obj.__DOM.getBoundingClientRect()
            event.x = sceneX - rect.left
            event.y = sceneY - rect.top
            if(obj.__emitComposed(event, signalName) && event.accepted) return
        }
    },
}