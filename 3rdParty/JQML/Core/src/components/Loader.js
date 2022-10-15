import { Item } from './Item'
import { Signal } from '../utils/Signal'

export class Loader extends Item {
    static Null = 0
    static Ready = 1
    static Loading = 2
    static Error = 3

    $wAuto = true
    $hAuto = true

    constructor(parent) {
        super(parent)

        this.$cP('active', true).connect(this.$activeChanged.bind(this))
        this.$cP('status', Loader.Null).connect(this.$statusChanged.bind(this))
        this.$cP('progress', 0)
        this.$cP('item', undefined)
        this.$cP('asynchronous', false)
        this.$cP('source', '').connect(this.$sourceChanged.bind(this))
        this.$cP('sourceComponent', undefined).connect(this.$sourceComponentChanged.bind(this))

        this.$s.loaded = Signal()
        
    }

    $domCreate(){
        super.$domCreate()
    }

    $statusChanged(){
        if(this.status === Loader.Ready) this.$s.loaded()
    }
    $activeChanged(){
        if(!this.active) this.status = Loader.Null
    }
    $sourceComponentChanged(){
        this.status = Loader.Loading

        if(this.item) this.item.$destroy()
        this.children = []
        
        if(this.sourceComponent){
            try {

                this.sourceComponent.$create(Core)
                this.item = this.children[0]
            } catch (err) {
                this.item = undefined
                while(this.children.length > 0){
                    let child = this.children.pop()
                    child.$destroy()
                    Core.temp.lastAndRemove()
                }
            } finally {
                Core.temp.lastAndRemove()
            }
  
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
            // try {
                // let path = []
                //if(Core.hostPath !== '') path.push(Core.hostPath)
                //if(Core.rootPath !== '') path.push(Core.rootPath)
                // if(this.$basePath !== '') path.push(this.$basePath)
                // path.push(this.source)

                this.item = Core.cC(this.source, this)
                this.$p.width.depends.add(this.$p.item.val.$p.width.signal)
                this.$p.item.val.$p.width.signal.connections[this.$p.width.PID] = ()=>{
                    this.width = this.$p.item.val.$p.width.val
                }
                this.$p.height.depends.add(this.$p.item.val.$p.height.signal)
                this.$p.item.val.$p.height.signal.connections[this.$p.height.PID] = ()=>{
                    this.height = this.$p.item.val.$p.height.val
                }
                this.item.$uP()
            // } catch (err) {
            //     this.item = undefined
            //     while(this.children.length > 0){
            //         let child = this.children.pop()
            //         child.$destroy()
            //         Core.temp.lastAndRemove()
            //     }
            // } finally {
            // }
  
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
        this.$wAuto = false
    }
    $heightChanged(){
        super.$heightChanged()
        this.$hAuto = false
    }

    $updateGeometry(){
        if(this.$p.item.val && this.$p.item.val.$p){
            if(this.$wAuto){
                // this.width = this.$p.item.val.$p.width.val
                
                // this.$p.width.val = this.$p.item.val.$p.width.val
                // this.dom.style.width = `${this.$p.item.val.$p.width.val}px`
                // if(this.$p.x.func) this.$p.x.val = this.$p.x.func()
                // this.$xChanged()

                this.$wAuto = true
                // this.$p.width.val = this.item.width
                // this.dom.style.width = `${this.width}px`
            }
            if(this.$hAuto){
                // this.height = this.$p.item.val.$p.height.val
                
                // this.$p.height.val = this.$p.item.val.$p.height.val
                // this.dom.style.height = `${this.$p.item.val.$p.height.val}px`
                // if(this.$p.y.func) this.$p.y.val = this.$p.y.func()
                // this.$yChanged()

                this.$hAuto = true
                // this.$p.height.val = this.item.height
                // this.dom.style.height = `${this.height}px`
            }
            // this.width = this.item.width
            // this.height = this.item.height
        }

        super.$updateGeometry()
    }
}

QML.Loader = Loader