const path = require('path')

module.exports = {
    entry: "./core/core.js",
    output: {
        path: path.resolve(__dirname, 'core/dist'),
    },
    externals: {
        'three': 'three',
        'three/examples/jsm/loaders/GLTFLoader.js': 'three/examples/jsm/loaders/GLTFLoader.js',
    },
}