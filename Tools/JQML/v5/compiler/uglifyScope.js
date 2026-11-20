// uglify-js 3.19 with keep_fnames (the runtime reads the names of functions and classes): AST_Symbol.mark_enclosed adds
// every function of a scope to the enclosed list of the definition's scope, each time a name is first enclosed in it,
// and push_uniq looks for each in the list with indexOf. The compiled application is one scope with thousands of
// functions and names, so the mangler spent most of the build there (O(names x functions), 8 of the 14 s of ProLife).
// The same method with the same lists in the same order: membership is kept in a Set beside each list (built again
// when the list changed elsewhere), and the functions of a scope are not walked again into a list that has them all.
// The output is the same, byte for byte (tests/unit/Compiler.test.js, tests/stress/Compiler.test.js).

// the Sets beside the lists and the functions walked, made for one minify
let sets = new WeakMap()
let walked = new WeakMap()

// U.minify(code, options) with the method replaced for its time only: a method replaced on the prototype for good made
// the rest of uglify (the parser the code generator uses before) a few percent slower
function minify(U, code, options){
    let original = U.AST_Symbol.prototype.mark_enclosed
    U.AST_Symbol.prototype.mark_enclosed = markEnclosedOf(U)
    sets = new WeakMap()
    walked = new WeakMap()
    try {
        return U.minify(code, options)
    } finally {
        U.AST_Symbol.prototype.mark_enclosed = original
        sets = new WeakMap()
        walked = new WeakMap()
    }
}

function markEnclosedOf(U){
    function pushUniq(array, el){
        let set = sets.get(array)
        if(!set || set.size !== array.length){
            set = new Set(array)
            sets.set(array, set)
        }
        if(set.has(el)) return undefined
        set.add(el)
        return array.push(el)
    }
    // walked: scope -> (enclosed list -> the number of functions of the scope all in that list)
    function addFunctions(s, target){
        let done = walked.get(s)
        if(!done){
            done = new WeakMap()
            walked.set(s, done)
        }
        let count = s.functions.size()
        if(done.get(target) === count && sets.has(target) && sets.get(target).size === target.length) return
        s.functions.each((d) => { pushUniq(target, d) })
        done.set(target, count)
    }
    const markEnclosed = function(options){
        let def = this.definition()
        for(let s = this.scope; s; s = s.parent_scope){
            if(!pushUniq(s.enclosed, def)) break
            if(!options){
                s._var_names = undefined
            } else {
                if(options.keep_fargs && s instanceof U.AST_Lambda) s.each_argname((arg) => { pushUniq(def.scope.enclosed, arg.definition()) })
                if(options.keep_fnames) addFunctions(s, def.scope.enclosed)
            }
            if(s === def.scope) break
        }
    }
    return markEnclosed
}

module.exports = { minify, markEnclosedOf }
