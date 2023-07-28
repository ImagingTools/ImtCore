import {QtObject} from './QtObject'

export class Gradient extends QtObject  {
    static Vertical = 0
    static Horizontal = 1

    constructor(args) {
        super(args)

        this.$cP('orientation', Gradient.Vertical, this.$gradientChanged)
        this.$cP('stops', [], this.$gradientChanged)

    }
    $domCreate(){}
    $gradientChanged(){
        this.parent.$gradientStyleChanged()
    }
}

QML.Gradient = Gradient