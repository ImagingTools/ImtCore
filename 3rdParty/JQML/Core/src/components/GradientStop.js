import {QtObject} from './QtObject'

export class GradientStop extends QtObject  {
    constructor(args) {
        super(args)

        this.$cP('color', "'black'", this.$gradientChanged)
        this.$cP('position', 0, this.$gradientChanged)

        this.parent.stops.push(this)
        this.parent.stopsChanged()

    }
    $domCreate(){}
    $gradientChanged(){
        this.parent.$gradientChanged()
    }
    $destroy(){
        if(this.parent){
            let indx = this.parent.stops.indexOf(this)
            if(indx >= 0){
                this.parent.stops.splice(indx, 1)
                this.parent.stopsChanged()
            }
        }
        
        super.$destroy()
    }
}

QML.GradientStop = GradientStop