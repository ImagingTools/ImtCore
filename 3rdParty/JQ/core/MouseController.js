module.exports = {
    timeStamp: 0,
    chain: [],

    init: function(){   
        window.addEventListener('mousemove', (e)=>{
            // global.JQModules.QtQuick.Item.onMouseMove(e, e.pageX, e.pageY)
        })
        window.addEventListener('click', (e)=>{
            if(e.timeStamp - this.timeStamp > 300){
                this.timeStamp = e.timeStamp

                for(obj of this.chain){
                    obj.onMouseClick(this.chain, e.pageX, e.pageY, e.button)
                }
            }
        })
        window.addEventListener('dblclick', (e)=>{
            for(obj of this.chain){
                obj.onMouseDblClick(this.chain, e.pageX, e.pageY, e.button)
            }
        })
        window.addEventListener('mousedown', (e)=>{
            this.chain = []
            let elements = document.elementsFromPoint(e.pageX, e.pageY)
            for(el of elements){
                if(el.qml) {
                    if(el.qml.onMouseDown(this.chain, e.pageX, e.pageY, e.button)) break
                }
            }
        })
        window.addEventListener('mouseup', (e)=>{
            // let elements = document.elementsFromPoint(e.pageX, e.pageY)
            for(obj of this.chain){
                obj.onMouseUp(this.chain, e.pageX, e.pageY, e.button)
            }
        })
        window.addEventListener('contextmenu', (e)=>{
            // global.JQModules.QtQuick.Item.onMouseClick(e, e.pageX, e.pageY)
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
            // global.JQModules.QtQuick.Item.onWheel(e, e.pageX, e.pageY, e.deltaX, e.deltaY)
        })
    }
}

