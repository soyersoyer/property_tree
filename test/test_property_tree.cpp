// ----------------------------------------------------------------------------
// ****----****
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see www.boost.org
// ----------------------------------------------------------------------------
#include "test_utils.hpp"
#include <boost/any.hpp>
#include <list>
#include <cmath>

// Predicate for sorting keys
template<class Ptree>
struct SortPred
{
    bool operator()(const typename Ptree::value_type &v1,
                    const typename Ptree::value_type &v2) const
    {
        return v1.first < v2.first;
    }
};

// Predicate for sorting keys in reverse
template<class Ptree>
struct SortPredRev
{
    bool operator()(const typename Ptree::value_type &v1,
                    const typename Ptree::value_type &v2) const
    {
        return v1.first > v2.first;
    }
};

// Custom translator that works with boost::any instead of std::string
struct MyTranslator
{

    // Custom extractor - converts data from boost::any to T
    template<class Ptree, class T> 
    bool get_value(const Ptree &pt, T &value) const
    {
        value = boost::any_cast<T>(pt.data());
        return true;    // Success
    }

    // Custom inserter - converts data from T to boost::any
    template<class Ptree, class T> 
    bool put_value(Ptree &pt, const T &value) const
    {
        pt.data() = value;
        return true;
    }

};

// Include char tests, case sensitive
#define CHTYPE char
#define T(s) s
#define PTREE boost::property_tree::ptree
#define NOCASE 0
#   include "test_property_tree.hpp"
#undef CHTYPE
#undef T
#undef PTREE
#undef NOCASE

// Include wchar_t tests, case sensitive
#ifndef BOOST_NO_CWCHAR
#   define CHTYPE wchar_t
#   define T(s) L ## s
#   define PTREE boost::property_tree::wptree
#   define NOCASE 0
#       include "test_property_tree.hpp"
#   undef CHTYPE
#   undef T
#   undef PTREE
#   undef NOCASE
#endif

// Include char tests, case insensitive
#define CHTYPE char
#define T(s) s
#define PTREE boost::property_tree::iptree
#define NOCASE 1
#   include "test_property_tree.hpp"
#undef CHTYPE
#undef T
#undef PTREE
#undef NOCASE

// Include wchar_t tests, case insensitive
#ifndef BOOST_NO_CWCHAR
#   define CHTYPE wchar_t
#   define T(s) L ## s
#   define PTREE boost::property_tree::wiptree
#   define NOCASE 1
#       include "test_property_tree.hpp"
#   undef CHTYPE
#   undef T
#   undef PTREE
#   undef NOCASE
#endif

int test_main(int, char *[])
{
    
    using namespace boost::property_tree;
    
    // char tests, case sensitive
    {
        ptree *pt = 0;
        test_debug(pt);
        test_constructor_destructor_assignment(pt);
        test_insertion(pt);
        test_erasing(pt);
        test_clear(pt);
        test_pushpop(pt);
        test_container_iteration(pt);
        test_swap(pt);
        test_sort_reverse(pt);
        test_case(pt);
        test_comparison(pt);
        test_front_back(pt);
        test_get_put(pt);
        test_get_child_put_child(pt);
        test_path_separator(pt);
        test_path(pt);
        test_precision(pt);
        test_locale(pt);
        test_custom_data_type(pt);
        test_empty_size_max_size(pt);
        test_leaks(pt);                  // must be a final test
    }

    // wchar_t tests, case sensitive
#ifndef BOOST_NO_CWCHAR
    {
        wptree *pt = 0;
        test_debug(pt);
        test_constructor_destructor_assignment(pt);
        test_insertion(pt);
        test_erasing(pt);
        test_clear(pt);
        test_pushpop(pt);
        test_container_iteration(pt);
        test_swap(pt);
        test_sort_reverse(pt);
        test_case(pt);
        test_comparison(pt);
        test_front_back(pt);
        test_get_put(pt);
        test_get_child_put_child(pt);
        test_path_separator(pt);
        test_path(pt);
        test_precision(pt);
        test_locale(pt);
        test_custom_data_type(pt);
        test_empty_size_max_size(pt);
        test_leaks(pt);                  // must be a final test
    }
#endif

    // char tests, case insensitive
    {
        iptree *pt = 0;
        test_debug(pt);
        test_constructor_destructor_assignment(pt);
        test_insertion(pt);
        test_erasing(pt);
        test_clear(pt);
        test_pushpop(pt);
        test_container_iteration(pt);
        test_swap(pt);
        test_sort_reverse(pt);
        test_case(pt);
        test_comparison(pt);
        test_front_back(pt);
        test_get_put(pt);
        test_get_child_put_child(pt);
        test_path_separator(pt);
        test_path(pt);
        test_precision(pt);
        test_locale(pt);
        test_custom_data_type(pt);
        test_empty_size_max_size(pt);
        test_leaks(pt);                  // must be a final test
    }

    // wchar_t tests, case insensitive
#ifndef BOOST_NO_CWCHAR
    {
        wiptree *pt = 0;
        test_debug(pt);
        test_constructor_destructor_assignment(pt);
        test_insertion(pt);
        test_erasing(pt);
        test_clear(pt);
        test_pushpop(pt);
        test_container_iteration(pt);
        test_swap(pt);
        test_sort_reverse(pt);
        test_case(pt);
        test_comparison(pt);
        test_front_back(pt);
        test_get_put(pt);
        test_get_child_put_child(pt);
        test_path_separator(pt);
        test_path(pt);
        test_precision(pt);
        test_locale(pt);
        test_custom_data_type(pt);
        test_empty_size_max_size(pt);
        test_leaks(pt);                  // must be a final test
    }
#endif

    return 0;
}
