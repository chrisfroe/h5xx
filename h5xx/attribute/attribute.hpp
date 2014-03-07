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
#include <h5xx/dataspace.hpp>
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

    /**
     * deleted copy constructor
     *
     * Calling the constructor throws an exception. Copying must be elided by
     * return value optimisation. See also "attribute h5xx::move(attribute&)".
     */
    attribute(attribute const& other);

    /**
     * assignment operator
     *
     * Uses the copy-and-swap idiom. Move semantics is achieved in conjunction
     * with "attribute h5xx::move(attribute&)", i.e., the attribute object on the right
     * hand side is empty after move assignment.
     */
    attribute const& operator=(attribute other);

    /** default destructor */
    ~attribute();

    /** deduce dataspace from attribute */
    operator dataspace() const;

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

    /** create attribute for given object */
    template <typename h5xxObject>
    attribute(h5xxObject const& object, std::string const& name, hid_t type_id, dataspace const& space, hid_t acpl_id = H5P_DEFAULT, hid_t aapl_id = H5P_DEFAULT);

    /** open existing attribute of given name, attached to the HDF5 object */
    template <typename h5xxObject>
    attribute(h5xxObject const& object, std::string const& name, hid_t aapl_id = H5P_DEFAULT);

    /** construct from HDF5 object ID */
    attribute(hid_t hid) : hid_(hid) {}

    /** write attribute */
    void write(hid_t mem_type_id, void const* value);

    /** read attribute */
    void read(hid_t mem_type_id, void* buffer);

private:
    /** HDF5 object ID */
    hid_t hid_;

    template <typename h5xxObject>
    friend void swap(h5xxObject& left, h5xxObject& right);
};

template <typename h5xxObject>
attribute::attribute(h5xxObject const& object, std::string const& name, hid_t type_id, dataspace const& space, hid_t acpl_id, hid_t aapl_id)
{
    if ((hid_ = H5Acreate(object.hid(), name.c_str(), type_id, space.hid(), acpl_id, aapl_id)) < 0 )
    {
        throw error("creating attribute \"" + name + "\"");
    }
}

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

attribute::attribute(attribute const& other)
  : hid_(other.hid_)
{
    // copying would be safe if the exception were disabled.
    throw error("h5xx::attribute can not be copied. Copying must be elided by return value optimisation.");
    H5Iinc_ref(hid_);
}

attribute const& attribute::operator=(attribute other)
{
    swap(*this, other);
    return *this;
}

attribute::~attribute()
{
    if (hid_ >= 0) {
        if(H5Aclose(hid_) < 0){
            throw error("closing h5xx::attribute with ID " + boost::lexical_cast<std::string>(hid_));
        }
    }
}

attribute::operator dataspace() const
{
    hid_t hid = H5Aget_space(hid_);
    if(hid < 0 ) {
        throw error("attribute has invalid dataspace");
    }
    dataspace ds(hid);
    return ds;
}

void attribute::write(hid_t mem_type_id, void const* value)
{
    if (H5Awrite(hid_, mem_type_id, value) < 0)
    {
        throw error("writing attribute with id ");
    }
}

void attribute::read(hid_t mem_type_id, void * buffer)
{
    if (H5Aread(hid_, mem_type_id, buffer) < 0)
    {
        throw error("reading attribute");
    }
}

} // namespace h5xx

#endif /* ! H5XX_ATTRIBUTE_ATTRIBUTE_HPP */
