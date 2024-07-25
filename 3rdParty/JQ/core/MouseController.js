module.exports = {
    timeStamp: 0,
    pressed: [],
    entered: [],
    enteredWithoutPressed: [],

    getRelativeRect(el, absX, absY){
        let rect = el.getBoundingClientRect()
        return {
            x: absX - rect.x,
            y: absY - rect.y,
            width: rect.width,
            height: rect.height,
        }
    },

    init: function(){   
        window.addEventListener('mousemove', (e)=>{
            let elements = document.elementsFromPoint(e.pageX, e.pageY)

            for(el of elements){
                if(!el.qml) continue

                if(this.pressed.length === 0 || this.pressed.indexOf(el) >= 0) {
                    if(this.entered.indexOf(el) < 0){
                        this.entered.push(el)
                        el.qml.__onEntered()
                    }

                    let rect = this.getRelativeRect(el, e.pageX, e.pageY)
                    el.qml.__onPositionChanged(rect.x, rect.y)
                }
            }

            for(let el of this.pressed){
                if(elements.indexOf(el) < 0){
                    let rect = this.getRelativeRect(el, e.pageX, e.pageY)
                    el.qml.__onPositionChanged(rect.x, rect.y)
                }
            }

            let i = 0
            while(i < this.entered.length){
                if(elements.indexOf(this.entered[i]) < 0){
                    if(this.pressed.length === 0 || this.pressed.indexOf(this.entered[i]) >= 0) this.entered[i].qml.__onExited()
                    this.entered.splice(i, 1)
                } else {
                    i++
                }
            }
        })

        window.addEventListener('click', (e)=>{
            if(e.timeStamp - this.timeStamp > 300){
                this.timeStamp = e.timeStamp

                for(el of this.pressed){
                    let rect = this.getRelativeRect(el, e.pageX, e.pageY)
                    if(el.qml.__onClick(rect.x, rect.y, e.button)) break
                }
            } else {
                for(el of this.pressed){
                    let rect = this.getRelativeRect(el, e.pageX, e.pageY)
                    if(el.qml.__onDblClick(rect.x, rect.y, e.button)) break
                }
            }
            for(el of this.pressed){
                if(!el.qml) continue

                el.qml.__onExited()
            }

            let elements = document.elementsFromPoint(e.pageX, e.pageY)

            for(el of this.enteredWithoutPressed){
                if(!el.qml) continue

                if(elements.indexOf(el) < 0){
                    el.qml.__onExited()
                }
            }
            this.pressed = []
        })
        window.addEventListener('dblclick', (e)=>{
            e.preventDefault()
        })
        window.addEventListener('mousedown', (e)=>{
            this.pressed = []
            this.enteredWithoutPressed = this.entered.slice()
            let elements = document.elementsFromPoint(e.pageX, e.pageY)
            for(el of elements){
                if(!el.qml) continue

                let rect = this.getRelativeRect(el, e.pageX, e.pageY)
                if(el.qml.__onPressed(rect.x, rect.y, e.button)) {
                    this.pressed = [el]
                    break
                }

                this.pressed.push(el)
            }
        })
        window.addEventListener('mouseup', (e)=>{
            for(el of this.pressed){
                let rect = this.getRelativeRect(el, e.pageX, e.pageY)
                el.qml.__onReleased(rect.x, rect.y, e.button)
            }
        })
        window.addEventListener('contextmenu', (e)=>{
            e.preventDefault()
            if(e.timeStamp - this.timeStamp > 300){
                this.timeStamp = e.timeStamp

                for(el of this.pressed){
                    let rect = this.getRelativeRect(el, e.pageX, e.pageY)
                    if(el.qml.__onClick(rect.x, rect.y, e.button)) break
                }
            } else {
                for(el of this.pressed){
                    let rect = this.getRelativeRect(el, e.pageX, e.pageY)
                    if(el.qml.__onDblClick(rect.x, rect.y, e.button)) break
                }
            }
            for(el of this.pressed){
                if(!el.qml) continue

                el.qml.__onExited()
            }

            let elements = document.elementsFromPoint(e.pageX, e.pageY)

            for(el of this.enteredWithoutPressed){
                if(!el.qml) continue

                if(elements.indexOf(el) < 0){
                    el.qml.__onExited()
                }
            }
            this.pressed = []
        })
        // window.addEventListener('touchstart', (e)=>{

        // })
        // window.addEventListener('touchend', (e)=>{

        // })
        // window.addEventListener('touchmove', (e)=>{

        // })
        // window.addEventListener('wheel', (e)=>{

        // })
    }
}

