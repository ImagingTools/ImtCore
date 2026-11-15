const { defineType } = require('../Core/type')
const { ACCEPTABLE, INTERMEDIATE, INVALID, Validator } = require('./Validator')

// A regex that matches every prefix of the strings the original matches: each atom becomes (?:atom|$)
// (the transformation of the current runtime, core/JQApplication.js toPartialMatchRegex)
function partialSource(re){
    let source = re.source, i = 0
    function processGroup(){
        let result = ''
        let raw = (n) => { result += source.substr(i, n); i += n }
        let optional = (n) => { result += '(?:' + source.substr(i, n) + '|$)'; i += n }
        while(i < source.length){
            let c = source[i]
            if(c === '\\'){
                let n = source[i + 1]
                if(n === 'c') optional(3)
                else if(n === 'x') optional(4)
                else if(n === 'u') re.unicode && source[i + 2] === '{' ? optional(source.indexOf('}', i) - i + 1) : optional(re.unicode ? 6 : 2)
                else if((n === 'p' || n === 'P') && re.unicode) optional(source.indexOf('}', i) - i + 1)
                else if(n === 'k') optional(source.indexOf('>', i) - i + 1)
                else optional(2)
            } else if(c === '['){
                let m = /\[(?:\\.|[^\\])*?\]/g
                m.lastIndex = i
                optional(m.exec(source)[0].length)
            } else if('|^$*+?'.indexOf(c) >= 0){
                raw(1)
            } else if(c === '{'){
                let m = /\{\d+,?\d*\}/y
                m.lastIndex = i
                let found = m.exec(source)
                if(found) raw(found[0].length)
                else optional(1)
            } else if(c === '('){
                if(source[i + 1] === '?'){
                    let k = source[i + 2]
                    if(k === ':'){
                        result += '(?:'
                        i += 3
                        result += processGroup() + '|$)'
                    } else if(k === '='){
                        result += '(?='
                        i += 3
                        result += processGroup() + ')'
                    } else if(k === '!'){
                        let start = i
                        i += 3
                        processGroup()
                        result += source.substr(start, i - start)
                    } else if(k === '<' && (source[i + 3] === '=' || source[i + 3] === '!')){
                        let start = i
                        i += 4
                        processGroup()
                        result += source.substr(start, i - start)
                    } else {
                        raw(source.indexOf('>', i) - i + 1)
                        result += processGroup() + '|$)'
                    }
                } else {
                    raw(1)
                    result += processGroup() + '|$)'
                }
            } else if(c === ')'){
                i++
                return result
            } else {
                optional(1)
            }
        }
        return result
    }
    return processGroup()
}

// QRegularExpressionValidator::validate against the anchored pattern; an empty input is Intermediate
class RegularExpressionValidator extends Validator {
    $compile(){
        let re = this.$v[RegularExpressionValidator.REGULAREXPRESSION]
        this.$full = this.$partial = null
        if(!re) return
        if(!(re instanceof RegExp)){
            try { re = new RegExp(String(re)) } catch(error) { return }
        }
        if(!re.source || re.source === '(?:)') return
        let flags = re.flags.replace(/[gy]/g, '')
        this.$full = new RegExp('^(?:' + re.source + ')$', flags)
        try {
            this.$partial = new RegExp('^(?:' + partialSource(re) + ')$', flags)
        } catch(error) {
            this.$partial = null
        }
    }

    $validate(input){
        if(this.$full === undefined) this.$compile()
        if(!this.$full) return ACCEPTABLE
        input = String(input)
        if(this.$full.test(input)) return ACCEPTABLE
        if(input === '' || (this.$partial && this.$partial.test(input))) return INTERMEDIATE
        return INVALID
    }
}
const recompile = (validator) => validator.$compile()
defineType(RegularExpressionValidator, { props: { regularExpression: { kind: 'var', effect: recompile } } })

// Qt 5 RegExpValidator { regExp: ... }
class RegExpValidator extends RegularExpressionValidator {
    $compile(){
        let saved = this.$v[RegularExpressionValidator.REGULAREXPRESSION]
        this.$v[RegularExpressionValidator.REGULAREXPRESSION] = this.$v[RegExpValidator.REGEXP]
        try { super.$compile() } finally { this.$v[RegularExpressionValidator.REGULAREXPRESSION] = saved }
    }
}
defineType(RegExpValidator, { props: { regExp: { kind: 'var', effect: recompile } } })

module.exports = { partialSource, RegularExpressionValidator, RegExpValidator }
