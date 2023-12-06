const { Item } = require('./Item')
const { QString, QReal, QVar, QBool } = require('../utils/properties')

class Loader extends Item {
    static Null = 0
    static Ready = 1
    static Loading = 2
    static Error = 3

    static defaultProperties = {
        status: { type: QReal, value: Loader.Null },
        progress: { type: QReal, value: 0 },
        asynchronous: { type: QBool, value: false },
        source: { type: QString, value: '', changed: '$sourceChanged' },
        sourceComponent: { type: QVar, changed: '$sourceComponentChanged' },
        item: { type: QVar },
    }

    static defaultSignals = {
        loaded: { params: [] },
    }

    $widthAuto = true
    $heightAuto = true

    constructor(parent,exCtx,exModel){
        super(parent,exCtx,exModel)
        this.$exCtx = exCtx
    }
    $complete(){
        // this.$widthAuto = this.getProperty('width').auto
        // this.$heightAuto = this.getProperty('height').auto

        this.$completed = true
        if(this.$lazy){
            this.$lazy()
            delete this.$lazy
        }
        // super.$complete()
        if(this.$signals['Component.completed']) this.$signals['Component.completed']()
    }

    $sourceComponentChanged(){
        // if(this.$widthAuto && this.getProperty('width').compute) this.$widthAuto = false
        // if(this.$heightAuto && this.getProperty('height').compute) this.$heightAuto = false
        // if(!this.$completed) {
        //     this.$lazy = this.getProperty('sourceComponent').getNotify()
        //     return
        // }

        this.getStatement('status').reset(Loader.Loading)

        if(this.getPropertyValue('item')) this.getPropertyValue('item').$destroy()
        
        if(this.getPropertyValue('sourceComponent')){
            let ctx = new ContextController(this.$exCtx, this.getStatement('sourceComponent').get().$exCtx)
            let item = this.getStatement('sourceComponent').get().createObject(this,ctx)

            // item.getProperty('x').getNotify().connect(()=>{
            //     this.getProperty('x').setAuto(item.getPropertyValue('x'))
            // })
            // item.getProperty('y').getNotify().connect(()=>{
            //     this.getProperty('y').setAuto(item.getPropertyValue('y'))
            // })
            if(item instanceof Item){
                item.getProperty('width').getNotify().connect(()=>{
                    this.getProperty('width').setAuto(item.getPropertyValue('width'))
                })
                item.getProperty('height').getNotify().connect(()=>{
                    this.getProperty('height').setAuto(item.getPropertyValue('height'))
                })
                // this.getProperty('x').setAuto(item.getPropertyValue('x'))
                // this.getProperty('y').setAuto(item.getPropertyValue('y'))
                this.getProperty('width').setAuto(item.getPropertyValue('width'))
                this.getProperty('height').setAuto(item.getPropertyValue('height'))
            }
            

            for(let update of updateList.splice(0, updateList.length)){
                update()
            }

            // if(!this.getProperty('x').auto) {
            //     item.getProperty('x').setCompute(()=>{
            //         item.getProperty('x').subscribe(this.getProperty('x'))
            //         return this.getPropertyValue('x')
            //     })
            //     item.getProperty('x').update()
            // }

            // if(!this.getProperty('y').auto) {
            //     item.getProperty('y').setCompute(()=>{
            //         item.getProperty('y').subscribe(this.getProperty('y'))
            //         return this.getPropertyValue('y')
            //     })
            //     item.getProperty('y').update()
            // }
            if(item instanceof Item){
                if(!this.$widthAuto) {
                    item.getProperty('width').setCompute(()=>{
                        item.getProperty('width').subscribe(this.getProperty('width'))
                        return this.getPropertyValue('width')
                    })
                    item.getProperty('width').update()
                }

                if(!this.$heightAuto) {
                    item.getProperty('height').setCompute(()=>{
                        item.getProperty('height').subscribe(this.getProperty('height'))
                        return this.getPropertyValue('height')
                    })
                    item.getProperty('height').update()
                }
            }

            item.$complete()
            this.getStatement('item').reset(item)

            if(this.getPropertyValue('item')){
                this.getStatement('status').reset(Loader.Ready)
            } else {
                this.getStatement('status').reset(Loader.Error)
            }

            if(this.$signals.loaded) this.$signals.loaded()
        } else {
            this.getStatement('item').reset(undefined)  
            this.getStatement('status').reset(Loader.Null)
        }
        
        
    }

    $sourceChanged(){
        // if(!this.$completed) {
        //     this.$lazy = this.getProperty('source').getNotify()
        //     return
        // }

        this.getStatement('status').reset(Loader.Loading)

        if(this.getPropertyValue('item')) this.getPropertyValue('item').$destroy()
        
        if(this.getPropertyValue('source')){
            let path = this.getStatement('source').get().replaceAll('/', '_').replaceAll('/', '_').replaceAll('.qml', '')
            let fullPath = path.indexOf('qrc:') >= 0 ? path.replaceAll('qrc:_qml_', '').split('_') : [].concat(this.$path ? this.$path.split('_') : [], path.split('_'))
            let dotIndex = fullPath.indexOf('..')
            while(dotIndex >= 0){
                if(dotIndex > 0){
                    fullPath.splice(dotIndex-1, 2)
                } else {
                    fullPath.splice(dotIndex, 2)
                }
                
                dotIndex = fullPath.indexOf('..')
            }

            let cls = eval(fullPath.join('_'))
            let item = new cls(this,this.$exCtx)

            // item.getProperty('x').getNotify().connect(()=>{
            //     this.getProperty('x').setAuto(item.getPropertyValue('x'))
            // })
            // item.getProperty('y').getNotify().connect(()=>{
            //     this.getProperty('y').setAuto(item.getPropertyValue('y'))
            // })
            if(item instanceof Item){
                item.getProperty('width').getNotify().connect(()=>{
                    this.getProperty('width').setAuto(item.getPropertyValue('width'))
                })
                item.getProperty('height').getNotify().connect(()=>{
                    this.getProperty('height').setAuto(item.getPropertyValue('height'))
                })
            }
            
            // this.getProperty('x').setAuto(item.getPropertyValue('x'))
            // this.getProperty('y').setAuto(item.getPropertyValue('y'))
            this.getProperty('width').setAuto(item.getPropertyValue('width'))
            this.getProperty('height').setAuto(item.getPropertyValue('height'))
            

            for(let update of updateList.splice(0, updateList.length)){
                update()
            }

            // if(!this.getProperty('x').auto) {
            //     item.getProperty('x').setCompute(()=>{
            //         item.getProperty('x').subscribe(this.getProperty('x'))
            //         return this.getPropertyValue('x')
            //     })
            //     item.getProperty('x').update()
            // }

            // if(!this.getProperty('y').auto) {
            //     item.getProperty('y').setCompute(()=>{
            //         item.getProperty('y').subscribe(this.getProperty('y'))
            //         return this.getPropertyValue('y')
            //     })
            //     item.getProperty('y').update()
            // }

            if(item instanceof Item){
                if(!this.$widthAuto) {
                    item.getProperty('width').setCompute(()=>{
                        item.getProperty('width').subscribe(this.getProperty('width'))
                        return this.getPropertyValue('width')
                    })
                    item.getProperty('width').update()
                }

                if(!this.$heightAuto) {
                    item.getProperty('height').setCompute(()=>{
                        item.getProperty('height').subscribe(this.getProperty('height'))
                        return this.getPropertyValue('height')
                    })
                    item.getProperty('height').update()
                }
            }

            item.$complete()
            this.getStatement('item').reset(item)

            if(this.getPropertyValue('item')){
                this.getStatement('status').reset(Loader.Ready)
            } else {
                this.getStatement('status').reset(Loader.Error)
            }

            if(this.$signals.loaded) this.$signals.loaded()
        } else {
            this.getStatement('item').reset(undefined)  
            this.getStatement('status').reset(Loader.Null)
        }
    }
}

module.exports.Loader = Loader