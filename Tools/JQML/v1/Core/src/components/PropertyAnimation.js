import {Animation} from './Animation'

export class PropertyAnimation extends Animation {

    constructor(args) {
        super(args)

        this.$cP('duration', 250, this.$durationChanged)
        this.$cP('from', undefined, this.$fromChanged)
        this.$cP('to', undefined, this.$toChanged)
        this.$cP('property', '', this.$propertyChanged)
        this.$cP('properties', '', this.$propertiesChanged)
        this.$cP('target', this.parent, this.$targetChanged)
    }

    $tick(){
        if(this.$completed && this.target && this.running){
            let properties = this.properties.split(',')

            for(let prop of properties){
                if(this.to === undefined) this.to = this.target[prop]
                if(this.from === undefined) this.from = this.target[prop]
                
                let increment = (this.to - this.from) / (this.duration / (1000/Core.FPS))

                // this.target[prop] += increment
                this.target.$p[prop].val += increment
                // this.target.$p[prop].getSignal()()
                

                if(this.to >= this.from && this.target[prop] >= this.to) {
                    this.target.$p[prop].val = this.to
                    let loops = this.$loops + 1
                    if(loops < this.loops || this.loops === Animation.Infinite){
                        this.restart()
                        this.$loops = loops
                    } else {
                        this.running = false
                        this.stopped()
                        this.finished()
                    }
                }
                if(this.to <= this.from && this.target[prop] <= this.to) {
                    this.target.$p[prop].val = this.to
                    let loops = this.$loops + 1
                    if(loops < this.loops || this.loops === Animation.Infinite){
                        this.restart()
                        this.$loops = loops
                    } else {
                        this.running = false
                        this.stopped()
                        this.finished()
                    }
                }
                this.target.$p[prop].getSignal()()

            }
            this.target.$updateRect()
        }
    }

    initProperties(meta, on){
        // super.initProperties(meta, on)
        // if(on !== undefined){
        //     this.__property = on
        // }
    }

    $durationChanged(){

    }
    $fromChanged(){

    }
    $toChanged(){

    }
    $propertiesChanged(){

    }
    $propertyChanged(){
        this.properties = this.property
    }
    $targetChanged(){

    }
    $targetsChanged(){

    }
    restart() { 
        let properties = this.properties.split(',')
        for(let prop of properties){
            if(this.target.$p[prop].val !== this.from){
                this.target.$p[prop].val = this.from
                this.target.$p[prop].getSignal()()
            }
            
        }
		super.restart()
    }
}

QML.PropertyAnimation = PropertyAnimation