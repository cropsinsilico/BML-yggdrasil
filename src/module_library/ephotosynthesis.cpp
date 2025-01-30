#include "ephotosynthesis.h"
#include "../framework/constants.h"
#ifdef WITH_YGGDRASIL

using yggdrasilBML::ephotosynthesis;

string_vector ephotosynthesis::get_inputs()
{
    return {
        "incident_ppfd",                // micromol / (m^2 leaf) / s
        "temp",                         // deg. C
        "rh",                           // dimensionless
        "vmax1",                        // micromole / m^2 / s
        "jmax",                         // micromole / m^2 / s
        "tpu_rate_max",                 // micromole / m^2 / s
        "Rd",                           // micromole / m^2 / s
        "b0",                           // mol / m^2 / s
        "b1",                           // dimensionless
        "Gs_min",                       // mol / m^2 / s
        "Catm",                         // micromole / mol
        "atmospheric_pressure",         // Pa
        "O2",                           // mmol / mol
        "theta",                        // dimensionless
        "StomataWS",                    // dimensionless
        "water_stress_approach",        // a dimensionless switch
        "electrons_per_carboxylation",  // electron / carboxylation
        "electrons_per_oxygenation",    // electron / oxygenation
        "average_absorbed_shortwave",   // J / (m^2 leaf) / s
        "windspeed",                    // m / s
        "height",                       // m
        "specific_heat_of_air",         // J / kg / K
        "minimum_gbw",                  // mol / m^2 / s
        "windspeed_height",             // m
        "enzyme_sf"                     // dimensionless 
    };
}

string_vector ephotosynthesis::get_outputs()
{
    return {
        "Assim",             // micromole / m^2 /s
        "GrossAssim",        // micromole / m^2 /s
        "Ci",                // micromole / mol
        "Gs",                // mmol / m^2 / s
        // "iterTimes",         // 
        // "penalty",           // 
        "TransR",            // mmol / m^2 / s
        "EPenman",           // mmol / m^2 / s
        "EPriestly",         // mmol / m^2 / s
        "leaf_temperature",  // deg. C
        "gbw"                // mol / m^2 / s
    };
}

void ephotosynthesis::prepareInput(rapidjson::Document&) const {
    // Don't do anything
  // state.SetObject();
  // // TODO: Get values from output?
  // state.AddMember("Tp", Tleaf, state.GetAllocator());
  // state.AddMember("CO2_in", Ci, state.GetAllocator());
  // state.AddMember("TestLi", incident_ppfd, state.GetAllocator());
  // // state.AddMember("sensitivity_sf", enzyme_sf, state.GetAllocator());
  // // Other variables that could be passed:
  // // - O2
}
void ephotosynthesis::processOutput(rapidjson::Document&) const {
    // Don't do anything
  // co2_assim_ephoto = _safe_get(state, "CO2AR");
  // // penalty = _safe_get(state, "penalty");
}

void ephotosynthesis::do_call(rapidjson::Document& state) const
{
    struct c3_str param;
    
    // Get an initial estimate of stomatal conductance, assuming the leaf is at
    // air temperature
    // YH:I use FvCB c3 here just to get the initial Gs quickly
    c3photoC_FvCB(param);

    // Calculate a new value for leaf temperature using the estimate for
    // stomatal conductance
    c3EvapoTrans(param);

    param.Tleaf = temp + param.Deltat;  // deg. C

    // Calculate final values for assimilation, stomatal conductance, and Ci
    // using the new leaf temperature
    c3photoC(param, state);

    // Update the outputs
    update(Assim_op, param.co2_assimilation_rate); // micromol / m^2 / s
    update(GrossAssim_op, param.co2_assimilation_rate + param.Rd_T); // micromol / m^2 / s
    update(Ci_op, param.Ci);       // micromol / mol
    update(Gs_op, param.Gs * 1e3); // mmol / m^2 / s
    // update(iterTimes_op, photo.iterTimes);
    // update(penalty_op, photo.penalty);
    // TransR has units of kg / m^2 / s.
    // Convert to mm / m^2 / s.
    /* 1e3 - g / kg  */
    /* 18 - g / mol for water */
    /* 1e3 - mmol / mol */
    // TransR has units of kg / m^2 / s. Convert to mmol / m^2 / s using the
    // molar mass of water (in kg / mol) and noting that 1e3 mmol = 1 mol
    double cf = 1e3 / physical_constants::molar_mass_of_water;  // mmol / kg for water
    update(TransR_op, param.TransR * cf);
    update(EPenman_op, param.EPenman * cf);
    update(EPriestly_op, param.EPriestly * cf);
    update(leaf_temperature_op, param.Tleaf);
    update(gbw_op, param.boundary_layer_conductance);
}
#endif // WITH_YGGDRASIL
