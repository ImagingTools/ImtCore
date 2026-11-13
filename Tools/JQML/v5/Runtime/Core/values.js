// Value kinds of QML properties: default value and coercion on write.
// Coercion rules are the ones of the current runtime (QtQml/Int.js, Real.js, Bool.js, String.js).

const kinds = {
    // QQmlScriptString: { fn, scope } made by the compiler, run on demand
    script: {
        initial: null,
        coerce: (value) => value,
    },
    int: {
        initial: 0,
        coerce(value){
            if(value === undefined) throw 'Cannot assign [undefined] to int'
            if(value === null) throw 'Cannot assign std::nullptr_t to int'
            if(typeof value === 'number'){
                if(value !== value || value === Infinity || value === -Infinity) return -2147483648
                return Math.trunc(value)
            }
            if(value === true) return 1
            if(value === false) return 0
            if(typeof value === 'string'){
                let number = Number.parseInt(value)
                if(number !== number) throw 'Cannot assign QString to int'
                return number
            }
            throw 'Cannot assign QJSValue to int'
        },
    },
    real: {
        initial: 0,
        coerce(value){
            if(typeof value === 'number') return value
            if(value === undefined) throw 'Cannot assign [undefined] to double'
            if(value === null || value === false || value === '') return 0
            if(value === true) return 1
            if(typeof value === 'string'){
                let number = Number.parseFloat(value)
                if(number === Infinity || number === -Infinity) throw 'Cannot assign QString to double'
                return number
            }
            throw 'Cannot assign QJSValue to double'
        },
    },
    bool: {
        initial: false,
        coerce(value){
            if(value === undefined) throw 'Cannot assign [undefined] to Bool'
            return !!value
        },
    },
    string: {
        initial: '',
        coerce(value){
            if(typeof value === 'string') return value
            if(value === undefined) throw 'Cannot assign [undefined] to String'
            if(value === null) return ''
            if(typeof value === 'number'){
                if(value !== value) return 'nan'
                if(value === Infinity) return 'inf'
                if(value === -Infinity) return '-inf'
                return String(value)
            }
            if(typeof value === 'boolean') return String(value)
            throw 'Cannot assign JSValue to String'
        },
    },
    color: {
        initial: '#000000',
        coerce: require('./color').coerceColor,
    },
    var: {
        initial: undefined,
        coerce: null,
    },
    // holds a Component; the compiler wraps an element assigned to it instead of creating it
    component: {
        initial: null,
        coerce: null,
    },
}

module.exports = kinds
