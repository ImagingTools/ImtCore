const path = require('path')

module.exports = {
    entry: "./core/core.js",
    output: {
        path: path.resolve(__dirname, 'core/dist'),
    },
    externals: {
        // three.js UMD bundle exposes the global as window.THREE (upper-case)
        'three': 'THREE',
        'three/examples/jsm/loaders/GLTFLoader.js': 'THREE_GLTFLoader',
    },
}