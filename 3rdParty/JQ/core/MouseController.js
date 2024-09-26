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
    wasDrag = false

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
        let rect = obj.__DOM.getBoundingClientRect()
        this.x = this.originX - rect.x
        this.y = this.originY - rect.y
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

    accepted = true
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

    timeStamp: 0,
    event: null,

    entered: [],
    hovered: [],

    add: function(obj){
        this.objects.add(obj)
    },
    remove: function(obj){
        this.objects.delete(obj)
        if(this.event){
            let index = this.event.path.indexOf(obj)
            while(index >= 0){
                this.event.path.splice(index, 1)
                index = this.event.path.indexOf(obj)
            }
        }
    },

    getObjectsFromPoint: function(x, y){
        let result = []

        for(let obj of this.objects){
            let dom = obj.__getDOM()
            dom.classList.add("pointer")
        }

        for(let el of document.elementsFromPoint(x, y)){
            if(this.objects.has(el.qml) && !el.qml.__destroyed){
                result.push(el.qml)
            }
            
        }

        for(let obj of this.objects){
            let dom = obj.__getDOM()
            dom.classList.remove("pointer")
        }

        return result
    },

    init: function(){   
        window.addEventListener('mousemove', (e)=>{
            let event = this.event ? this.event : new QmlMouseEvent({path: this.getObjectsFromPoint(e.pageX, e.pageY)})
            event.moveX = event.originX - e.pageX
            event.moveY = event.originY - e.pageY
            event.originX = e.pageX
            event.originY = e.pageY

            // if(!event.target) JQApplication.setCursor(QtEnums.ArrowCursor)

            if(event.pressed && (Math.abs(event.startX - event.originX) > 15 || Math.abs(event.startY - event.originY) > 15)) {
                event.wasDrag = true
            }

            let i = 0
            while(i < this.entered.length){
                if(this.entered[i].__destroyed) {
                    this.entered.splice(i, 1)
                    continue
                }

                event.relative(this.entered[i])
                if(event.x < 0 || event.y < 0 || event.x >= this.entered[i].width || event.y >= this.entered[i].height) {
                    this.entered[i].__onMouseLeave(event)
                    this.entered.splice(i, 1)
                } else {
                    i++
                }
            }

            i = 0
            while(i < event.path.length){
                if(event.path[i].__destroyed) {
                    event.path.splice(i, 1)
                    continue
                }

                event.relative(event.path[i])
                if(event.x >= 0 && event.y >= 0 && event.x < event.path[i].width && event.y < event.path[i].height){
                    if(this.entered.indexOf(event.path[i]) < 0) {
                        this.entered.push(event.path[i])
                        event.path[i].__onMouseEnter(event)
                    }
                } 
                event.path[i].__onMouseMove(event)

                i++
            }
        })

        window.addEventListener('click', (e)=>{
            if(this.event) this.event.fillButton(e)
            this.click(e)
        })
        window.addEventListener('dblclick', (e)=>{
            if(this.event) this.event.fillButton(e)
            e.preventDefault()
            this.click(e)
        })
        window.addEventListener('mousedown', (e)=>{
            this.event = new QmlMouseEvent({pressed: true})
            this.event.fillButton(e)
            this.event.originX = e.pageX
            this.event.originY = e.pageY
            this.event.startX = e.pageX
            this.event.startY = e.pageY
            this.event.path = this.getObjectsFromPoint(e.pageX, e.pageY)

            for(obj of this.event.path){
                this.event.accepted = true
                this.event.relative(obj)
                obj.__onMouseDown(this.event) 
            }  
        })
        window.addEventListener('mouseup', (e)=>{
            if(this.event){
                this.event.fillButton(e)
                this.event.originX = e.pageX
                this.event.originY = e.pageY
                
                if(this.event.target) {
                    this.event.relative(obj)
                    this.event.target.__onMouseUp(this.event)
                }
            }
        })
        window.addEventListener('contextmenu', (e)=>{
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
            this.event.originX = e.pageX
            this.event.originY = e.pageY
            this.event.angleDelta.x = e.deltaX / 8
            this.event.angleDelta.y = e.deltaY / 8
            this.event.path = this.getObjectsFromPoint(e.pageX, e.pageY)

            for(obj of this.event.path){
                this.event.target = obj
                this.event.relative(obj)
                obj.__onWheel(this.event)
            }

            this.event = null
        })
    },

    click: function(e){
        if(this.event && this.event.target){
            this.event.originX = e.pageX
            this.event.originY = e.pageY
            this.event.relative(this.event.target)
            if(e.timeStamp - this.timeStamp > 300){
                this.timeStamp = e.timeStamp
                this.event.target.__onMouseClick(this.event)
            } else {
                this.event.target.__onMouseDblClick(this.event)
            }
            
            this.event.target.__onMouseLeave(this.event)
        }

        this.event = null
    },
}

