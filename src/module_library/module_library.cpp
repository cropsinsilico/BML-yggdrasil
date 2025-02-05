#include "module_library.h"
#include "../framework/module_creator.h"  // for create_mc

// Include all the header files that define the modules.
#include "ephotosynthesis.h"
#include "multilayer_canopy_properties.h"
#include "multilayer_c3_canopy.h"
#include "multilayer_canopy_integrator.h"
#include "ball_berry_module.hpp"
#include "opensimroot.h"

creator_map yggdrasilBML::module_library::library_entries =
{
    {"ball_berry_module", &create_mc<ball_berry_module>},
    {"c3_ephotosynthesis", &create_mc<c3_ephotosynthesis>},
    // {"c4_ephotosynthesis", &create_mc<c4_ephotosynthesis>},
    {"ten_layer_canopy_properties", &create_mc<ten_layer_canopy_properties>},
    {"ten_layer_c3_canopy", &create_mc<ten_layer_c3_canopy>},
    {"ten_layer_canopy_integrator", &create_mc<ten_layer_canopy_integrator>},
    {"opensimroot", &create_mc<opensimroot>}
};
