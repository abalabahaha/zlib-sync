{
    "targets": [
        {
            "target_name": "zlib",
            "type": "static_library",
            "include_dirs": [
                "config",
                "zlib",
            ],
            "direct_dependent_settings": {
                "include_dirs": [
                    "zlib",
                ],
            },
            "msvs_settings": {
                "VCCLCompilerTool": {
                    "RuntimeLibrary": 2,
                },
                "VCLinkerTool": {
                    "GenerateDebugInformation": "false",
                },
            },
            "xcode_settings": {
                "OTHER_CFLAGS": [
                    "-fPIC",
                    "-DPIC",
                    "-O3",
                    "-Wno-implicit-function-declaration",
                ],
            },
            "cflags": [
                "-fPIC",
                "-DPIC",
                "-O3",
                "-Wno-implicit-function-declaration",
            ],
            "sources": [
                "zlib/adler32.c",
                "zlib/compress.c",
                "zlib/crc32.c",
                "zlib/deflate.c",
                "zlib/gzclose.c",
                "zlib/gzlib.c",
                "zlib/gzread.c",
                "zlib/gzwrite.c",
                "zlib/infback.c",
                "zlib/inffast.c",
                "zlib/inflate.c",
                "zlib/inftrees.c",
                "zlib/trees.c",
                "zlib/uncompr.c",
                "zlib/zutil.c",
            ],
        },
    ],
}
