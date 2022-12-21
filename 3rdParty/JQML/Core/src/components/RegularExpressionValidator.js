import {QtObject} from './QtObject'

export class RegularExpressionValidator extends QtObject  {
    constructor(args) {
        super(args)

        this.$cP('regularExpression', undefined).connect(this.$regularExpressionChanged.bind(this))
        this.$cP('regExp', undefined).connect(this.$regularExpressionChanged.bind(this))
    }
    $domCreate(){}

    $regularExpressionChanged(){

    }
    validate(str){
        if(this.$p.regularExpression.val){
            return this.$p.regularExpression.val.test(str)
        } else if(this.$p.regExp.val){
            return this.$p.regExp.val.test(str)
        } else {
            return true
        }
    }
}

QML.RegularExpressionValidator = RegularExpressionValidator