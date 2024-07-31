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

}

class QmlWheelEvent {
    
}


module.exports = {
    timeStamp: 0,
    event: null,

    entered: [],
    hovered: [],

    getObjectsFromPoint: function(x, y){
        let result = []
        for(let el of document.elementsFromPoint(x, y)){
            if(!el.qml) continue
            result.push(el.qml)
        }
        return result
    },

    init: function(){   
        window.addEventListener('mousemove', (e)=>{
            let event = this.event ? this.event : new QmlMouseEvent({path: this.getObjectsFromPoint(e.pageX, e.pageY)})
            event.moveX = event.x - e.pageX
            event.moveY = event.y - e.pageY
            event.x = e.pageX
            event.y = e.pageY

            if(event.pressed && (Math.abs(event.startX - event.x) > 15 || Math.abs(event.startY - event.y) > 15)) {
                event.wasDrag = true
            }

            let i = 0
            while(i < this.entered.length){
                let point = event.getRelativePoint(this.entered[i])
                if(point.x < 0 || point.y < 0 || point.x >= this.entered[i].width || point.y >= this.entered[i].height) {
                    this.entered[i].__onMouseLeave(event)
                    this.entered.splice(i, 1)
                } else {
                    i++
                }
            }

            for(obj of event.path){
                let point = event.getRelativePoint(obj)
                if(point.x >= 0 && point.y >= 0 && point.x < obj.width && point.y < obj.height){
                    if(this.entered.indexOf(obj) < 0) {
                        this.entered.push(obj)
                        obj.__onMouseEnter(event)
                    }
                } 
                obj.__onMouseMove(event)
            }
        })

        window.addEventListener('click', (e)=>{
            this.click(e)
        })
        window.addEventListener('dblclick', (e)=>{
            e.preventDefault()
            this.click(e)
        })
        window.addEventListener('mousedown', (e)=>{
            this.event = new QmlMouseEvent({pressed: true})
            this.event.x = e.pageX
            this.event.y = e.pageY
            this.event.startX = e.pageX
            this.event.startY = e.pageY
            this.event.path = this.getObjectsFromPoint(e.pageX, e.pageY)

            for(obj of this.event.path){
                this.event.accepted = true
                obj.__onMouseDown(this.event)
            }
        })
        window.addEventListener('mouseup', (e)=>{
            this.event.x = e.pageX
            this.event.y = e.pageY
            
            if(this.event.target) this.event.target.__onMouseUp(this.event)
        })
        window.addEventListener('contextmenu', (e)=>{
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
            let elements = document.elementsFromPoint(e.pageX, e.pageY)

            for(el of elements){
                if(!el.qml) continue

                if(this.pressed.length === 0 || this.pressed.indexOf(el) >= 0) {
                    let rect = this.getRelativeRect(el, e.pageX, e.pageY)
                    if(el.qml.__onWheel(rect.x, rect.y, e.deltaX / 8, e.deltaY / 8)) break
                }
            }
        })
    },

    click: function(e){
        if(this.event && this.event.target){
            this.event.x = e.pageX
            this.event.y = e.pageY

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

