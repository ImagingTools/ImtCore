import {QtObject} from './QtObject'

export class Gradient extends QtObject  {
    static Vertical = 0
    static Horizontal = 1

    constructor(args) {
        super(args)

        this.$cP('orientation', Gradient.Vertical).connect(this.$gradientChanged.bind(this))
        this.$cP('stops', []).connect(this.$gradientChanged.bind(this))

    }
    $domCreate(){}
    $gradientChanged(){
        this.parent.$gradientStyleChanged()
    }
}

QML.Gradient = Gradient