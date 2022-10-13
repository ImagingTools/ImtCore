import {Animation} from './Animation'

export class PropertyAnimation extends Animation {

    constructor(parent) {
        super(parent)

        this.$cP('duration', 250).connect(this.$durationChanged.bind(this))
        this.$cP('from', undefined).connect(this.$fromChanged.bind(this))
        this.$cP('to', undefined).connect(this.$toChanged.bind(this))
        this.$cP('property', '').connect(this.$propertyChanged.bind(this))
        this.$cP('properties', '').connect(this.$pChanged.bind(this))
        this.$cP('target', this.parent).connect(this.$targetChanged.bind(this))
    }

    $tick(){
        if(this.$p.target.val && this.$p.running.val){
            let properties = this.$p.properties.val.split(',')

            for(let prop of properties){
                if(this.$p.to.val === undefined) this.to = this.$p.target.val.$p[prop].val
                if(this.$p.from.val === undefined) this.from = this.$p.target.val.$p[prop].val
                
                let increment = (this.$p.to.val - this.$p.from.val) / (this.$p.duration.val / (1000/Core.FPS))

                this.$p.target.val[prop] += increment

                if(this.$p.to.val >= this.$p.from.val && this.$p.target.val.$p[prop].val >= this.$p.to.val) {
                    this.$p.target.val[prop] = this.$p.to.val
                    this.running = false
                    this.$s.finished()
                }
                if(this.$p.to.val <= this.$p.from.val && this.$p.target.val.$p[prop].val <= this.$p.to.val) {
                    this.$p.target.val[prop] = this.$p.to.val
                    this.running = false
                    this.$s.finished()
                }

            }
            this.$p.target.val.$updateRect()
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
    $pChanged(){

    }
    $propertyChanged(){
        this.properties = this.$p.property.val
    }
    $targetChanged(){

    }
    $targetsChanged(){

    }
    restart() { 
        let properties = this.$p.properties.val.split(',')
        for(let prop of properties){
            this.$p.target.val[prop] = this.$p.from.val
        }
		super.restart()
    }
}

QML.PropertyAnimation = PropertyAnimation