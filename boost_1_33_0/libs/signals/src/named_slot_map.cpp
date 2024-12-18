// Boost.Signals library

// Copyright Douglas Gregor 2001-2004. Use, modification and
// distribution is subject to the Boost Software License, Version
// 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// For more information, see http://www.boost.org

#define BOOST_SIGNALS_SOURCE

#include <boost/signals/detail/named_slot_map.hpp>
#include <cassert>
#include <map>
#include <list>
#include <typeinfo>

namespace boost { namespace BOOST_SIGNALS_NAMESPACE { namespace detail {

typedef std::list<connection_slot_pair> group_list;
typedef group_list::iterator slot_pair_iterator;
typedef std::map<any, group_list, compare_type> slot_container_type;
typedef slot_container_type::iterator group_iterator;
typedef slot_container_type::const_iterator const_group_iterator;

named_slot_map_iterator::named_slot_map_iterator() : slot_assigned(false) {}

named_slot_map_iterator::
named_slot_map_iterator(const named_slot_map_iterator& other)
  : group(other.group), last_group(other.last_group), 
    slot_assigned(other.slot_assigned)
{
  if (slot_assigned) slot_ = other.slot_;
}

named_slot_map_iterator& 
named_slot_map_iterator::operator=(const named_slot_map_iterator& other)
{
  slot_assigned = other.slot_assigned;
  group = other.group;
  last_group = other.last_group;
  if (slot_assigned) slot_ = other.slot_;
  return *this;
}


connection_slot_pair& named_slot_map_iterator::dereference() const
{ return *slot_; }

void named_slot_map_iterator::increment()
{
  ++slot_;
  if (slot_ == group->second.end()) {
    ++group;
    init_next_group();
  }
}

bool 
named_slot_map_iterator::equal(const named_slot_map_iterator& other) const
{
  return (group == other.group
          && (group == last_group
              || slot_ == other.slot_));
}

#if BOOST_WORKAROUND(_MSC_VER, <= 0x1701)
void named_slot_map_iterator::decrement() { assert(false); }
void named_slot_map_iterator::advance(difference_type) { assert(false); }
#endif

named_slot_map::named_slot_map(const compare_type& compare) : groups(compare)
{
  clear();
}

void named_slot_map::clear() 
{ 
  groups.clear();
  groups[front_type()];
  groups[back_type()];
  back = groups.end();
  --back;
}

named_slot_map::iterator named_slot_map::begin()
{
  return named_slot_map::iterator(groups.begin(), groups.end());
}

named_slot_map::iterator named_slot_map::end()
{
  return named_slot_map::iterator(groups.end(), groups.end());
}

named_slot_map::iterator
named_slot_map::insert(const any& name, const connection& con, const any& slot,
                       connect_position at)
{
  group_iterator group;
  if (name.empty()) {
    switch (at) {
    case at_front: group = groups.begin(); break;
    case at_back: group = back; break;
    }
  } else {
    group = groups.find(name);
    if (group == groups.end()) {
      slot_container_type::value_type v(name, group_list());
      group = groups.insert(v).first;
    }
  }
  iterator it;
  it.group = group;
  it.last_group = groups.end();

  switch (at) {
  case at_back:
    group->second.push_back(connection_slot_pair(con, slot));
    it.slot_ = group->second.end();
    it.slot_assigned = true;
    --(it.slot_);
    break;

  case at_front:
    group->second.push_front(connection_slot_pair(con, slot));
    it.slot_ = group->second.begin();
    it.slot_assigned = true;
    break;
  }
  return it;
}

void named_slot_map::disconnect(const any& name)
{
  group_iterator group = groups.find(name);
  if (group != groups.end()) {
    slot_pair_iterator i = group->second.begin();
    while (i != group->second.end()) {
      slot_pair_iterator next = i;
      ++next;
      i->first.disconnect();
      i = next;
    }
    groups.erase(group);
  }
}

void named_slot_map::erase(iterator pos)
{
  // Erase the slot
  pos.slot_->first.disconnect();
  pos.group->second.erase(pos.slot_);
}

void named_slot_map::remove_disconnected_slots()
{
  // Remove any disconnected slots
  group_iterator g = groups.begin();
  while (g != groups.end()) {
    slot_pair_iterator s = g->second.begin();
    while (s != g->second.end()) {
      if (s->first.connected()) ++s;
      else g->second.erase(s++);
    }

    // Clear out empty groups
    if (empty(g)) groups.erase(g++);
    else ++g;
  }
}


} } }
