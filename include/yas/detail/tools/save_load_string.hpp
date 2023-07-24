
// Copyright (c) 2010-2023 niXman (github dot nixman at pm dot me). All
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

#ifndef __yas__detail__tools__save_load_string_hpp
#define __yas__detail__tools__save_load_string_hpp

#include <string>
#include <vector>
#include <cassert>

#if defined(YAS_SERIALIZE_BOOST_TYPES)
#   include <boost/container/string.hpp>
#endif // defined(YAS_SERIALIZE_BOOST_TYPES)

namespace yas {
namespace detail {

/***************************************************************************/

// based on the code from: https://github.com/nlohmann/json/blob/develop/src/json.hpp

template<typename CharT>
std::size_t string_extra_space(const CharT *s) {
    std::size_t res = 0;
    for ( ; *s; ++s ) {
        switch ( *s ) {
            case '"':
            case '\\':
            case '\b':
            case '\f':
            case '\n':
            case '\r':
            case '\t': {
                // from c (1 byte) to \x (2 bytes)
                res += 1;
                break;
            }

            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x06:
            case 0x07:
            case 0x0b:
            case 0x0e:
            case 0x0f:
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
            case 0x14:
            case 0x15:
            case 0x16:
            case 0x17:
            case 0x18:
            case 0x19:
            case 0x1a:
            case 0x1b:
            case 0x1c:
            case 0x1d:
            case 0x1e:
            case 0x1f: {
                // from c (1 byte) to \uxxxx (6 bytes)
                res += 5;
            }
        }
    }

    return res;
}

template<typename CharT>
void string_escape(CharT *d, const CharT *s) {
    std::size_t pos = 0;

    for ( ; *s; ++s ) {
        switch ( *s ) {
            // quotation mark (0x22)
            case '"': {
                d[pos + 1] = '"';
                pos += 2;
                break;
            }
            // reverse solidus (0x5c)
            case '\\': {
                // nothing to change
                pos += 2;
                break;
            }

            // backspace (0x08)
            case '\b': {
                d[pos + 1] = 'b';
                pos += 2;
                break;
            }

            // formfeed (0x0c)
            case '\f': {
                d[pos + 1] = 'f';
                pos += 2;
                break;
            }

            // newline (0x0a)
            case '\n': {
                d[pos + 1] = 'n';
                pos += 2;
                break;
            }

            // carriage return (0x0d)
            case '\r': {
                d[pos + 1] = 'r';
                pos += 2;
                break;
            }

            // horizontal tab (0x09)
            case '\t': {
                d[pos + 1] = 't';
                pos += 2;
                break;
            }

            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x06:
            case 0x07:
            case 0x0b:
            case 0x0e:
            case 0x0f:
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
            case 0x14:
            case 0x15:
            case 0x16:
            case 0x17:
            case 0x18:
            case 0x19:
            case 0x1a:
            case 0x1b:
            case 0x1c:
            case 0x1d:
            case 0x1e:
            case 0x1f: {
                // convert a number 0..15 to its hex representation
                // (0..f)
                static const char hexify[16] = {
                    '0', '1', '2', '3', '4', '5', '6', '7',
                    '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
                };

                // print character c as \uxxxx
                for (const char m : {'u', '0', '0', hexify[(*s) >> 4], hexify[(*s) & 0x0f]}) {
                    d[++pos] = m;
                }

                ++pos;
                break;
            }

            default: {
                // all other characters are added as-is
                d[pos++] = *s;
                break;
            }
        }
    }
}

template<typename Archive>
static int string_get_codepoint(Archive &ar)  {
    int codepoint = 0;

    char ch0;
    ar.read(ch0);
    switch ( ch0 ) {
        case '0': break;
        case '1': codepoint += 0x1000; break;
        case '2': codepoint += 0x2000; break;
        case '3': codepoint += 0x3000; break;
        case '4': codepoint += 0x4000; break;
        case '5': codepoint += 0x5000; break;
        case '6': codepoint += 0x6000; break;
        case '7': codepoint += 0x7000; break;
        case '8': codepoint += 0x8000; break;
        case '9': codepoint += 0x9000; break;
        case 'A':
        case 'a': codepoint += 0xa000; break;
        case 'B':
        case 'b': codepoint += 0xb000; break;
        case 'C':
        case 'c': codepoint += 0xc000; break;
        case 'D':
        case 'd': codepoint += 0xd000; break;
        case 'E':
        case 'e': codepoint += 0xe000; break;
        case 'F':
        case 'f': codepoint += 0xf000; break;
        default: return -1;
    }

    char ch1;
    ar.read(ch1);
    switch ( ch1 ) {
        case '0': break;
        case '1': codepoint += 0x0100; break;
        case '2': codepoint += 0x0200; break;
        case '3': codepoint += 0x0300; break;
        case '4': codepoint += 0x0400; break;
        case '5': codepoint += 0x0500; break;
        case '6': codepoint += 0x0600; break;
        case '7': codepoint += 0x0700; break;
        case '8': codepoint += 0x0800; break;
        case '9': codepoint += 0x0900; break;
        case 'A':
        case 'a': codepoint += 0x0a00; break;
        case 'B':
        case 'b': codepoint += 0x0b00; break;
        case 'C':
        case 'c': codepoint += 0x0c00; break;
        case 'D':
        case 'd': codepoint += 0x0d00; break;
        case 'E':
        case 'e': codepoint += 0x0e00; break;
        case 'F':
        case 'f': codepoint += 0x0f00; break;
        default: return -1;
    }

    char ch2;
    ar.read(ch2);
    switch ( ch2 ) {
        case '0': break;
        case '1': codepoint += 0x0010; break;
        case '2': codepoint += 0x0020; break;
        case '3': codepoint += 0x0030; break;
        case '4': codepoint += 0x0040; break;
        case '5': codepoint += 0x0050; break;
        case '6': codepoint += 0x0060; break;
        case '7': codepoint += 0x0070; break;
        case '8': codepoint += 0x0080; break;
        case '9': codepoint += 0x0090; break;
        case 'A':
        case 'a': codepoint += 0x00a0; break;
        case 'B':
        case 'b': codepoint += 0x00b0; break;
        case 'C':
        case 'c': codepoint += 0x00c0; break;
        case 'D':
        case 'd': codepoint += 0x00d0; break;
        case 'E':
        case 'e': codepoint += 0x00e0; break;
        case 'F':
        case 'f': codepoint += 0x00f0; break;
        default: return -1;
    }

    char ch3;
    ar.read(ch3);
    switch ( ch3 ) {
        case '0': break;
        case '1': codepoint += 0x0001; break;
        case '2': codepoint += 0x0002; break;
        case '3': codepoint += 0x0003; break;
        case '4': codepoint += 0x0004; break;
        case '5': codepoint += 0x0005; break;
        case '6': codepoint += 0x0006; break;
        case '7': codepoint += 0x0007; break;
        case '8': codepoint += 0x0008; break;
        case '9': codepoint += 0x0009; break;
        case 'A':
        case 'a': codepoint += 0x000a; break;
        case 'B':
        case 'b': codepoint += 0x000b; break;
        case 'C':
        case 'c': codepoint += 0x000c; break;
        case 'D':
        case 'd': codepoint += 0x000d; break;
        case 'E':
        case 'e': codepoint += 0x000e; break;
        case 'F':
        case 'f': codepoint += 0x000f; break;
        default: return -1;
    }

    return codepoint;
}

template<
     typename Archive
    ,template<typename, typename, typename> class StringT
    ,typename CharT
    ,typename TraitsT
    ,typename AllocatorT
>
static void string_read_string(StringT<CharT, TraitsT, AllocatorT> &d, Archive &ar) {
#if defined(__clang__) || defined(__GNUC__)
#   define __JSON_LIKELY(x)      __builtin_expect(static_cast<bool>(x), 1)
#   define __JSON_UNLIKELY(x)    __builtin_expect(static_cast<bool>(x), 0)
#else
#   define __JSON_LIKELY(x)      x
#   define __JSON_UNLIKELY(x)    x
#endif

    auto dit = std::back_inserter(d);
    while ( true ) {
        std::uint8_t ch = ar.getch();

        switch ( ch ) {
            // closing quote
            case '\"': {
                ar.ungetch(ch);
                return;
            }
            // escapes
            case '\\': {
                char ch2 = ar.getch();
                switch ( ch2 ) {
                    // quotation mark
                    case '\"': *dit++ = '\"'; break;
                    // reverse solidus
                    case '\\': *dit++ = '\\'; break;
                    // solidus
                    case '/': *dit++ = '/'; break;
                    // backspace
                    case 'b': *dit++ = '\b'; break;
                    // form feed
                    case 'f': *dit++ = '\f'; break;
                    // line feed
                    case 'n': *dit++ = '\n'; break;
                    // carriage return
                    case 'r': *dit++ = '\r'; break;
                    // tab
                    case 't': *dit++ = '\t'; break;
                    // unicode escapes
                    case 'u': {
                        int codepoint, codepoint1 = string_get_codepoint(ar);

                        if ( __JSON_UNLIKELY(codepoint1 == -1) ) {
                            __YAS_THROW_INVALID_JSON_STRING("invalid string: '\\u' must be followed by 4 hex digits");
                        }

                        // check if code point is a high surrogate
                        if ( 0xD800 <= codepoint1 && codepoint1 <= 0xDBFF ) {
                            // expect next \uxxxx entry
                            if ( __JSON_LIKELY(ar.getch() == '\\' && ar.getch() == 'u') ) {
                                const int codepoint2 = string_get_codepoint(ar);

                                if ( __JSON_UNLIKELY(codepoint2 == -1) ) {
                                    __YAS_THROW_INVALID_JSON_STRING("invalid string: '\\u' must be followed by 4 hex digits");
                                }

                                // check if codepoint2 is a low surrogate
                                if ( __JSON_LIKELY(0xDC00 <= codepoint2 && codepoint2 <= 0xDFFF) ) {
                                    codepoint =
                                            // high surrogate occupies the most significant 22 bits
                                            (codepoint1 << 10)
                                            // low surrogate occupies the least significant 15 bits
                                            + codepoint2
                                            // there is still the 0xD800, 0xDC00 and 0x10000 noise
                                            // in the result so we have to subtract with:
                                            // (0xD800 << 10) + DC00 - 0x10000 = 0x35FDC00
                                            - 0x35FDC00;
                                } else {
                                    //error_message = "invalid string: surrogate " + codepoint_to_string(codepoint1) + " must be followed by U+DC00..U+DFFF instead of " + codepoint_to_string(codepoint2);
                                    __YAS_THROW_INVALID_JSON_STRING("invalid string");
                                }
                            } else {
                                //error_message = "invalid string: surrogate " + codepoint_to_string(codepoint1) + " must be followed by U+DC00..U+DFFF";
                                __YAS_THROW_INVALID_JSON_STRING("invalid string");
                            }
                        } else {
                            if ( __JSON_UNLIKELY(0xDC00 <= codepoint1 && codepoint1 <= 0xDFFF) ) {
                                //error_message = "invalid string: surrogate " + codepoint_to_string(codepoint1) + " must follow U+D800..U+DBFF";
                                __YAS_THROW_INVALID_JSON_STRING("invalid string");
                            }

                            // only work with first code point
                            codepoint = codepoint1;
                        }

                        // result of the above calculation yields a proper codepoint
                        assert(0x00 <= codepoint && codepoint <= 0x10FFFF);

                        // translate code point to bytes
                        if ( codepoint < 0x80 ) {
                            // 1-byte characters: 0xxxxxxx (ASCII)
                            *dit++ = __YAS_SCAST(char, codepoint);
                        } else if ( codepoint <= 0x7ff ) {
                            // 2-byte characters: 110xxxxx 10xxxxxx
                            *dit++ = __YAS_SCAST(char, 0xC0 | (codepoint >> 6));
                            *dit++ = __YAS_SCAST(char, 0x80 | (codepoint & 0x3F));
                        } else if ( codepoint <= 0xffff ) {
                            // 3-byte characters: 1110xxxx 10xxxxxx 10xxxxxx
                            *dit++ = __YAS_SCAST(char, 0xE0 | (codepoint >> 12));
                            *dit++ = __YAS_SCAST(char, 0x80 | ((codepoint >> 6) & 0x3F));
                            *dit++ = __YAS_SCAST(char, 0x80 | (codepoint & 0x3F));
                        } else {
                            // 4-byte characters: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                            *dit++ = __YAS_SCAST(char, 0xF0 | (codepoint >> 18));
                            *dit++ = __YAS_SCAST(char, 0x80 | ((codepoint >> 12) & 0x3F));
                            *dit++ = __YAS_SCAST(char, 0x80 | ((codepoint >> 6) & 0x3F));
                            *dit++ = __YAS_SCAST(char, 0x80 | (codepoint & 0x3F));
                        }

                        break;
                    }
                        // other characters after escape
                    default:
                        __YAS_THROW_INVALID_JSON_STRING("invalid string: forbidden character after backslash");
                }

                break;
            }

            // invalid control characters
            case 0x00:
            case 0x01:
            case 0x02:
            case 0x03:
            case 0x04:
            case 0x05:
            case 0x06:
            case 0x07:
            case 0x08:
            case 0x09:
            case 0x0a:
            case 0x0b:
            case 0x0c:
            case 0x0d:
            case 0x0e:
            case 0x0f:
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
            case 0x14:
            case 0x15:
            case 0x16:
            case 0x17:
            case 0x18:
            case 0x19:
            case 0x1a:
            case 0x1b:
            case 0x1c:
            case 0x1d:
            case 0x1e:
            case 0x1f: {
                //error_message = "invalid string: control character " + codepoint_to_string(current) + " must be escaped";
                __YAS_THROW_INVALID_JSON_STRING("invalid string");
            }

            // U+0020..U+007F (except U+0022 (quote) and U+005C (backspace))
            case 0x20:
            case 0x21:
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
            case 0x28:
            case 0x29:
            case 0x2a:
            case 0x2b:
            case 0x2c:
            case 0x2d:
            case 0x2e:
            case 0x2f:
            case 0x30:
            case 0x31:
            case 0x32:
            case 0x33:
            case 0x34:
            case 0x35:
            case 0x36:
            case 0x37:
            case 0x38:
            case 0x39:
            case 0x3a:
            case 0x3b:
            case 0x3c:
            case 0x3d:
            case 0x3e:
            case 0x3f:
            case 0x40:
            case 0x41:
            case 0x42:
            case 0x43:
            case 0x44:
            case 0x45:
            case 0x46:
            case 0x47:
            case 0x48:
            case 0x49:
            case 0x4a:
            case 0x4b:
            case 0x4c:
            case 0x4d:
            case 0x4e:
            case 0x4f:
            case 0x50:
            case 0x51:
            case 0x52:
            case 0x53:
            case 0x54:
            case 0x55:
            case 0x56:
            case 0x57:
            case 0x58:
            case 0x59:
            case 0x5a:
            case 0x5b:
            case 0x5d:
            case 0x5e:
            case 0x5f:
            case 0x60:
            case 0x61:
            case 0x62:
            case 0x63:
            case 0x64:
            case 0x65:
            case 0x66:
            case 0x67:
            case 0x68:
            case 0x69:
            case 0x6a:
            case 0x6b:
            case 0x6c:
            case 0x6d:
            case 0x6e:
            case 0x6f:
            case 0x70:
            case 0x71:
            case 0x72:
            case 0x73:
            case 0x74:
            case 0x75:
            case 0x76:
            case 0x77:
            case 0x78:
            case 0x79:
            case 0x7a:
            case 0x7b:
            case 0x7c:
            case 0x7d:
            case 0x7e:
            case 0x7f: {
                *dit++ = ch;
                break;
            }

            // U+0080..U+07FF: bytes C2..DF 80..BF
            case 0xc2:
            case 0xc3:
            case 0xc4:
            case 0xc5:
            case 0xc6:
            case 0xc7:
            case 0xc8:
            case 0xc9:
            case 0xca:
            case 0xcb:
            case 0xcc:
            case 0xcd:
            case 0xce:
            case 0xcf:
            case 0xd0:
            case 0xd1:
            case 0xd2:
            case 0xd3:
            case 0xd4:
            case 0xd5:
            case 0xd6:
            case 0xd7:
            case 0xd8:
            case 0xd9:
            case 0xda:
            case 0xdb:
            case 0xdc:
            case 0xdd:
            case 0xde:
            case 0xdf: {
                *dit++ = ch;
                ch = ar.getch();
                if ( __JSON_LIKELY(0x80 <= ch && ch <= 0xbf) ) {
                    *dit++ = ch;
                    continue;
                }

                __YAS_THROW_INVALID_JSON_STRING("invalid string: ill-formed UTF-8 byte");
            }

            // U+0800..U+0FFF: bytes E0 A0..BF 80..BF
            case 0xe0: {
                *dit++ = ch;
                ch = ar.getch();
                if ( __JSON_LIKELY(0xa0 <= ch && ch <= 0xbf) ) {
                    *dit++ = ch;
                    ch = ar.getch();
                    if ( __JSON_LIKELY(0x80 <= ch && ch <= 0xbf) ) {
                        *dit++ = ch;
                        continue;
                    }
                }
                __YAS_THROW_INVALID_JSON_STRING("invalid string: ill-formed UTF-8 byte");
            }

            // U+1000..U+CFFF: bytes E1..EC 80..BF 80..BF
            // U+E000..U+FFFF: bytes EE..EF 80..BF 80..BF
            case 0xe1:
            case 0xe2:
            case 0xe3:
            case 0xe4:
            case 0xe5:
            case 0xe6:
            case 0xe7:
            case 0xe8:
            case 0xe9:
            case 0xea:
            case 0xeb:
            case 0xec:
            case 0xee:
            case 0xef: {
                *dit++ = ch;
                ch = ar.getch();
                if ( __JSON_LIKELY(0x80 <= ch && ch <= 0xbf) ) {
                    *dit++ = ch;
                    ch = ar.getch();
                    if ( __JSON_LIKELY(0x80 <= ch && ch <= 0xbf) ) {
                        *dit++ = ch;
                        continue;
                    }
                }

                __YAS_THROW_INVALID_JSON_STRING("invalid string: ill-formed UTF-8 byte");
            }

            // U+D000..U+D7FF: bytes ED 80..9F 80..BF
            case 0xed: {
                *dit++ = ch;
                ch = ar.getch();
                if ( __JSON_LIKELY(0x80 <= ch && ch <= 0x9f) ) {
                    *dit++ = ch;
                    ch = ar.getch();
                    if ( __JSON_LIKELY(0x80 <= ch && ch <= 0xbf) ) {
                        *dit++ = ch;
                        continue;
                    }
                }

                __YAS_THROW_INVALID_JSON_STRING("invalid string: ill-formed UTF-8 byte");
            }

            // U+10000..U+3FFFF F0 90..BF 80..BF 80..BF
            case 0xf0: {
                *dit++ = ch;
                ch = ar.getch();
                if ( __JSON_LIKELY(0x90 <= ch && ch <= 0xbf) ) {
                    *dit++ = ch;
                    ch = ar.getch();
                    if ( __JSON_LIKELY(0x80 <= ch && ch <= 0xbf) ) {
                        *dit++ = ch;
                        ch = ar.getch();
                        if ( __JSON_LIKELY(0x80 <= ch && ch <= 0xbf) ) {
                            *dit++ = ch;
                            continue;
                        }
                    }
                }

                __YAS_THROW_INVALID_JSON_STRING("invalid string: ill-formed UTF-8 byte");
            }

            // U+40000..U+FFFFF F1..F3 80..BF 80..BF 80..BF
            case 0xf1:
            case 0xf2:
            case 0xf3: {
                *dit++ = ch;
                ch = ar.getch();
                if ( __JSON_LIKELY(0x80 <= ch && ch <= 0xbf) ) {
                    *dit++ = ch;
                    ch = ar.getch();
                    if ( __JSON_LIKELY(0x80 <= ch && ch <= 0xbf) ) {
                        *dit++ = ch;
                        ch = ar.getch();
                        if (__JSON_LIKELY(0x80 <= ch && ch <= 0xbf)) {
                            *dit++ = ch;
                            continue;
                        }
                    }
                }

                __YAS_THROW_INVALID_JSON_STRING("invalid string: ill-formed UTF-8 byte");
            }

            // U+100000..U+10FFFF F4 80..8F 80..BF 80..BF
            case 0xf4: {
                *dit++ = ch;
                ch = ar.getch();
                if (__JSON_LIKELY(0x80 <= ch && ch <= 0x8f))
                {
                    *dit++ = ch;
                    ch = ar.getch();
                    if (__JSON_LIKELY(0x80 <= ch && ch <= 0xbf))
                    {
                        *dit++ = ch;
                        ch = ar.getch();
                        if ( __JSON_LIKELY(0x80 <= ch && ch <= 0xbf) ) {
                            *dit++ = ch;
                            continue;
                        }
                    }
                }

                __YAS_THROW_INVALID_JSON_STRING("invalid string: ill-formed UTF-8 byte");
            }

            // remaining bytes (80..C1 and F5..FF) are ill-formed
            default: __YAS_THROW_INVALID_JSON_STRING("invalid string: ill-formed UTF-8 byte");
        }
    }
#undef __JSON_LIKELY
#undef __JSON_UNLIKELY
}

/***************************************************************************/

template<
     typename Archive
    ,typename CharT
>
Archive& save_string(Archive &ar, const CharT *str, std::size_t len) {
    //TODO: зачем создавать временную строку для экранирования, если можно экранировать записывая в архив!
    const std::size_t n = string_extra_space(str);
    // must be escaped
    if ( n ) {
        std::vector<CharT> escaped(len + n, '\\');
        string_escape(escaped.data(), str);

        ar.write(escaped.data(), escaped.size());
    } else {
        ar.write(str, len);
    }

    return ar;
}

template<
     typename Archive
    ,template<typename, typename, typename> class StringT
    ,typename CharT
    ,typename TraitsT
    ,typename AllocatorT
>
Archive& load_string(StringT<CharT, TraitsT, AllocatorT> &string, Archive &ar) {
    string_read_string(string, ar);

    return ar;
}

/***************************************************************************/

} // ns detail
} // ns yas

#endif // __yas__detail__tools__save_load_string_hpp
