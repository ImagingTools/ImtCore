import {QtObject} from './QtObject'

export class DoubleValidator extends QtObject  {
    static StandardNotation = 0
    static ScientificNotation = 1

    constructor(parent) {
        super(parent)

        this.$cP('bottom', -Infinity)
        this.$cP('top', Infinity)
        this.$cP('locale', '')
        this.$cP('decimals', 1000)
        this.$cP('notation', DoubleValidator.ScientificNotation)

        this.$standardRegExp = /^(-|\+)?\s*[0-9]+(\.[0-9]+)?$/
        this.$scientificRegExp = /^(-|\+)?\s*[0-9]+(\.[0-9]+)?(E(-|\+)?[0-9]+)?$/
    }
    $domCreate(){}

    $getRegExpForNotation() {
        switch (this.notation) {
          case DoubleValidator.ScientificNotation:
            return this.$scientificRegExp;
          case DoubleValidator.StandardNotation:
            return this.$standardRegExp;
        }
        return null;
      }
    $getDecimalsForNumber(number) {
        if (Math.round(number) === number) {
            return 0
        }
        const str = `${number}`
        return /\d*$/.exec(str)[0].length
    }
    validate(str) {
        const regExp = this.$getRegExpForNotation()
        if (!regExp.test(str.trim())) {
            return false
        }
        const value = parseFloat(str)
        const acceptable = this.bottom <= value && this.top >= value && this.$getDecimalsForNumber(value) <= this.decimals
        return acceptable
    }
}

QML.DoubleValidator = DoubleValidator