import {QtObject} from './QtObject'

export class RegExpValidator extends QtObject  {
    constructor(args) {
        super(args)

        this.$cP('regularExpression', undefined, this.$regularExpressionChanged)
        this.$cP('regExp', undefined, this.$regularExpressionChanged)
    }
    $domCreate(){}

    $regularExpressionChanged(){

    }
    validate(str){
        if(this.regularExpression){
            return this.regularExpression.test(str)
        } else if(this.regExp){
            return this.regExp.test(str)
        } else {
            return true
        }
    }
}

QML.RegExpValidator = RegExpValidator