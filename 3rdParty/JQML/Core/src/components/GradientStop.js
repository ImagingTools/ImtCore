import {QtObject} from './QtObject'

export class GradientStop extends QtObject  {
    constructor(parent) {
        super(parent)

        this.$cP('color', "'black'").connect(this.$gradientChanged.bind(this))
        this.$cP('position', 0).connect(this.$gradientChanged.bind(this))

        this.parent.$p.stops.val.push(this)
        this.parent.$s.stopsChanged()

    }
    $domCreate(){}
    $gradientChanged(){
        this.parent.$gradientChanged()
    }
    $destroy(){
        let indx = this.parent.$p.stops.val.indexOf(this)
        if(indx >= 0){
            this.parent.$p.stops.val.splice(indx, 1)
            this.parent.$s.stopsChanged()
        }
        super.$destroy()
    }
}

QML.GradientStop = GradientStop