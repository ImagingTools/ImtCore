import {Qt} from './utils/Qt'

global.Qt = Qt
global.QML = {}
require('./components/Component')
require('./components/QtObject')
require('./components/Item')
require('./components/MouseArea')
require('./components/Rectangle')
require('./components/Loader')
require('./components/ListElement')
require('./components/ListModel')
require('./components/DropShadow')
require('./components/Repeater')
require('./components/Image')
require('./components/Column')
require('./components/Row')
require('./components/Flickable')
require('./components/ListView')
require('./components/GridView')
require('./components/Text')
require('./components/Timer')
require('./components/Animation')
require('./components/PropertyAnimation')
require('./components/PauseAnimation')
require('./components/NumberAnimation')
require('./components/FontLoader')
require('./components/TextInput')
require('./components/TextEdit')
require('./components/IntValidator')
require('./components/DoubleValidator')
require('./components/RegularExpressionValidator')
require('./components/RegExpValidator')
require('./components/Href')
require('./components/FileDialog')
require('./components/Gradient')
require('./components/GradientStop')

QML.Qt = Qt
for(let key in Qt){
    QML[key] = Qt[key]
}

global.UID = 0
global.PID = 0
global.UIDList = {}
global.caller = null
global.ReadyList = []
// global.PropertyManager = {
//     list: [],
//     add: function(obj){
//         this.list.push(obj)
//     },
//     update: function(){
//         let i = 0
//         while(i < this.list.length){
//             try {
//                 caller = this.list[i]
//                 let val = caller.func()
//                 if(caller.val !== val){
//                     caller.val = val
//                     caller.signal()
//                 }
//                 caller = null
//                 this.list.splice(i, 1)
//             } catch (error) {
//                 i++
//             }
//         }
//         while(ReadyList.length){
//             ReadyList.pop().$s.completed()
//         }
//     },
//     remove: function(obj){
//         let i = 0
//         while(i < this.list.length){
//             if(this.list[i].obj === obj){
//                 this.list.splice(i, 1)
//             } else {
//                 i++
//             }
//         }
//     }
// }
global.IDManager = {
    list: {},
    set: function(obj, ID){
        if(this.list[ID]){
            if(this.list[ID].indexOf(obj) < 0) this.list[ID].push(obj)
        } else {
            this.list[ID] = [obj]
        }
    },
    get: function(obj, ID){
        if(this.list[ID]){
            let d = 9999999//Math.abs(obj.UID - this.list[ID][0].UID)
            let res = null//this.list[ID][0]
            if(this.list[ID].length === 1) return this.list[ID][0]
            for(let i = 0; i < this.list[ID].length; i++){
                if(Math.abs(obj.UID - this.list[ID][i].UID) < d){

                    let available = true
                    if(obj.LVL.size > this.list[ID][i].LVL.size){
                        for(let lvl of this.list[ID][i].LVL){
                            if(!obj.LVL.has(lvl)) available = false
                        }
                    } else {
                        for(let lvl of obj.LVL){
                            if(!this.list[ID][i].LVL.has(lvl)) available = false
                        }
                    }
                    if(available){
                        d = Math.abs(obj.UID - this.list[ID][i].UID)
                        res = this.list[ID][i]
                    }
                    
                }
            }
            return res
        } else {
            return undefined
        }
    },
    remove: function(obj){
        for(let ID of obj.ID){
            if(this.list[ID]){
                let pos = this.list[ID].indexOf(obj)
                if(pos >= 0) this.list[ID].splice(pos, 1)
            }
        }
    }

}
global.Core = {
    FPS: 60,
    exports: {},
    components: {...QML},
    animations: {},
    LVL: 0,
    cC: function(componentPath, parent, LVL){ // createComponent => CC
        if(componentPath[0] === '/') componentPath = componentPath.slice(1)
        let l = componentPath.lastIndexOf('/')
        let r = componentPath.indexOf('.qml')
        if(r < 0) r = componentPath.length
        let componentName = componentPath
        if(l >= 0 && r >= 0){
            componentName = componentPath.slice(l+1, r)
        } else {
            componentName = componentPath.replaceAll('.qml', '')
        }
        
        if(this.components[componentName]){
            let component = new this.components[componentName](parent)
            component.LVL.add(LVL)
            component.$domCreate()
            return component
        } else {
            let component = this.exports[componentName](parent)
            component.LVL.add(LVL)
            return component
        }
    },
    init: function(){
        console.time('build')
        

        history.pushState = (f => function pushState(){
            let ret = f.apply(this, arguments)
            window.dispatchEvent(new Event('pushState'))
            window.dispatchEvent(new Event('locationchange'))
            return ret
        })(history.pushState)
        history.replaceState = (f => function replaceState(){
            let ret = f.apply(this, arguments)
            window.dispatchEvent(new Event('replaceState'))
            window.dispatchEvent(new Event('locationchange'))
            return ret
        })(history.replaceState)
        window.addEventListener('popstate', ()=>{
            window.dispatchEvent(new Event('locationchange'))
        })

        document.head.insertAdjacentHTML("beforeend", `
        <style>
        *{padding:0;margin:0;border:0;overflow:hidden;-ms-overflow-style:none;scrollbar-width:none;}
        *::-webkit-scrollbar{width:0;height:0;}
        *,*:before,*:after{-moz-box-sizing:border-box;-webkit-box-sizing:border-box;box-sizing:border-box;}
        :focus,:active{outline:none;}
        a:focus,a:active{outline:none;}
        nav,footer,header,aside{display:block;}
        html,body{height:100%;width:100%;font-size:100%;line-height:1;font-size:14px;-ms-text-size-adjust:100%;-moz-text-size-adjust:100%;-webkit-text-size-adjust:100%;}
        input,button,textarea{font-family:inherit;}
        input::-ms-clear{display:none;}
        button{cursor:pointer;}
        button::-moz-focus-inner{padding:0;border:0;}
        a,a:visited{text-decoration:none;}
        a:hover{text-decoration:none;}
        ul li{list-style:none;}
        img{vertical-align:top;}
        h1,h2,h3,h4,h5,h6{font-size:inherit;font-weight:inherit;}
        </style>`)
        let root = this.cC('Item', null, this.LVL++)
        Core.root = root
        let dom = document.createElement("div")
        dom.style.width = '100%'
        dom.style.height = '100%'
        dom.style.position = 'absolute'
        dom.id = root.UID
        dom.classList.add('Item')
        document.body.appendChild(dom)
        root.$setDom(dom)

        root.eventState = {
            target: null,
            blocked: function(obj){
                this.target = obj
            },
            release: function(){
                this.target = null
            },
            view: null,
        }
        
        let events = ['mousedown', 'mouseup', 'mousemove', 'wheel', 'mousewheel', 'contextmenu', 'touchstart', 'touchend', 'touchmove', 'keydown', 'keyup', 'keypress', 'mouseover', 'mouseout']
		
		for(let event of events){
			root.dom.addEventListener(event, (e)=>{
                if(root.$readyEvents){

                
                    //e.preventDefault()

                    let path = [e. target]
                    let parent = e.target.parentNode
                    while(parent){
                        if(parent.id) path.push(parent)
                        parent = parent.parentNode
                    }
                    let find = false
                    root.eventState.view = null
                    for(let p of path){
                        let obj = UIDList[p.id.slice(3)]
                        if(obj && obj.webScroll !== undefined && !find){
                            find = true
                            root.eventState.view = obj
                        }
                        
                    }
                    if(root.eventState.target && root.eventState.target[`$${e.type}`]){
                        root.eventState.target[`$${e.type}`](e, root.eventState)
                    } else {
                        for(let p of path){
                            let obj = UIDList[p.id.slice(3)]
                            if(!root.eventState.target && obj && obj[`$${e.type}`]){
                                obj[`$${e.type}`](e, root.eventState)
                            }
                        }
                    }

                    if(e.type === 'mousewheel' || e.type === 'wheel'){
                        root.eventState.release()
                    }
                }
			})
		}
        this.hostPath = location.pathname.replaceAll(/\w+[.]html[/]*/g, '')
        if(this.hostPath[this.hostPath.length-1] === '/') this.hostPath = this.hostPath.slice(0, this.hostPath.length-1)
        let pos = document.body.dataset.qml.lastIndexOf('/')
        this.rootPath = pos >= 0 ? document.body.dataset.qml.slice(0, pos) : ''

        // let xhr = new XMLHttpRequest()
        // xhr.open('GET', [this.hostPath, 'fullImport.js'].join('/'), false)
        // try {
        //     xhr.send();
        //     if (xhr.status === 200) {
        //         eval(xhr.response)(this)
        //     } else {
                
        //     }
        // } catch(err) {
        //     console.log(err)
        // }
        // if(fullImport) fullImport()

        this.cC(document.body.dataset.qml, root, this.LVL++)
    

        setInterval(() => {
            for(let akey in this.animations){
                this.animations[akey].$tick()
            } 
            // for(let uid in UIDList){
            //     UIDList[uid].$updateRect()
            // } 
        }, 1000 / this.FPS)
        
        root.width = window.innerWidth
        root.height = window.innerHeight
        window.addEventListener('resize', ()=>{
            root.width = window.innerWidth
            root.height = window.innerHeight
        })

        root.$readyEvents = true
        root.$uP()
        // let timerFocus = null
        // setInterval(()=>{
        //     if(document.hasFocus()){
        //         if(!timerFocus && !root.$readyEvents) {
        //             timerFocus = setTimeout(()=>{
        //                 root.$readyEvents = true
        //             }, 200)
        //         }
        //     } else {
        //         root.$readyEvents = false
        //         if(timerFocus) {
        //             clearTimeout(timerFocus)
        //             timerFocus = null
        //         }
        //     }
        //     // console.log('hasFocus', root.$readyEvents)
        // }, 100)

        // document.body.addEventListener('mousemove', ()=>{
        //     root.$readyEvents = true
        // })
        
        console.timeEnd('build')
    }
}


window.onload = Core.init.bind(Core)