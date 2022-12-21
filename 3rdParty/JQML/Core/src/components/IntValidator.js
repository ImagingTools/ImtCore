import {QtObject} from './QtObject'

export class IntValidator extends QtObject  {
    constructor(args) {
        super(args)

        this.$cP('bottom', -2147483647)
        this.$cP('top', 2147483647)
        this.$cP('locale', '')
    }
    $domCreate(){}


    validate(str){
        const regExp = /^(-|\+)?\s*[0-9]+$/;

        if (regExp.test(str.trim())) {
            const value = parseInt(str, 10);
            return this.$p.bottom.val <= value && this.$p.top.val >= value
        }
            return false;
        }
}

QML.IntValidator = IntValidator