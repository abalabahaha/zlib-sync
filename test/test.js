"use strict";

var assert = require("assert");
var fs = require("fs");
var path = require("path");
var Zlib = require("zlib");
var ZlibSync = require("../");

describe("ZlibSync", function() {
    var str = fs.readFileSync(path.join(__dirname, "loremipsum.txt"));
    var uncompressed = Buffer.alloc ? Buffer.from(str) : new Buffer(str);
    var compressed = Zlib.deflateSync(uncompressed);
    var compressedBad = compressed.slice(1);

    describe("Inflate", function() {
        describe("constructor", function() {
            it("should have a default windowBits and chunkSize", function() {
                var inflate = new ZlibSync.Inflate();
                assert.equal(16 * 1024, inflate.chunkSize);
                assert.equal(15, inflate.windowBits);
            });
            it("should accept options", function() {
                var inflate = new ZlibSync.Inflate({
                    chunkSize:  8 * 1024,
                    windowBits: 16
                });
                assert.equal(8 * 1024, inflate.chunkSize);
                assert.equal(16, inflate.windowBits);
            });
            it("should result in a buffer by default", function() {
                var inflate = new ZlibSync.Inflate();
                inflate.push(compressed, ZlibSync.Z_SYNC_FLUSH);
                assert.ok(inflate.result instanceof Buffer);
                assert.deepEqual(inflate.result, uncompressed);
            });
            it("should result in a string if { to: \"string\" } is passed", function() {
                var inflate = new ZlibSync.Inflate({
                    to: "string"
                });
                inflate.push(compressed, ZlibSync.Z_SYNC_FLUSH);
                assert.ok(typeof inflate.result === "string");
                assert.equal(inflate.result, uncompressed.toString());
            });
        });

        describe("#push()", function() {
            it("should result in a result on flush", function() {
                var inflate = new ZlibSync.Inflate();
                inflate.push(compressed, ZlibSync.Z_SYNC_FLUSH);
                assert.ok(inflate.err >= ZlibSync.Z_OK);
                assert.deepEqual(inflate.result, uncompressed);
            });
            it("should accumulate data until flush", function() {
                var inflate = new ZlibSync.Inflate();
                inflate.push(compressed.slice(0, ~~(compressed.length / 2)));
                assert.equal(inflate.err, ZlibSync.Z_OK);
                inflate.push(compressed.slice(~~(compressed.length / 2)), ZlibSync.Z_SYNC_FLUSH);
                assert.ok(inflate.err >= ZlibSync.Z_OK);
                assert.deepEqual(inflate.result, uncompressed);
            });
            it("should error on invalid data", function() {
                var inflate = new ZlibSync.Inflate();
                inflate.push(compressedBad, ZlibSync.Z_SYNC_FLUSH);
                assert.equal(inflate.err, ZlibSync.Z_DATA_ERROR);
                assert.notEqual(inflate.msg, null);
                assert.equal(inflate.result, null);
            });
        });
    });
});
