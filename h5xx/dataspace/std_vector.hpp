/*
 * Copyright © 2014-2015 Klaus Reuter
 * Copyright © 2014      Felix Höfling
 *
 * This file is part of h5xx.
 *
 * h5xx is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef H5XX_DATASPACE_STD_VECTOR
#define H5XX_DATASPACE_STD_VECTOR

#include <vector>

#include <h5xx/dataspace.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/mpl/and.hpp>
#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>

namespace h5xx {

template <typename T>
inline typename boost::enable_if< boost::mpl::and_< is_vector<T>, boost::is_fundamental<typename T::value_type> >, dataspace>::type
create_dataspace(T const& value)
{
    std::vector<hsize_t> value_dims;
    value_dims.push_back(value.size());
    return dataspace(value_dims);
}

} // namespace h5xx

#endif // ! H5XX_DATASPACE_STD_VECTOR
