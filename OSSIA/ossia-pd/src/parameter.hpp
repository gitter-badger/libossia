#pragma once

#include "device.hpp"
#include "model.hpp"
#include "remote.hpp"
#include <ossia-pd/src/parameter_base.hpp>

namespace ossia
{
namespace pd
{

class parameter : public parameter_base
{
public:
  using is_model = std::true_type;

  parameter();

  bool register_node(const std::vector<ossia::net::node_base*>& node);
  bool do_registration(const std::vector<ossia::net::node_base*>& node);
  bool unregister();

  static ossia::safe_vector<parameter*>& quarantine();

  static t_pd_err notify(parameter*x, t_symbol*s, t_symbol* msg, void* sender, void* data);
  static void get_unit(parameter*x);
  static void get_mute(parameter*x);
  static void get_rate(parameter*x);

  void set_unit();

};
} // namespace pd
} // namespace ossia
