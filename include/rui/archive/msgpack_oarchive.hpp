#ifndef RUI_ARCHIVE_856D9559129E4262A93A162E5CCDB7FA
#define RUI_ARCHIVE_856D9559129E4262A93A162E5CCDB7FA

#include <cstring>
#include <string>
#include <boost/archive/archive_exception.hpp>
#include <boost/archive/detail/common_oarchive.hpp>
#include <boost/archive/detail/register_archive.hpp>
#include <msgpack.hpp>

namespace rui { namespace archive {

namespace archive = boost::archive;
namespace serialization = boost::serialization;

struct msgpack_oarchive
    : archive::detail::common_oarchive<msgpack_oarchive> {
    friend class boost::archive::save_access;
    friend class boost::archive::detail::interface_oarchive<msgpack_oarchive>;

    explicit
    msgpack_oarchive(std::ostream& stream, int = 0)
    : stream_(stream), packer_(stream) {}

    void save_binary(void const* buffer, size_t size) {
        packer_.pack_bin(size);
        packer_.pack_bin_body(static_cast<char const*>(buffer), size);
        check_stream();
    }

private:
    template <class T>
    void save_override(T& v, int) {
        archive::detail::common_oarchive<msgpack_oarchive>::save_override(v, 0);
    }

    // ignore.
    void save_override(archive::class_id_optional_type const&, int) {}

    void save_override(std::string const& v, int) {
        packer_ << v;
        check_stream();
    }

    void save_override(std::wstring const& v, int) {
        save_binary(v.c_str(), v.size() * sizeof(wchar_t));
    }

    void save_override(boost::archive::class_name_type const& v, int) {
        save_binary(v, std::strlen(v) * sizeof(char));
    }

#define RUI_ARCHIVE_SAVE(t)                                             \
    void save(t v) {                                                    \
        packer_ << v;                                                   \
        check_stream();                                                 \
    }

#define RUI_ARCHIVE_SAVE_AS(t, u)                                       \
    void save(t v) {                                                    \
        packer_ << static_cast<u>(v);                                   \
        check_stream();                                                 \
    }

    RUI_ARCHIVE_SAVE_AS(char, int)
    RUI_ARCHIVE_SAVE_AS(signed char, int)
    RUI_ARCHIVE_SAVE_AS(unsigned char, int)
    RUI_ARCHIVE_SAVE_AS(wchar_t, int)
    RUI_ARCHIVE_SAVE(bool)
    RUI_ARCHIVE_SAVE(short)
    RUI_ARCHIVE_SAVE(unsigned short)
    RUI_ARCHIVE_SAVE(int)
    RUI_ARCHIVE_SAVE(unsigned int)
    RUI_ARCHIVE_SAVE(long)
    RUI_ARCHIVE_SAVE(unsigned long)
    RUI_ARCHIVE_SAVE(long long)
    RUI_ARCHIVE_SAVE(unsigned long long)
    RUI_ARCHIVE_SAVE(float)
    RUI_ARCHIVE_SAVE(double)

#undef RUI_ARCHIVE_SAVE
#undef RUI_ARCHIVE_SAVE_AS

    template <class T>
    void save(T const& v) {
        save_binary(&v, sizeof(v));
    }

    void check_stream() {
        if (stream_.fail()) {
            serialization::throw_exception(
                archive::archive_exception(archive::archive_exception::output_stream_error)
            );
        }
    }

    std::ostream& stream_;
    msgpack::packer<std::ostream> packer_;
};

} }

BOOST_SERIALIZATION_REGISTER_ARCHIVE(rui::archive::msgpack_oarchive)

#endif

