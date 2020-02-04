/* This file is part of the Palabos library.
 *
 * Copyright (C) 2011-2017 FlowKit Sarl
 * Route d'Oron 2
 * 1010 Lausanne, Switzerland
 * E-mail contact: contact@flowkit.com
 *
 * The most recent release of Palabos can be downloaded at 
 * <http://www.palabos.org/>
 *
 * The library Palabos is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * The library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "atomicBlock/atomicContainerBlock2D.h"

namespace plb {

AtomicContainerBlock2D::AtomicContainerBlock2D(plint nx_, plint ny_)
    : AtomicBlock2D(nx_,ny_),
      data(0)
{ }

AtomicContainerBlock2D::~AtomicContainerBlock2D() {
    delete data;
}

AtomicContainerBlock2D& AtomicContainerBlock2D::operator=(AtomicContainerBlock2D const& rhs) {
    AtomicContainerBlock2D tmp(rhs);
    swap(tmp);
    return *this;
}

AtomicContainerBlock2D::AtomicContainerBlock2D(AtomicContainerBlock2D const& rhs)
    : AtomicBlock2D(rhs),
      data(rhs.data ? rhs.data->clone() : 0)
{ }

void AtomicContainerBlock2D::swap(AtomicContainerBlock2D& rhs) {
    std::swap(data, rhs.data);
    AtomicBlock2D::swap(rhs);
}

void AtomicContainerBlock2D::setData(ContainerBlockData* data_) {
    delete data;
    data = data_;
}

ContainerBlockData* AtomicContainerBlock2D::getData() {
    return data;
}

ContainerBlockData const* AtomicContainerBlock2D::getData() const {
    return data;
}

identifiers::BlockId AtomicContainerBlock2D::getBlockId() const {
    return identifiers::getContainerId();
}

}  // namespace plb