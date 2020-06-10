#include <algorithm>
#include <nan.h>
#include <node.h>
#include <vector>
#include <zlib.h>

using namespace node;
using namespace v8;

const char* zlib_get_error(int error_code) {
    switch(error_code) {
        case Z_STREAM_END:
            return "stream end";
        case Z_NEED_DICT:
            return "need dictionary";
        case Z_ERRNO:
            return "file error";
        case Z_STREAM_ERROR:
            return "stream error";
        case Z_DATA_ERROR:
            return "data error";
        case Z_MEM_ERROR:
            return "insufficient memory";
        case Z_BUF_ERROR:
            return "buffer error";
        case Z_VERSION_ERROR:
            return "incompatible version";
        default:
            return "unknown zlib error";
    }
}

class ZlibSyncInflate : public ObjectWrap {
    private:
        z_stream stream;
        int err;

        unsigned int chunk_size;
        bool to_string;
        int window_bits;

        std::vector<unsigned char> out_buffer;
        int total_out;
        char did_resize;

        int result_size;

    public:
        ZlibSyncInflate(unsigned int chunk_size, bool to_string, int window_bits):
        chunk_size(chunk_size), to_string(to_string), window_bits(window_bits),
        out_buffer(chunk_size) {
            err = did_resize = result_size = 0;

            stream.zalloc = Z_NULL;
            stream.zfree = Z_NULL;
            stream.opaque = Z_NULL;
            stream.avail_in = 0;
            stream.next_in = Z_NULL;
            stream.total_out = total_out = 0;
            int err = inflateInit2(&stream, window_bits);
            if(err != 0) {
                Nan::ThrowError(zlib_get_error(err));
            }
        }

        static NAN_METHOD(Push) {
            ZlibSyncInflate* self = ObjectWrap::Unwrap<ZlibSyncInflate>(info.This());
            Local<Object> buffer = Local<Object>::Cast(info[0]);
            int flush = Z_NO_FLUSH;
            if(info[1]->IsBoolean()) {
                if(Nan::To<bool>(info[1]).FromJust()) {
                    flush = Z_FINISH;
                }
            } else if(info[1]->IsNumber()) {
                flush = Nan::To<int32_t>(info[1]).FromJust();
            }

            z_stream* stream = &self->stream;

            stream->next_in = (Bytef*)Buffer::Data(buffer);
            stream->avail_in = Buffer::Length(buffer);
            stream->total_out = self->total_out;

            self->did_resize = (self->did_resize << 1) & 2;

            while(true) {
                stream->next_out = self->out_buffer.data() + stream->total_out;
                stream->avail_out = self->out_buffer.size() - stream->total_out;

                self->err = inflate(stream, flush);

                if((self->err == Z_OK || self->err == Z_BUF_ERROR) && stream->avail_in > 0) {
                    self->did_resize &= 1;
                    self->out_buffer.resize((int)self->out_buffer.size() + self->chunk_size);
                    continue;
                } else {
                    self->result_size = stream->total_out;
                    if(flush == Z_SYNC_FLUSH || flush == Z_FINISH) {
                        self->total_out = 0;
                    } else {
                        self->total_out = stream->total_out;
                    }
                    if(!self->did_resize && self->out_buffer.size() - self->chunk_size > self->chunk_size) {
                        self->out_buffer.resize(self->out_buffer.size() - self->chunk_size);
                    }
                    break;
                }
            }
        }

        static NAN_GETTER(GetChunkSize) {
            ZlibSyncInflate* self = ObjectWrap::Unwrap<ZlibSyncInflate>(info.This());

            info.GetReturnValue().Set(self->chunk_size);
        }

        static NAN_GETTER(GetErr) {
            ZlibSyncInflate* self = ObjectWrap::Unwrap<ZlibSyncInflate>(info.This());

            info.GetReturnValue().Set(self->err);
        }

        static NAN_GETTER(GetMsg) {
            ZlibSyncInflate* self = ObjectWrap::Unwrap<ZlibSyncInflate>(info.This());

            if(self->err == Z_OK) {
                info.GetReturnValue().Set(Nan::Null());
            } else {
                info.GetReturnValue().Set(Nan::New<String>(zlib_get_error(self->err)).ToLocalChecked());
            }
        }

        static NAN_GETTER(GetResult) {
            ZlibSyncInflate* self = ObjectWrap::Unwrap<ZlibSyncInflate>(info.This());

            if(self->err < Z_OK) {
                info.GetReturnValue().Set(Nan::Null());
            } else if(self->to_string) {
                info.GetReturnValue().Set(Nan::New<String>((char*)self->out_buffer.data(), self->result_size).ToLocalChecked());
            } else {
                info.GetReturnValue().Set(Nan::CopyBuffer((char*)self->out_buffer.data(), self->result_size).ToLocalChecked());
            }
        }

        static NAN_GETTER(GetWindowBits) {
            ZlibSyncInflate* self = ObjectWrap::Unwrap<ZlibSyncInflate>(info.This());

            info.GetReturnValue().Set(self->window_bits);
        }

        static NAN_METHOD(New) {
            if(!info.IsConstructCall()) {
                return Nan::ThrowTypeError("Use the new operator to construct a ZlibSyncInflate.");
            }
            unsigned int chunkSize = 16 * 1024;
            bool toString = false;
            int windowBits = 15;
            if(info.Length() >= 1 && info[0]->IsObject()) {
                Local<Object> options = Local<Object>::Cast(info[0]);

                Local<Value> val = Nan::Get(options, Nan::New<String>("chunkSize").ToLocalChecked()).ToLocalChecked();
                if(val->IsNumber()) {
                    chunkSize = Nan::To<uint32_t>(val).FromJust();
                }

                val = Nan::Get(options, Nan::New<String>("to").ToLocalChecked()).ToLocalChecked();
                if(val->IsString()) {
                    std::string to = *Nan::Utf8String(Nan::To<String>(val).ToLocalChecked());
                    if(to == "string") {
                        toString = true;
                    }
                }

                val = Nan::Get(options, Nan::New<String>("windowBits").ToLocalChecked()).ToLocalChecked();
                if(val->IsNumber()) {
                    windowBits = Nan::To<int32_t>(val).FromJust();
                }
            }

            ZlibSyncInflate* zlib_sync = new ZlibSyncInflate(chunkSize, toString, windowBits);

            zlib_sync->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
        }

        static NAN_MODULE_INIT(Init) {
            Nan::HandleScope scope;
            Local<Context> context = Nan::GetCurrentContext();
            Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
            tpl->SetClassName(Nan::New<String>("ZlibSyncInflate").ToLocalChecked());
            tpl->InstanceTemplate()->SetInternalFieldCount(1);

            Nan::SetPrototypeMethod(tpl, "push", Push);

            Local<ObjectTemplate> itpl = tpl->InstanceTemplate();
            Nan::SetAccessor(itpl, Nan::New<String>("chunkSize").ToLocalChecked(), GetChunkSize);
            Nan::SetAccessor(itpl, Nan::New<String>("err").ToLocalChecked(), GetErr);
            Nan::SetAccessor(itpl, Nan::New<String>("msg").ToLocalChecked(), GetMsg);
            Nan::SetAccessor(itpl, Nan::New<String>("result").ToLocalChecked(), GetResult);
            Nan::SetAccessor(itpl, Nan::New<String>("windowBits").ToLocalChecked(), GetWindowBits);

            target->Set(context, Nan::New<String>("Inflate").ToLocalChecked(), tpl->GetFunction(context).ToLocalChecked()).ToChecked();
        }
};


NAN_MODULE_INIT(AllInit) {
    ZlibSyncInflate::Init(target);

    #define EXPORT_CONST(prop) Nan::DefineOwnProperty(target, Nan::New<String>(#prop).ToLocalChecked(), Nan::New<Integer>(static_cast<int32_t>(prop)), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete))

    EXPORT_CONST(Z_NO_FLUSH);
    EXPORT_CONST(Z_PARTIAL_FLUSH);
    EXPORT_CONST(Z_SYNC_FLUSH);
    EXPORT_CONST(Z_FULL_FLUSH);
    EXPORT_CONST(Z_FINISH);
    EXPORT_CONST(Z_BLOCK);
    EXPORT_CONST(Z_TREES);

    EXPORT_CONST(Z_OK);
    EXPORT_CONST(Z_STREAM_END);
    EXPORT_CONST(Z_NEED_DICT);
    EXPORT_CONST(Z_ERRNO);
    EXPORT_CONST(Z_STREAM_ERROR);
    EXPORT_CONST(Z_DATA_ERROR);
    EXPORT_CONST(Z_MEM_ERROR);
    EXPORT_CONST(Z_BUF_ERROR);
    EXPORT_CONST(Z_VERSION_ERROR);

    EXPORT_CONST(Z_NO_COMPRESSION);
    EXPORT_CONST(Z_BEST_SPEED);
    EXPORT_CONST(Z_BEST_COMPRESSION);
    EXPORT_CONST(Z_DEFAULT_COMPRESSION);

    EXPORT_CONST(Z_FILTERED);
    EXPORT_CONST(Z_HUFFMAN_ONLY);
    EXPORT_CONST(Z_RLE);
    EXPORT_CONST(Z_FIXED);
    EXPORT_CONST(Z_DEFAULT_STRATEGY);

    EXPORT_CONST(Z_BINARY);
    EXPORT_CONST(Z_TEXT);
    EXPORT_CONST(Z_ASCII);
    EXPORT_CONST(Z_UNKNOWN);

    EXPORT_CONST(Z_DEFLATED);

    EXPORT_CONST(Z_NULL);

    Nan::DefineOwnProperty(target, Nan::New<String>("ZLIB_VERSION").ToLocalChecked(), Nan::New<String>(zlibVersion()).ToLocalChecked(), static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
}

NAN_MODULE_WORKER_ENABLED(zlib_sync, AllInit)
