#include "module_library.h"
#include "../framework/module_creator.h"  // for create_mc

// Include all the header files that define the modules.
#include "ephotosynthesis.h"
#include "opensimroot.h"

creator_map yggdrasilBML::module_library::library_entries =
{
    {"ephotosynthesis", &create_mc<ephotosynthesis>},
    {"opensimroot", &create_mc<opensimroot>}
};
