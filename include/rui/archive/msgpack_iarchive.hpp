#ifndef RUI_ARCHIVE_BC1117E149E045998830F967CE30D059
#define RUI_ARCHIVE_BC1117E149E045998830F967CE30D059

#include <cstring>
#include <string>
#include <boost/archive/archive_exception.hpp>
#include <boost/archive/detail/common_iarchive.hpp>
#include <boost/archive/detail/register_archive.hpp>
#include <msgpack.hpp>

namespace rui { namespace archive {

namespace archive = boost::archive;
namespace serialization = boost::serialization;

struct msgpack_iarchive
    : archive::detail::common_iarchive<msgpack_iarchive> {
    friend class boost::archive::load_access;
    friend class boost::archive::detail::interface_iarchive<msgpack_iarchive>;

    explicit
    msgpack_iarchive(std::istream& stream, int = 0)
    : stream_(stream) {}

    void load_binary(void* buffer, size_t size) {
        msgpack::unpacked unpacked;
        unpack(unpacked);

        msgpack::object const o = unpacked.get();
        if (o.type != msgpack::type::BIN || o.via.bin.size != size) {
            throw msgpack::type_error();
        }

        std::memcpy(buffer, o.via.bin.ptr, size);
    }

private:
    template <class T>
    void load_override(T& v, int) {
        archive::detail::common_iarchive<msgpack_iarchive>::load_override(v, 0);
    }

    // ignore.
    void load_override(archive::class_id_optional_type&, int) {}

    void load_override(boost::archive::class_name_type& v, int) {
        std::string s;
        load_<std::string>(s);

        if (s.size() > BOOST_SERIALIZATION_MAX_KEY_SIZE - 1) {
            serialization::throw_exception(
                archive::archive_exception(archive::archive_exception::invalid_class_name)
            );
        }

        *std::copy(s.begin(), s.end(), static_cast<char*>(v)) = 0;
    }

    void load_override(std::string& v, int) {
        load_<std::string>(v);
    }

    void load_override(std::wstring& v, int) {
        msgpack::unpacked unpacked;
        unpack(unpacked);

        msgpack::object const o = unpacked.get();
        if (o.type != msgpack::type::BIN || o.via.bin.size % sizeof(wchar_t) != 0) {
            throw msgpack::type_error();
        }

        v.assign(
            reinterpret_cast<wchar_t const*>(o.via.bin.ptr),
            reinterpret_cast<wchar_t const*>(o.via.bin.ptr + o.via.bin.size)
        );
    }

#define RUI_ARCHIVE_LOAD_AS(t, u)                                       \
    void load(t& v) {                                                   \
        load_<u>(v);                                                    \
    }

#define RUI_ARCHIVE_LOAD(t) RUI_ARCHIVE_LOAD_AS(t, t)

    RUI_ARCHIVE_LOAD_AS(char, int)
    RUI_ARCHIVE_LOAD_AS(signed char, int)
    RUI_ARCHIVE_LOAD_AS(unsigned char, int)
    RUI_ARCHIVE_LOAD_AS(wchar_t, int)
    RUI_ARCHIVE_LOAD(bool)
    RUI_ARCHIVE_LOAD(short)
    RUI_ARCHIVE_LOAD(unsigned short)
    RUI_ARCHIVE_LOAD(int)
    RUI_ARCHIVE_LOAD(unsigned int)
    RUI_ARCHIVE_LOAD(long)
    RUI_ARCHIVE_LOAD(unsigned long)
    RUI_ARCHIVE_LOAD(long long)
    RUI_ARCHIVE_LOAD(unsigned long long)
    RUI_ARCHIVE_LOAD(float)
    RUI_ARCHIVE_LOAD(double)

#undef RUI_ARCHIVE_LOAD
#undef RUI_ARCHIVE_LOAD_AS

    template <class T>
    void load(T& v) {
        load_binary(&v, sizeof(v));
    }

    template <class As, class T>
    void load_(T& v) {
        msgpack::unpacked unpacked;
        unpack(unpacked);
        v = unpacked.get().as<As>();
    }

    void unpack(msgpack::unpacked& unpacked) {
        while (!un_.next(&unpacked)) {
            un_.reserve_buffer(8 * 1024);
            stream_.read(un_.buffer(), un_.buffer_capacity());
            check_stream();
            un_.buffer_consumed(stream_.gcount());
        }
    }

    void check_stream() {
        if (stream_.fail() && !stream_.eof()
            /*|| un_.message_size() > 10 * 1024 * 1024*/) {
            serialization::throw_exception(
                archive::archive_exception(archive::archive_exception::input_stream_error)
            );
        }
    }

    std::istream& stream_;
    msgpack::unpacker un_;
};

} }

BOOST_SERIALIZATION_REGISTER_ARCHIVE(rui::archive::msgpack_iarchive)

#endif

