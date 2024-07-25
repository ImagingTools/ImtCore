module.exports = {
    timeStamp: 0,
    chain: [],

    init: function(){   
        // window.addEventListener('mouseover', (e)=>{
        //     if(e.target.qml) {
        //         e.target.qml.__onMouseOver(this.chain, e.pageX, e.pageY, e.button)
        //     }
        // })
        window.addEventListener('mouseout', (e)=>{
            if(e.target.qml) {
                e.target.qml.__onMouseOut(this.chain, e.pageX, e.pageY, e.button)
            }
        })
        window.addEventListener('mousemove', (e)=>{
            let elements = document.elementsFromPoint(e.pageX, e.pageY)
            for(el of elements){
                if(el.qml) {
                    el.qml.__onMouseMove(this.chain, e.pageX, e.pageY, e.button)
                }
            }
        })
        window.addEventListener('click', (e)=>{
            if(e.timeStamp - this.timeStamp > 300){
                this.timeStamp = e.timeStamp

                for(obj of this.chain){
                    obj.__onMouseClick(this.chain, e.pageX, e.pageY, e.button)
                }
            } else {
                for(obj of this.chain){
                    obj.__onMouseDblClick(this.chain, e.pageX, e.pageY, e.button)
                }
            }
            this.chain = []
        })
        window.addEventListener('dblclick', (e)=>{
            e.preventDefault()
        })
        window.addEventListener('mousedown', (e)=>{
            this.chain = []
            let elements = document.elementsFromPoint(e.pageX, e.pageY)
            for(el of elements){
                if(el.qml) {
                    if(el.qml.__onMouseDown(this.chain, e.pageX, e.pageY, e.button)) break
                }
            }
        })
        window.addEventListener('mouseup', (e)=>{
            for(obj of this.chain){
                obj.__onMouseUp(this.chain, e.pageX, e.pageY, e.button)
            }
        })
        window.addEventListener('contextmenu', (e)=>{
            e.preventDefault()
            if(e.timeStamp - this.timeStamp > 300){
                this.timeStamp = e.timeStamp

                for(obj of this.chain){
                    obj.__onMouseClick(this.chain, e.pageX, e.pageY, e.button)
                }
            } else {
                for(obj of this.chain){
                    obj.__onMouseDblClick(this.chain, e.pageX, e.pageY, e.button)
                }
            }
            this.chain = []
        })
        window.addEventListener('touchstart', (e)=>{
            // global.JQModules.QtQuick.Item.onMouseDown(e, e.changedTouches[0].pageX, e.changedTouches[0].pageY, 0)
        })
        window.addEventListener('touchend', (e)=>{
            // global.JQModules.QtQuick.Item.onMouseUp(e, e.changedTouches[0].pageX, e.changedTouches[0].pageY, 0)
        })
        window.addEventListener('touchmove', (e)=>{
            // global.JQModules.QtQuick.Item.onMouseMove(e, e.changedTouches[0].pageX, e.changedTouches[0].pageY)
        })
        window.addEventListener('wheel', (e)=>{
            let elements = document.elementsFromPoint(e.pageX, e.pageY)
            for(el of elements){
                if(el.qml) {
                    if(el.qml.__onWheel(this.chain, e.pageX, e.pageY)) break
                }
            }
        })
    }
}

