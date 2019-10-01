/**********************************************************************
 *  This file is part of exact-real.
 *
 *        Copyright (C) 2019 Julian Rüth
 *
 *  exact-real is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  exact-real is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with exact-real. If not, see <https://www.gnu.org/licenses/>.
 *********************************************************************/

#ifndef LIBEXACTREAL_CPPYY_HPP
#define LIBEXACTREAL_CPPYY_HPP

#include <iosfwd>
#include <sstream>
#include <memory>

#include "exact-real/element.hpp"
#include "exact-real/integer_ring.hpp"
#include "exact-real/module.hpp"
#include "exact-real/number_field.hpp"
#include "exact-real/rational_field.hpp"
#include "exact-real/real_number.hpp"
#include "exact-real/yap/arb.hpp"
#include "exact-real/yap/arf.hpp"

// See https://bitbucket.org/wlav/cppyy/issues/95/lookup-of-friend-operator
namespace exactreal {
std::ostream &operator<<(std::ostream &, const exactreal::Arb &);
std::ostream &operator<<(std::ostream &, const exactreal::Arf &);
std::ostream &operator<<(std::ostream &, const exactreal::RealNumber &);
template <typename Ring>
std::ostream &operator<<(std::ostream &, const exactreal::Module<Ring> &);
template <typename Ring>
std::ostream &operator<<(std::ostream &, const exactreal::Element<Ring> &);

namespace cppyy {
// cppyy has trouble looking up very complicated operators so we provide
// some stubs that help it get on the right track:
template <typename T>
Arb eval(T expression, prec prec) {
  Arb ret = std::move(expression)(prec);
  return ret;
}

template <typename T>
Arf eval(T expression, prec prec, int round) {
  Arf ret;
  ret = std::move(expression)(prec, static_cast<Arf::Round>(round));
  return ret;
}

// cppyy does not see the operators provided by boost::operators so we provide
// something to make them explicit here:
template <typename S, typename T>
auto add(const S &lhs, const T &rhs) { return lhs + rhs; }
template <typename S, typename T>
auto sub(const S &lhs, const T &rhs) { return lhs - rhs; }
template <typename S, typename T>
auto mul(const S &lhs, const T &rhs) { return lhs * rhs; }
template <typename S, typename T>
auto truediv(const S &lhs, const T &rhs) { return lhs / rhs; }
template <typename T>
auto neg(const T &value) { return -value; }

// A helper to get RAII that cereal needs to make sure that its output has been flushed.
template <typename T, typename Archive>
std::string serialize(const T& value) {
  std::stringstream serialized;
  {
    Archive archive(serialized);
    archive(value);
  }
  return serialized.str();
}

// For the sake of symmetry, the same for deserialization.
template <typename T, typename Archive>
T deserialize(const std::string& serialized) {
  std::stringstream stream(serialized);
  T value;
  {
    Archive archive(stream);
    archive(value);
  }
  return value;
}
}  // namespace cppyy

}  // namespace exactreal

// Work around https://bitbucket.org/wlav/cppyy/issues/96/cannot-make-wrapper-for-a-function
extern template std::ostream &exactreal::operator<<<exactreal::IntegerRing>(std::ostream &, const exactreal::Module<exactreal::IntegerRing> &);
extern template std::ostream &exactreal::operator<<<exactreal::RationalField>(std::ostream &, const exactreal::Module<exactreal::RationalField> &);
extern template std::ostream &exactreal::operator<<<exactreal::NumberField>(std::ostream &, const exactreal::Module<exactreal::NumberField> &);
extern template std::ostream &exactreal::operator<<<exactreal::IntegerRing>(std::ostream &, const exactreal::Element<exactreal::IntegerRing> &);
extern template std::ostream &exactreal::operator<<<exactreal::RationalField>(std::ostream &, const exactreal::Element<exactreal::RationalField> &);
extern template std::ostream &exactreal::operator<<<exactreal::NumberField>(std::ostream &, const exactreal::Element<exactreal::NumberField> &);

extern template std::shared_ptr<const exactreal::Module<exactreal::IntegerRing>> exactreal::Module<exactreal::IntegerRing>::make(const std::vector<std::shared_ptr<const exactreal::RealNumber>> &, const exactreal::IntegerRing &);
extern template std::shared_ptr<const exactreal::Module<exactreal::RationalField>> exactreal::Module<exactreal::RationalField>::make(const std::vector<std::shared_ptr<const exactreal::RealNumber>> &, const exactreal::RationalField &);
extern template std::shared_ptr<const exactreal::Module<exactreal::NumberField>> exactreal::Module<exactreal::NumberField>::make(const std::vector<std::shared_ptr<const exactreal::RealNumber>> &, const exactreal::NumberField &);

#endif
