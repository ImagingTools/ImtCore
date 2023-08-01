import { Item } from './Item'
import { Signal } from '../utils/Signal'
import { ListView } from './ListView'
import { GridView } from './GridView'
import { Repeater } from './Repeater'

export class Loader extends Item {
    static Null = 0
    static Ready = 1
    static Loading = 2
    static Error = 3

    $xOverride = true
    $yOverride = true
    $widthOverride = true
    $heightOverride = true
    $clipOverride = true
    
    $overrideItemProperties = []

    constructor(args) {
        super(args)

        this.$cP('active', true, this.$activeChanged)
        this.$cP('status', Loader.Null, this.$statusChanged)
        this.$cP('progress', 0)
        this.$cP('item', undefined, this.$itemChanged)
        this.$cP('asynchronous', false)
        this.$cP('source', '', this.$sourceChanged)
        this.$cP('sourceComponent', undefined, this.$sourceComponentChanged)

        this.$sP('width', ()=>{return this.item ? this.item.width : 0})
        this.$sP('height', ()=>{return this.item ? this.item.height : 0})

        this.$cS('loaded')
    }

    $domCreate(){
        super.$domCreate()
    }

    $statusChanged(){
        if(this.status === Loader.Ready) this.loaded()
    }
    $activeChanged(){
        if(!this.active) this.status = Loader.Null
    }
    $itemChanged(){
        if(this.item){
            if(!this.$xOverride){
                if(this.item.$uL.properties.indexOf('x') < 0 && this.item.$uL.properties.indexOf('anchors.fill') < 0 && this.item.$uL.properties.indexOf('anchors.centerIn') < 0 && this.item.$uL.properties.indexOf('anchors.left') < 0 && this.item.$uL.properties.indexOf('anchors.right') < 0) 
                this.$xOverride = true
            }
            if(!this.$yOverride){
                if(this.item.$uL.properties.indexOf('y') < 0 && this.item.$uL.properties.indexOf('anchors.fill') < 0 && this.item.$uL.properties.indexOf('anchors.centerIn') < 0 && this.item.$uL.properties.indexOf('anchors.top') < 0 && this.item.$uL.properties.indexOf('anchors.bottom') < 0) 
                this.$yOverride = true
            }
            // if(!this.$widthOverride){
            //     if(this.item.$uL.properties.indexOf('width') < 0 && this.item.$uL.properties.indexOf('anchors.fill') < 0 && this.item.$uL.properties.indexOf('anchors.left') < 0 && this.item.$uL.properties.indexOf('anchors.right') < 0) 
            //     this.$widthOverride = true
            // }
            // if(!this.$heightOverride){
            //     if(this.item.$uL.properties.indexOf('height') < 0 && this.item.$uL.properties.indexOf('anchors.fill') < 0 && this.item.$uL.properties.indexOf('anchors.top') < 0 && this.item.$uL.properties.indexOf('anchors.bottom') < 0) 
            //     this.$heightOverride = true
            // }
            // if(!this.$clipOverride){
            //     if(this.item.$uL.properties.indexOf('clip') < 0) 
            //     this.$clipOverride = true
            // }
            this.$propertyOverride()

        }
    }
    $propertyOverride(){
        if(this.$xOverride){
            this.item.x = 0
            if(this.$p.x.func){
                caller = this.$p.x
                let x = this.$p.x.func()
                caller = null
                if(this.$p.x.val !== x){
                    this.$p.x.val = x
                    this.$p.x.getSignal()()
                }
            }
        } else {
            this.x = this.item.x
            // this.$p.x.depends.add(this.item.$p.x.signal)
            // this.item.$p.x.signal.connections[this.$p.x.PID] = ()=>{
            //     this.x = this.item.x
            // }
            // this.$p.x.depends.add(this.item.$p.x.signal)
            // this.item.$p.x.signal.connections[this.$p.x.PID] = ()=>{
            //     this.x = this.item.x
            // }
            this.$sP('x', ()=>{return this.item.x})
        }
        if(this.$yOverride){
            this.item.y = 0
            if(this.$p.y.func){
                caller = this.$p.y
                let y = this.$p.y.func()
                caller = null
                if(this.$p.y.val !== y){
                    this.$p.y.val = y
                    this.$p.y.getSignal()()
                }
            }
        } else {
            this.y = this.item.y
            // this.$p.y.depends.add(this.item.$p.y.signal)
            // this.item.$p.y.signal.connections[this.$p.y.PID] = ()=>{
            //     this.y = this.item.y
            // }
            this.$sP('y', ()=>{return this.item.y})
            // this.$p.y.depends.add(this.item.$p.y.signal)
            // this.item.$p.y.signal.connections[this.$p.y.PID] = ()=>{
            //     this.y = this.item.y
            // }
        }

        if(this.$widthOverride){
            this.item.width = this.width
            if(this.$p.width.func){
                caller = this.$p.width
                let width = this.$p.width.func()
                caller = null
                if(this.$p.width.val !== width){
                    this.$p.width.val = width
                    this.$p.width.getSignal()()
                }
            }
            this.item.$sP('width', ()=>{return this.width})
            // this.$p.width.signal.connections[this.item.$p.width.PID] = ()=>{
            //     this.$p.item.val.width = this.width
            // }
            // this.item.$p.width.depends.add(this.$p.width.signal)
            // let i = 0
            // while(i < this.item.$uL.properties.length){
            //     if(this.item.$uL.properties[i] === 'width') this.item.$uL.properties.splice(i, 1); else i++
            // }
        } else {
            this.width = this.item.width
            this.$sP('width', ()=>{return this.item.width})
            // this.$p.width.depends.add(this.item.$p.width.signal)
            // this.item.$p.width.signal.connections[this.$p.width.PID] = ()=>{
            //     this.width = this.item.width
            // }
        }
        if(this.$heightOverride){
           
            this.item.height = this.height
            if(this.$p.height.func){
                caller = this.$p.height
                let height = this.$p.height.func()
                caller = null
                if(this.$p.height.val !== height){
                    this.$p.height.val = height
                    this.$p.height.getSignal()()
                }
            }
            this.item.$sP('height', ()=>{return this.height})
            // this.$p.height.signal.connections[this.item.$p.height.PID] = ()=>{
            //     this.$p.item.val.height = this.height
            // }
            // this.item.$p.height.depends.add(this.$p.height.signal)
            // this.item.height = this.height
            // this.$p.height.signal.connections[this.item.$p.height.PID] = ()=>{
            //     this.$p.item.val.height = this.height
            // }
            // this.item.$p.height.depends.add(this.$p.height.signal)
            // let i = 0
            // while(i < this.item.$uL.properties.length){
            //     if(this.item.$uL.properties[i] === 'height') this.item.$uL.properties.splice(i, 1); else i++
            // }
        } else {
            
            this.height = this.item.height
            this.$sP('height', ()=>{return this.item.height})
            // this.$p.height.depends.add(this.item.$p.height.signal)
            // this.item.$p.height.signal.connections[this.$p.height.PID] = ()=>{
            //     this.height = this.item.height
            // }
            // this.$p.height.depends.add(this.item.$p.height.signal)
            // this.item.$p.height.signal.connections[this.$p.height.PID] = ()=>{
            //     this.height = this.item.height
            // }
        }
        if(this.$clipOverride){
            
            this.item.clip = this.clip
            if(this.$p.clip.func){
                caller = this.$p.clip
                let clip = this.$p.clip.func()
                caller = null
                if(this.$p.clip.val !== clip){
                    this.$p.clip.val = clip
                    this.$p.clip.getSignal()()
                }
            }
            this.item.$sP('clip', ()=>{return this.clip})
            // this.$p.clip.signal.connections[this.item.$p.clip.PID] = ()=>{
            //     this.$p.item.val.clip = this.clip
            // }
            // this.item.$p.clip.depends.add(this.$p.clip.signal)
            // this.item.clip = this.clip
            // this.$p.clip.signal.connections[this.item.$p.clip.PID] = ()=>{
            //     this.$p.item.val.clip = this.clip
            // }
            // this.item.$p.clip.depends.add(this.$p.clip.signal)
            // let i = 0
            // while(i < this.item.$uL.properties.length){
            //     if(this.item.$uL.properties[i] === 'clip') this.item.$uL.properties.splice(i, 1); else i++
            // }
        } else {
            this.clip = this.item.clip
            // this.$p.clip.depends.add(this.item.$p.clip.signal)
            // this.item.$p.clip.signal.connections[this.$p.clip.PID] = ()=>{
            //     this.clip = this.item.clip
            // }
            this.$sP('clip', ()=>{return this.item.clip})
            // this.$p.clip.depends.add(this.item.$p.clip.signal)
            // this.item.$p.clip.signal.connections[this.$p.clip.PID] = ()=>{
            //     this.clip = this.item.clip
            // }
        }
    }
    $sourceComponentChanged(){
        this.status = Loader.Loading

        if(this.item) this.item.$destroy()
        this.children = []
        
        if(this.sourceComponent){
            // this.$cP('index', this.parent.index)
            // this.index = this.parent.index
            this.$p.item.val = this.sourceComponent.createObject ? this.sourceComponent.createObject({parent: this, index: this.index}) : this.sourceComponent({parent: this, index: this.index})
            
            if(this.sourceComponent.parent) {
                this.item.parent2 = this.sourceComponent.parent
                this.item.$treeParent2 = this.sourceComponent.parent
            }

            if(this.index !== undefined){
                let childRecursive = (obj, index)=>{
                    if(obj.$qmlClassName !== 'ListElement'){
                        obj.$cP('index',index)
                        obj.index = index
                    }

                    if(!obj.$useModel && !obj.$repeater && obj.$qmlClassName !== 'ListElement')
                    for(let child of obj.children){ 
                        childRecursive(child, index)
                    }
                }
                childRecursive(this.item, this.index)
            }

            // if(Core.root.$completed){
                try {
                    this.item.$uP()
                } catch (error) {
                    console.error(error)
                }
            // }
            this.$p.item.getSignal()()
            if(this.$uL.properties.length) this.$uP()
            
        } else {
            this.item = undefined
            
        }
        
        if(this.item){
            this.status = Loader.Ready
        } else {
            this.status = Loader.Error
        }
    }
    $sourceChanged(){
        this.status = Loader.Loading

        if(this.item) this.item.$destroy()
        this.children = []
        
        if(this.source){
                this.$p.item.val = Core.cC(this.source, {parent: this, index: this.index})

                if(this.index !== undefined){
                    let childRecursive = (obj, index)=>{
                        if(obj.$qmlClassName !== 'ListElement'){
                            obj.$cP('index',index)
                            obj.index = index
                        }
                        if(!obj.$useModel && !obj.$repeater && obj.$qmlClassName !== 'ListElement')
                        for(let child of obj.children){
                            childRecursive(child, index)
                        }
                    }
                    childRecursive(this.item, this.index)
                }
                
                // if(Core.root.$completed){
                    try {
                        this.item.$uP()
                    } catch (error) {
                        console.error(error)
                    }
                    
                // }
                this.$p.item.getSignal()()
                if(this.$uL.properties.length) this.$uP()
  
        } else {
            this.item = undefined
        }
        
        if(this.item){
            this.status = Loader.Ready
        } else {
            this.status = Loader.Error
        }
        
    }

    $widthChanged(){
        super.$widthChanged()
        // this.$widthOverride = true
    }
    $heightChanged(){
        super.$heightChanged()
        // this.$heightOverride = true
    }
    $xChanged(){
        super.$xChanged()
        // this.$xOverride = true
    }
    $yChanged(){
        super.$yChanged()
        // this.$yOverride = true
    }
    $clipChanged(){
        super.$clipChanged()
        // this.$clipOverride = true
    }


}

QML.Loader = Loader