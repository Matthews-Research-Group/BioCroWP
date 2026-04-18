#include "module_library.h"
#include "../framework/module_creator.h"  // for create_mc

// Include all the header files that define the modules.
#include "example_module.h"
#include "soil_temperature.h"
#include "soil_potential.h"
#include "osmotic_potential.h"
#include "longitudinal_extensibility_update.h"
#include "pressure_potential_senescence.h"
#include "total_potential.h"
//#include "transport_balance.h"
#include "calculate_flux.h"
#include "total_biomass.h"

creator_map BioCroWP::module_library::library_entries =
{
    {"example_module", &create_mc<example_module>},
    {"soil_temperature", &create_mc<soil_temperature>},
    {"soil_potential", &create_mc<soil_potential>},
    {"osmotic_potential", &create_mc<osmotic_potential>},
    {"longitudinal_extensibility_update", &create_mc<longitudinal_extensibility_update>},
    {"pressure_potential_senescence", &create_mc<pressure_potential_senescence>},
    {"total_potential", &create_mc<total_potential>},
    //{"transport_balance", &create_mc<transport_balance>},
    {"calculate_flux", &create_mc<calculate_flux>},
    {"total_biomass", &create_mc<total_biomass>}
};
