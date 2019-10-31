declare module 'zlib-sync' {
    export interface InflateOptions {
        chunkSize?: number;
        to?: 'string';
        windowBits?: number;
    }

    export class Inflate {
        readonly windowBits: number;
        readonly result: Buffer | string | null;
        readonly msg: string | null;
        readonly err: number;
        readonly chunkSize: number;
        public constructor(options?: InflateOptions);
        public push(buffer: Buffer, flush?: boolean | number): void;
    }

    export const Z_NO_FLUSH: number;
    export const Z_PARTIAL_FLUSH: number;
    export const Z_SYNC_FLUSH: number;
    export const Z_FULL_FLUSH: number;
    export const Z_FINISH: number;
    export const Z_BLOCK: number;
    export const Z_TREES: number;
    export const Z_OK: number;
    export const Z_STREAM_END: number;
    export const Z_NEED_DICT: number;
    export const Z_ERRNO: number;
    export const Z_STREAM_ERROR: number;
    export const Z_DATA_ERROR: number;
    export const Z_MEM_ERROR: number;
    export const Z_BUF_ERROR: number;
    export const Z_VERSION_ERROR: number;
    export const Z_NO_COMPRESSION: number;
    export const Z_BEST_SPEED: number;
    export const Z_BEST_COMPRESSION: number;
    export const Z_DEFAULT_COMPRESSION: number;
    export const Z_FILTERED: number;
    export const Z_HUFFMAN_ONLY: number;
    export const Z_RLE: number;
    export const Z_FIXED: number;
    export const Z_DEFAULT_STRATEGY: number;
    export const Z_BINARY: number;
    export const Z_TEXT: number;
    export const Z_ASCII: number;
    export const Z_UNKNOWN: number;
    export const Z_DEFLATED: number;
    export const Z_NULL: number;

    export const ZLIB_VERSION: string;
}
