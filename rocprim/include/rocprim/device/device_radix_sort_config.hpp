// Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef ROCPRIM_DEVICE_DEVICE_RADIX_SORT_CONFIG_HPP_
#define ROCPRIM_DEVICE_DEVICE_RADIX_SORT_CONFIG_HPP_

#include <type_traits>

#include "../config.hpp"
#include "../detail/various.hpp"

#include "config_types.hpp"

BEGIN_ROCPRIM_NAMESPACE

template<
    unsigned int LongRadixBits,
    unsigned int ShortRadixBits,
    class ScanConfig,
    class SortConfig
>
struct radix_sort_config
{
    static constexpr unsigned int long_radix_bits = LongRadixBits;
    static constexpr unsigned int short_radix_bits = ShortRadixBits;
    using scan = ScanConfig;
    using sort = SortConfig;
};

namespace detail
{

template<class Key, class Value>
struct radix_sort_config_803
{
    static constexpr unsigned int item_scale =
        ::rocprim::detail::ceiling_div<unsigned int>(::rocprim::max(sizeof(Key), sizeof(Value)), sizeof(int));

    using scan = kernel_config<256, 2>;

    using type = select_type<
        select_type_case<
            (sizeof(Key) == 1 && sizeof(Value) <= 8),
            radix_sort_config<8, 7, scan, kernel_config<256, 10> >
        >,
        select_type_case<
            (sizeof(Key) == 2 && sizeof(Value) <= 8),
            radix_sort_config<8, 7, scan, kernel_config<256, 10> >
        >,
        select_type_case<
            (sizeof(Key) == 4 && sizeof(Value) <= 8),
            radix_sort_config<7, 6, scan, kernel_config<256, 15> >
        >,
        select_type_case<
            (sizeof(Key) == 8 && sizeof(Value) <= 8),
            radix_sort_config<7, 6, scan, kernel_config<256, 13> >
        >,
        radix_sort_config<7, 6, scan, kernel_config<256, ::rocprim::max(1u, 15u / item_scale)> >
    >;
};

template<class Key>
struct radix_sort_config_803<Key, empty_type>
    : select_type<
        select_type_case<sizeof(Key) == 1, radix_sort_config<8, 7, kernel_config<256, 2>, kernel_config<256, 10> > >,
        select_type_case<sizeof(Key) == 2, radix_sort_config<8, 7, kernel_config<256, 2>, kernel_config<256, 10> > >,
        select_type_case<sizeof(Key) == 4, radix_sort_config<7, 6, kernel_config<256, 2>, kernel_config<256, 9> > >,
        select_type_case<sizeof(Key) == 8, radix_sort_config<7, 6, kernel_config<256, 2>, kernel_config<256, 7> > >
    > { };

template<class Key, class Value>
struct radix_sort_config_900
{
    static constexpr unsigned int item_scale =
        ::rocprim::detail::ceiling_div<unsigned int>(::rocprim::max(sizeof(Key), sizeof(Value)), sizeof(int));

    using scan = kernel_config<256, 2>;

    using type = select_type<
        select_type_case<
            (sizeof(Key) == 1 && sizeof(Value) <= 8),
            radix_sort_config<4, 4, scan, kernel_config<256, 10> >
        >,
        select_type_case<
            (sizeof(Key) == 2 && sizeof(Value) <= 8),
            radix_sort_config<6, 5, scan, kernel_config<256, 10> >
        >,
        select_type_case<
            (sizeof(Key) == 4 && sizeof(Value) <= 8),
            radix_sort_config<7, 6, scan, kernel_config<256, 15> >
        >,
        select_type_case<
            (sizeof(Key) == 8 && sizeof(Value) <= 8),
            radix_sort_config<7, 6, scan, kernel_config<256, 15> >
        >,
        radix_sort_config<7, 6, scan, kernel_config<256, ::rocprim::max(1u, 15u / item_scale)> >
    >;
};

template<class Key>
struct radix_sort_config_900<Key, empty_type>
    : select_type<
        select_type_case<sizeof(Key) == 1, radix_sort_config<4, 3, kernel_config<256, 2>, kernel_config<256, 10> > >,
        select_type_case<sizeof(Key) == 2, radix_sort_config<6, 5, kernel_config<256, 2>, kernel_config<256, 10> > >,
        select_type_case<sizeof(Key) == 4, radix_sort_config<7, 6, kernel_config<256, 2>, kernel_config<256, 17> > >,
        select_type_case<sizeof(Key) == 8, radix_sort_config<7, 6, kernel_config<256, 2>, kernel_config<256, 15> > >
    > { };

} // end namespace detail

template<unsigned int TargetArch, class Key, class Value>
struct default_radix_sort_config
    : select_arch<
        TargetArch,
        select_arch_case<803, detail::radix_sort_config_803<Key, Value> >,
        select_arch_case<900, detail::radix_sort_config_900<Key, Value> >,
        detail::radix_sort_config_900<Key, Value>
    > { };

END_ROCPRIM_NAMESPACE

#endif // ROCPRIM_DEVICE_DEVICE_RADIX_SORT_CONFIG_HPP_
