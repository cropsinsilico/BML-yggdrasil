#ifndef yggdrasilBML_OPENSIMROOT_H
#define yggdrasilBML_OPENSIMROOT_H

#ifdef WITH_YGGDRASIL

#include "yggdrasil_modules.h"

namespace yggdrasilBML
{
/**
 *
 * @class opensimroot
 *
 * @brief Use yggdrasil (Lang 2019) to connect to an OpenSimRoot model 
 *   running in parallel (Postma et al., 2017)
 *
 *  References:
 *  Lang, M. M. 2019
 *  Postma et al., 2017
 *
 */
class opensimroot : public ygg_direct_timesync_module<opensimroot>
{
public:
    opensimroot(
        state_map const& input_quantities,
        state_map* output_quantities)
        :  // Define basic module properties by passing its name to its parent class
      ygg_direct_timesync_module(input_quantities, output_quantities) {}
  static std::vector<std::string> get_inputs();
  static std::vector<std::string> get_outputs();
  static std::string get_name() { return "opensimroot"; }
};

std::vector<std::string> opensimroot::get_inputs()
{
  // Variables provided by BioCro to OSR
  return {
//    "dRoot",
     "canopy_assimilation_rate",
     "number_of_plants"
    // "d_substrate_root"
  };
}

std::vector<std::string> opensimroot::get_outputs()
{
  // Variables provided by OSR to BioCro
  return {
    "LeafN"
//    "soil_saturated_conductivity",
    // "soil_saturation_capacity",
    // "soil_field_capacity",
    // "soil_wilting_point",
  };
}

}

#endif // WITH_YGGDRASIL

#endif
