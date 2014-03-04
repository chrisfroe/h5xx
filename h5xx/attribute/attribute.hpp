/*
 * Copyright © 2014 Felix Höfling
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

#ifndef H5XX_ATTRIBUTE_ATTRIBUTE_HPP
#define H5XX_ATTRIBUTE_ATTRIBUTE_HPP

#include <boost/lexical_cast.hpp>

#include <h5xx/hdf5_compat.hpp>
#include <h5xx/error.hpp>
#include <h5xx/utility.hpp>

namespace h5xx {

/**
 * Represents an HDF5 attribute.
 *
 */
class attribute
{
public:
    /** default constructor */
    attribute() : hid_(-1) {}

    /** copy constructor with move semantics
     *
     * The attribute object on the right hand side is empty after assignment. In
     * C++98, moving from an r-values (e.g., a temporary) is not possible.
     */
    attribute(attribute& other);

    /** assignment operator with move semantics
     *
     * the attribute object on the right hand side is empty after assignment
     */
    attribute const& operator=(attribute other);

    /** default destructor */
    ~attribute();

    /** return HDF5 object ID */
    hid_t hid() const
    {
        return hid_;
    }

    /** returns true if associated to a valid HDF5 object */
    bool valid() const
    {
        return hid_ >= 0;
    }

    // FIXME make the following functions private and add friend functions

    /** open existing attribute of given name, attached to the HDF5 object */
    template <typename h5xxObject>
    attribute(h5xxObject const& object, std::string const& name, hid_t aapl_id = H5P_DEFAULT);

    /** construct from HDF5 object ID */
    attribute(hid_t hid) : hid_(hid) {}

private:
    hid_t hid_;
};

template <typename h5xxObject>
attribute::attribute(h5xxObject const& object, std::string const& name, hid_t aapl_id)
  : hid_(-1)
{
    hid_t obj_hid = object.hid();
    char const* attr_name = name.c_str();
    if (H5Aexists(obj_hid, attr_name) > 0) {
        hid_ = H5Aopen(obj_hid, attr_name, aapl_id);
    }
    if (hid_ < 0){
        throw error("opening attribute \"" + name + "\" at HDF5 object \"" + get_name(object) + "\"");
    }
}

attribute::attribute(attribute& other)
{
    hid_ = other.hid_;
    other.hid_ = -1;
}

attribute::~attribute()
{
    if (hid_ >= 0) {
        if(H5Aclose(hid_) < 0){
            throw error("closing h5xx::attribute with ID " + boost::lexical_cast<std::string>(hid_));
        }
    }
}

attribute const& attribute::operator=(attribute other)
{
    // swap(hid_, other.hid_),
    // the previous attribute object will be closed upon destruction of other
    hid_t tmp = hid_;
    hid_ = other.hid_;
    other.hid_ = tmp;
    return *this;
}

} // namespace h5xx

#endif /* ! H5XX_ATTRIBUTE_ATTRIBUTE_HPP */
