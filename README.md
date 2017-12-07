zlib-sync
=========

Synchronous zlib bindings for NodeJS

Originally made for JS libraries interacting with the Discord gateway, where compression requires a shared zlib context in a synchronous fashion.

Features
--------

* Synchronous, [Pako](https://github.com/nodeca/pako)-like API

* Near-native performance and efficiency

Installing
----------

`zlib-sync` is a native module, so it requires `node-gyp` to be installed, which requires Python 2.7 and a C++ compiler on your system. See [the node-gyp documentation](https://github.com/nodejs/node-gyp#installation) for more info. A pre-built module is planned for a future update.

After getting `node-gyp` set up, install `zlib-sync` like a normal NPM package:

```
npm install zlib-sync
```

Usage
-------

Documentation can be found [here](https://github.com/abalabahaha/zlib-sync/wiki/Documentation).

```js
var ZlibSync = require("zlib-sync");

var inflate = new ZlibSync.Inflate();

inflate.push(compressedChunk1);
inflate.push(compressedChunk2, ZlibSync.Z_SYNC_FLUSH);

if(inflate.err < 0) {
    throw new Error("zlib error: " + inflate.msg);
}

var result = inflate.result;

console.log(result.toString());
```

To-Do
-----

* Deflate stream support
* One-time deflate/inflate support

License
-------

Refer to the [LICENSE](LICENSE) file.
