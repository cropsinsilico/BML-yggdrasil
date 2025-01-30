#include <cmath>      // for pow, sqrt
#include <algorithm>  // for std::min, std::max
#include <limits>     // fot std::numeric_limits
#include "ephotosynthesis.h"
#include "ball_berry.hpp"
#include "../framework/constants.h"  // for ideal_gas_constant, celsius_to_kelvin
#include "AuxBioCro.h"

#include <stdlib.h>
#include <string>
#include <map>
#include <sstream>
#include <vector>
#include <iterator>

using yggdrasilBML::ephotosynthesis;

//This is now using the ephoto function
//the original FvCB has been renamed to c3photoC_FvCB
void ephotosynthesis::c3photoC(struct c3_str& param,
                               rapidjson::Document& state) const {
    UNPACK_C3_STR(param);
    
    double Ca = Catm;
    double const AP = atmospheric_pressure;
    
    // Get leaf temperature in Kelvin
    double const Tleaf_K =
        Tleaf + conversion_constants::celsius_to_kelvin;  // K

    Rd_T = Rd * arrhenius_exponential(18.72, 46.39e3, Tleaf_K);        // micromol / m^2 / s

    if (Ca <= 0) {
        Ca = 1e-4;  // micromol / mol
    }

    double const Ca_pa = Ca * 1e-6 * AP;  // Pa.

    // Initialize variables before running fixed point iteration in a loop
    double Ci_pa = 30.0;                  // Pa                 (initial guess)
    co2_assim_ephoto = 0.0;
    co2_assimilation_rate = 0.0;  // micromol / m^2 / s (initial guess)
    double const Tol = 0.01;             // micromol / m^2 / s
    int iterCounter = 0;
    int max_iter = 10;
    double penalty = 0.0;

    // Run iteration loop
    while (iterCounter < max_iter) {
        double OldAssim = co2_assimilation_rate;  // micromol / m^2 / s
        Ci = (Ci_pa / AP) * 1e6;                  // micromol / mol

        //call ephotosynthesis
        state.SetObject();
        state.AddMember("Tp", Tleaf, state.GetAllocator());
        state.AddMember("CO2_in", Ci, state.GetAllocator());
        state.AddMember("TestLi", incident_ppfd, state.GetAllocator());
        ygg_direct_module::do_call(state);
        co2_assim_ephoto = _safe_get(state, "CO2AR");
        
        //ephoto returns the Gross A, which should not be negative!
        if (co2_assim_ephoto < 0) co2_assim_ephoto = 0 ;
        //now we overwrite the FvCB's An, make sure to minus the Rd!
        co2_assimilation_rate = co2_assim_ephoto - Rd_T; 


        if (water_stress_approach == 0) {
            co2_assimilation_rate *= StomataWS;  // micromol / m^2 / s
        }

        //Here we hard limit the co2_assimilation_rate because sometimes the ephoto's result 
        //can be over 300 micromol and cause the ball_berry to stop! 
        //Why hard limit by 60 micromol? Because
        //in the multilayer_canopy_integrator.h, we remove any An above 50
        //So, 60 can make sure this "wrong" result to be removed in the canopy integration
        co2_assimilation_rate = std::min(co2_assimilation_rate,60.0);
        

        Gs = ball_berry(co2_assimilation_rate * 1e-6, Ca * 1e-6, rh, b0, b1) * 1e-3;  // mol / m^2 / s

        if (water_stress_approach == 1) {
            Gs = Gs_min + StomataWS * (Gs - Gs_min);  // mol / m^2 / s
        }

        if (Gs <= 0) {
            Gs = 1e-8;  // mol / m^2 / s
        }

        Ci_pa = Ca_pa - (co2_assimilation_rate * 1e-6) * 1.6 * AP / Gs;  // Pa

        if (Ci_pa < 0) {
            Ci_pa = 1e-5;  // Pa
        }

        if (abs(OldAssim - co2_assimilation_rate) < Tol) {
            break;
        }

        ++iterCounter;
    }
}

void ephotosynthesis::c3photoC_FvCB(struct c3_str& param) const {
    UNPACK_C3_STR(param);
    
    double Ca = Catm;
    double const AP = atmospheric_pressure;
    
    // Get leaf temperature in Kelvin
    double const Tleaf_K =
        Tleaf + conversion_constants::celsius_to_kelvin;  // K

    // Define the leaf reflectance
    double const leaf_reflectance = 0.2;  // dimensionless

    // Temperature corrections are from the following sources:
    // - Bernacchi et al. (2003) Plant, Cell and Environment, 26(9), 1419-1430.
    //   https://doi.org/10.1046/j.0016-8025.2003.01050.x
    // - Bernacchi et al. (2001) Plant, Cell and Environment, 24(2), 253-259.
    //   https://doi.org/10.1111/j.1365-3040.2001.00668.x
    // Note: Values in Dubois and Bernacchi are incorrect.
    double const Kc = arrhenius_exponential(38.05, 79.43e3, Tleaf_K);              // micromol / mol
    double const Ko = arrhenius_exponential(20.30, 36.38e3, Tleaf_K);              // mmol / mol
    double const Gstar = arrhenius_exponential(19.02, 37.83e3, Tleaf_K);           // micromol / mol
    double const Vcmax = vmax1 * arrhenius_exponential(26.35, 65.33e3, Tleaf_K);  // micromol / m^2 / s
    double const Jmax = jmax * arrhenius_exponential(17.57, 43.54e3, Tleaf_K);    // micromol / m^2 / s
    Rd_T = Rd * arrhenius_exponential(18.72, 46.39e3, Tleaf_K);        // micromol / m^2 / s

    double const theta_T = theta + 0.018 * Tleaf - 3.7e-4 * pow(Tleaf, 2);  // dimensionless

    // Light limited
    double const dark_adapted_phi_PSII =
        0.352 + 0.022 * Tleaf - 3.4 * pow(Tleaf, 2) / 1e4;  // dimensionless (Bernacchi et al. (2003))

    double const I2 =
        incident_ppfd * dark_adapted_phi_PSII * (1.0 - leaf_reflectance) / 2.0;  // micromol / m^2 / s

    double const J =
        (Jmax + I2 - sqrt(pow(Jmax + I2, 2) - 4.0 * theta_T * I2 * Jmax)) /
        (2.0 * theta_T);  // micromol / m^2 / s

    double const Oi = O2 * solo(Tleaf);  // mmol / mol

    if (Ca <= 0) {
        Ca = 1e-4;  // micromol / mol
    }

    double const Ca_pa = Ca * 1e-6 * AP;  // Pa.

    // TPU rate temperature dependence from Figure 7, Yang et al. (2016) Planta,
    // 243, 687-698. https://doi.org/10.1007/s00425-015-2436-8
    //
    // In Yang et al., the equation in the caption of Figure 7 calculates the
    // maximum rate of TPU utilization, but here we need the rate relative to
    // its value at 25 degrees C (as shown in the figure itself). Using the
    // equation, the rate at 25 degrees C can be found to have the value
    // 306.742, so here we normalize the equation by this value.
    double const TPU_c = 25.5;                                               // dimensionless (fitted constant)
    double const Ha = 62.99e3;                                               // J / mol (enthalpy of activation)
    double const S = 0.588e3;                                                // J / K / mol (entropy)
    double const Hd = 182.14e3;                                              // J / mol (enthalpy of deactivation)
    double const R = physical_constants::ideal_gas_constant;                 // J / K / mol (ideal gas constant)
    double const top = Tleaf_K * arrhenius_exponential(TPU_c, Ha, Tleaf_K);  // dimensionless
    double const bot = 1.0 + arrhenius_exponential(S / R, Hd, Tleaf_K);      // dimensionless
    double TPU_rate_multiplier = (top / bot) / 306.742;                      // dimensionless

    double TPU = tpu_rate_max * TPU_rate_multiplier;  // micromol / m^2 / s

    // The alpha constant for calculating Ap is from Eq. 2.26, von Caemmerer, S.
    // Biochemical models of leaf photosynthesis.
    double const alpha_TPU = 0.0;  // dimensionless. Without more information, alpha=0 is often assumed.

    // Initialize variables before running fixed point iteration in a loop
    double Ci_pa = 0.0;                  // Pa                 (initial guess)
    double const Tol = 0.01;             // micromol / m^2 / s
    int iterCounter = 0;
    int max_iter = 1000;

    // Run iteration loop
    while (iterCounter < max_iter) {
        double OldAssim = co2_assimilation_rate;  // micromol / m^2 / s
        Ci = (Ci_pa / AP) * 1e6;                  // micromol / mol

        // Calculate the net CO2 assimilation rate using the method described in
        // "Avoiding Pitfalls When Using the FvCB Model"
        if (Ci == 0.0) {
            // RuBP-saturated net assimilation rate when Ci is 0
            double Ac0 = -Gstar * Vcmax / (Kc * (1 + Oi / Ko)) - Rd_T;  // micromol / m^2 / s

            // RuBP-regeneration-limited net assimilation when C is 0
            double Aj0 =
                -J / (2.0 * electrons_per_oxygenation) - Rd_T;  // micromol / m^2 / s

            co2_assimilation_rate = std::max(Ac0, Aj0);  // micromol / m^2 / s
        } else {
            // RuBP-saturated carboxylation rate
            double Wc = Vcmax * Ci /
                        (Ci + Kc * (1.0 + Oi / Ko));  // micromol / m^2 / s

            // RuBP-regeneration-limited carboxylation rate
            double Wj = J * Ci /
                        (electrons_per_carboxylation * Ci +
                         2.0 * electrons_per_oxygenation * Gstar);  // micromol / m^2 / s

            // Triose-phosphate-utilization-limited carboxylation rate. There is
            // an asymptote at Ci = Gstar * (1 + 3 * alpha_TPU), and TPU cannot
            // limit the carboxylation rate for values of Ci below this
            // asymptote. A simple way to handle this is to make Wp infinite for
            // Ci <= Gstar * (1 + 3 * alpha_TPU), so that it is never limiting
            // in this case.
            double Wp = Ci > Gstar * (1.0 + 3.0 * alpha_TPU)
                            ? 3.0 * TPU * Ci / (Ci - Gstar * (1.0 + 3.0 * alpha_TPU))
                            : std::numeric_limits<double>::infinity();  // micromol / m^2 / s

            // Limiting carboxylation rate
            double Vc = std::min(Wc, std::min(Wj, Wp));  // micromol / m^2 / s

            co2_assimilation_rate = (1.0 - Gstar / Ci) * Vc - Rd_T;  // micromol / m^2 / s
        }

        if (water_stress_approach == 0) {
            co2_assimilation_rate *= StomataWS;  // micromol / m^2 / s
        }

        Gs = ball_berry(co2_assimilation_rate * 1e-6, Ca * 1e-6, rh, b0, b1) * 1e-3;  // mol / m^2 / s

        if (water_stress_approach == 1) {
            Gs = Gs_min + StomataWS * (Gs - Gs_min);  // mol / m^2 / s
        }

        if (Gs <= 0) {
            Gs = 1e-8;  // mol / m^2 / s
        }

        Ci_pa = Ca_pa - (co2_assimilation_rate * 1e-6) * 1.6 * AP / Gs;  // Pa

        if (Ci_pa < 0) {
            Ci_pa = 1e-5;  // Pa
        }

        if (abs(OldAssim - co2_assimilation_rate) < Tol) {
            break;
        }

        ++iterCounter;
    }
}

// This function returns the solubility of O2 in H2O relative to its value at
// 25 degrees C. The equation used here was developed by forming a polynomial
// fit to tabulated solubility values from a reference book, and then a
// subsequent normalization to the return value at 25 degrees C. For more
// details, See Long, Plant, Cell & Environment 14, 729–739 (1991)
// (https://doi.org/10.1111/j.1365-3040.1991.tb01439.x).
double ephotosynthesis::solo(
    double LeafT  // degrees C
)
{
    return (0.047 - 0.0013087 * LeafT + 2.5603e-05 * pow(LeafT, 2) - 2.1441e-07 * pow(LeafT, 3)) / 0.026934;
}
