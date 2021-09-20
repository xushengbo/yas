
// Copyright (c) 2010-2021 niXman (github dot nixman at pm dot me). All
// rights reserved.
//
// This file is part of YAS(https://github.com/niXman/yas) project.
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//
//
// Boost Software License - Version 1.0 - August 17th, 2003
//
// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:
//
// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#ifndef __yas__types__utility__buffer_hpp
#define __yas__types__utility__buffer_hpp

#include <yas/detail/type_traits/type_traits.hpp>
#include <yas/detail/type_traits/serializer.hpp>
#include <yas/detail/tools/base64.hpp>
#include <yas/detail/io/serialization_exceptions.hpp>

#include <yas/object.hpp>
#include <yas/buffers.hpp>

namespace yas {
namespace detail {

/***************************************************************************/

template<std::size_t F>
struct serializer<
    type_prop::not_a_fundamental,
    ser_case::use_internal_serializer,
    F,
    intrusive_buffer
> {
    template<typename Archive>
    static Archive& save(Archive& ar, const intrusive_buffer& buf) {
        __YAS_CONSTEXPR_IF( F & yas::json ) {
            if ( !buf.data ) {
                static const char arr[] = "{\"size\":0,\"data\":null}";
                ar.write(arr, sizeof(arr)-1);
            } else {
                static const char size_[] = "{\"size\":";
                ar.write(size_, sizeof(size_)-1);
                const auto size = modp_b64_encode_strlen(buf.size);
                ar.write(size);
                static const char mid_[] = ",\"data\":\"";
                ar.write(mid_, sizeof(mid_)-1);
                modp_b64_encode(ar, buf.data, buf.size);
                static const char end_[] = "\"}";
                ar.write(end_, sizeof(end_)-1);
            }
        } else {
            ar.write_seq_size(buf.size);
            ar.write(buf.data, buf.size);
        }

        return ar;
    }
    
    template<typename Archive>
    static Archive& load(Archive& ar, intrusive_buffer &) {
        return ar;
    }
};

/***************************************************************************/

template<std::size_t F>
struct serializer<
    type_prop::not_a_fundamental,
    ser_case::use_internal_serializer,
    F,
    shared_buffer
> {
    template<typename Archive>
    static Archive& save(Archive& ar, const shared_buffer &buf) {
        intrusive_buffer ibuf{buf.data.get(), buf.size};
        ar & ibuf;

        return ar;
    }

    template<typename Archive>
    static Archive& load(Archive& ar, shared_buffer &buf) {
        __YAS_CONSTEXPR_IF ( F & yas::json ) {
            __YAS_CONSTEXPR_IF ( !(F & yas::compacted) ) {
                json_skipws(ar);
            }
            __YAS_THROW_IF_BAD_JSON_CHARS(ar, "{\"size\":");

            __YAS_CONSTEXPR_IF ( !(F & yas::compacted) ) {
                json_skipws(ar);
            }

            std::size_t size{};
            ar.read(size);

            buf.resize(size);

            __YAS_CONSTEXPR_IF ( !(F & yas::compacted) ) {
                json_skipws(ar);
            }

            __YAS_THROW_IF_BAD_JSON_CHARS(ar, ",");

            __YAS_CONSTEXPR_IF ( !(F & yas::compacted) ) {
                json_skipws(ar);
            }

            __YAS_THROW_IF_BAD_JSON_CHARS(ar, "\"data\":\"");

            std::size_t declen = modp_b64_decode(buf.data.get(), ar, size);
            buf.resize(declen);

            __YAS_THROW_IF_BAD_JSON_CHARS(ar, "\"");

            __YAS_CONSTEXPR_IF ( !(F & yas::compacted) ) {
                json_skipws(ar);
            }
            __YAS_THROW_IF_BAD_JSON_CHARS(ar, "}");
        } else {
            const std::size_t size = ar.read_seq_size();
            buf.resize(size);
            ar.read(buf.data.get(), buf.size);
        }

        return ar;
    }
};

/***************************************************************************/

} // namespace detail
} // namespace yas

#endif // __yas__types__utility__buffer_hpp
