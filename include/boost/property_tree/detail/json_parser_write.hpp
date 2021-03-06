// ----------------------------------------------------------------------------
// Copyright (C) 2002-2006 Marcin Kalicinski
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see www.boost.org
// ----------------------------------------------------------------------------
#ifndef BOOST_PROPERTY_TREE_DETAIL_JSON_PARSER_WRITE_HPP_INCLUDED
#define BOOST_PROPERTY_TREE_DETAIL_JSON_PARSER_WRITE_HPP_INCLUDED

#include <boost/cstdint.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/next_prior.hpp>
#include <boost/type_traits/make_unsigned.hpp>
#include <string>
#include <ostream>
#include <sstream>
#include <iomanip>
#include <codecvt>
#include <algorithm>

namespace boost { namespace property_tree { namespace json_parser
{

    // Create necessary escape sequences from illegal characters
    template<class Ch>
    std::basic_string<Ch> create_escapes(const std::basic_string<Ch> &s)
    {
        std::basic_string<Ch> result;
        typename std::basic_string<Ch>::const_iterator b = s.begin();
        typename std::basic_string<Ch>::const_iterator e = s.end();
        while (b != e)
        {
            // This assumes an ASCII superset. But so does everything in PTree.
            // We escape everything outside ASCII, because this code can't
            // handle high unicode characters.
            if (*b == 0x20 || *b == 0x21 || (*b >= 0x23 && *b <= 0x2E) ||
                (*b >= 0x30 && *b <= 0x5B) || (*b >= 0x5D /*&& *b <= 0x80*/))
                result += *b;
            else if (*b == Ch('\b')) result += Ch('\\'), result += Ch('b');
            else if (*b == Ch('\f')) result += Ch('\\'), result += Ch('f');
            else if (*b == Ch('\n')) result += Ch('\\'), result += Ch('n');
            else if (*b == Ch('\r')) result += Ch('\\'), result += Ch('r');
            else if (*b == Ch('/')) result += Ch('\\'), result += Ch('/');
            else if (*b == Ch('"'))  result += Ch('\\'), result += Ch('"');
            else if (*b == Ch('\\')) result += Ch('\\'), result += Ch('\\');
            else
            {
                std::ostringstream oss;
                typedef typename make_unsigned<Ch>::type UCh;
                if (long(std::numeric_limits<UCh>::max()) >= 0xFFFF)
                {
                    // Assume UTF16.
                    oss << "\\u" << std::setw(4) << std::setfill('0') << std::hex << std::uppercase << uint16_t(*b);
                    if ((0xD7FF < uint16_t(*b)) && (uint16_t(*b) < 0xE000))
                    {
                        // Add second 16 bit value for surrogat6e pair
                        if (e-b == 1)
                            BOOST_PROPERTY_TREE_THROW(json_parser_error("write error", "", 0));
									
                        ++b;
                        oss << "\\u" << std::setw(4) << std::setfill('0') << std::hex << std::uppercase << uint16_t(*b);
                    }
                    
                    result += oss.str();
                }
                else
                {
                    // Assume UTF8
                    std::mbstate_t state = std::mbstate_t();
                    std::codecvt_utf8_utf16<wchar_t> utf16_utf8;
                    
                    std::size_t in_max = std::min<size_t>(e-b,4);
                    // Determine how many 'C' chars will be consumed to constuct a single UTF8 codepoint - this is
                    // required as codecvt.in will fail if it begins another code point that can't be completed
                    // because the 'end' of input sequence is reached...
                    if ((*b & uint8_t(0xE0)) == uint8_t(0xC0))
                        in_max = std::min<size_t>(e-b,2);
                    else if ((*b & uint8_t(0xF0)) == uint8_t(0xE0))
                        in_max = std::min<size_t>(e-b,3);
                    else if ((*b & uint8_t(0xF8)) == uint8_t(0xF0))
									in_max = std::min<size_t>(e-b,4);
                    else
                        BOOST_PROPERTY_TREE_THROW(json_parser_error("write error", "", 0));
                    
                    Ch const *from_next = &*b;
                    wchar_t utf16str[2] = { wchar_t(0), wchar_t(0) };
                    wchar_t *to_next = utf16str;
                    if (utf16_utf8.in(state, &*b, (&*b) + in_max, from_next, utf16str, utf16str + 2, to_next) != 0)
                        BOOST_PROPERTY_TREE_THROW(json_parser_error("write error", "", 0));
                    
                    oss << "\\u" << std::setw(4) << std::setfill('0') << std::hex << std::uppercase << uint16_t(utf16str[0]);
                    if ( to_next - utf16str == 2)
                    {
                        // Add second 16 bit value for surrogat6e pair
									oss << "\\u" << std::setw(4) << std::setfill('0') << std::hex << std::uppercase << uint16_t(utf16str[1]);
                    }
                    result += oss.str();
                    
                    b += ((from_next - &*b) - std::size_t(1)); // Additinal incrementing for additional UTF8 chars consumed.
                }
            }
            ++b;
        }
        return result;
    }
    
    template<class Ptree>
    void write_json_helper(std::basic_ostream<typename Ptree::key_type::value_type> &stream, 
                           const Ptree &pt,
                           int indent, bool pretty)
    {

        typedef typename Ptree::key_type::value_type Ch;
        typedef typename std::basic_string<Ch> Str;

        // Value or object or array
        if (indent > 0 && pt.empty())
        {
            // Write value
            Str data = create_escapes(pt.template get_value<Str>());
            stream << Ch('"') << data << Ch('"');

        }
        else if (indent > 0 && pt.count(Str()) == pt.size())
        {
            // Write array
            stream << Ch('[');
            if (pretty) stream << Ch('\n');
            typename Ptree::const_iterator it = pt.begin();
            for (; it != pt.end(); ++it)
            {
                if (pretty) stream << Str(4 * (indent + 1), Ch(' '));
                write_json_helper(stream, it->second, indent + 1, pretty);
                if (boost::next(it) != pt.end())
                    stream << Ch(',');
                if (pretty) stream << Ch('\n');
            }
            stream << Str(4 * indent, Ch(' ')) << Ch(']');

        }
        else
        {
            // Write object
            stream << Ch('{');
            if (pretty) stream << Ch('\n');
            typename Ptree::const_iterator it = pt.begin();
            for (; it != pt.end(); ++it)
            {
                if (pretty) stream << Str(4 * (indent + 1), Ch(' '));
                stream << Ch('"') << create_escapes(it->first) << Ch('"') << Ch(':');
                if (pretty) {
                    if (it->second.empty())
                        stream << Ch(' ');
                    else
                        stream << Ch('\n') << Str(4 * (indent + 1), Ch(' '));
                }
                write_json_helper(stream, it->second, indent + 1, pretty);
                if (boost::next(it) != pt.end())
                    stream << Ch(',');
                if (pretty) stream << Ch('\n');
            }
            if (pretty) stream << Str(4 * indent, Ch(' '));
            stream << Ch('}');
        }

    }

    // Verify if ptree does not contain information that cannot be written to json
    template<class Ptree>
    bool verify_json(const Ptree &pt, int depth)
    {

        typedef typename Ptree::key_type::value_type Ch;
        typedef typename std::basic_string<Ch> Str;

        // Root ptree cannot have data
        if (depth == 0 && !pt.template get_value<Str>().empty())
            return false;
        
        // Ptree cannot have both children and data
        if (!pt.template get_value<Str>().empty() && !pt.empty())
            return false;

        // Check children
        typename Ptree::const_iterator it = pt.begin();
        for (; it != pt.end(); ++it)
            if (!verify_json(it->second, depth + 1))
                return false;

        // Success
        return true;

    }
    
    // Write ptree to json stream
    template<class Ptree>
    void write_json_internal(std::basic_ostream<typename Ptree::key_type::value_type> &stream, 
                             const Ptree &pt,
                             const std::string &filename,
                             bool pretty)
    {
        if (!verify_json(pt, 0))
            BOOST_PROPERTY_TREE_THROW(json_parser_error("ptree contains data that cannot be represented in JSON format", filename, 0));
        write_json_helper(stream, pt, 0, pretty);
        stream << std::endl;
        if (!stream.good())
            BOOST_PROPERTY_TREE_THROW(json_parser_error("write error", filename, 0));
    }

} } }

#endif
