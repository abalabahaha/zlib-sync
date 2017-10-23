{
    "targets": [
        {
            "target_name": "zlib_sync",
            "dependencies": [
                "deps/binding.gyp:zlib",
            ],
            "sources": [
                "src/zlib_sync.cc",
            ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
            ],
            "xcode_settings": {
                "OTHER_CPLUSPLUSFLAGS": [
                    "-fPIC",
                    "-O3",
                ],
            },
            "cflags": [
                "-fPIC",
                "-O3",
            ],
        },
    ]
}
