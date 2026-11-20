// uglify-js 3.19 with keep_fnames (the runtime reads the names of functions and classes): AST_Symbol.mark_enclosed adds
// every function of a scope to the enclosed list of the definition's scope, each time a name is first enclosed in it,
// and push_uniq looks for each in the list with indexOf. The compiled application is one scope with thousands of
// functions and names, so the mangler spent most of the build there (O(names x functions), 8 of the 14 s of ProLife).
// The same method with the same lists in the same order: membership is kept in a Set beside each list (built again
// when the list changed elsewhere), and the functions of a scope are not walked again into a list that has them all.
// The output is the same, byte for byte (tests/unit/Compiler.test.js).
let installed = false

function install(U){
    if(installed) return
    installed = true
    const sets = new WeakMap()
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
    // scope -> (enclosed list -> the number of functions of the scope all in that list)
    const walked = new WeakMap()
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
    markEnclosed.$jqml = true
    markEnclosed.$original = U.AST_Symbol.prototype.mark_enclosed
    U.AST_Symbol.prototype.mark_enclosed = markEnclosed
}

module.exports = { install }
