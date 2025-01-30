#include <cmath>
#include <stdexcept>
#include "ephotosynthesis.h"
#include "../framework/constants.h"  // for ideal_gas_constant, molar_mass_of_water,
                                     // stefan_boltzmann, celsius_to_kelvin
#include "AuxBioCro.h"

using yggdrasilBML::ephotosynthesis;

/**
 * Many of the equations used in this function come from Chapter 14 of Thornley
 * and Johnson (1990).
 *
 * Thornley, J.H.M. and Johnson, I.R. (1990) Plant and Crop Modelling. A
 * Mathematical Approach to Plant and Crop Physiology.
 */
void ephotosynthesis::c3EvapoTrans(struct c3_str& param) const {
    UNPACK_C3_STR(param);
    
    double absorbed_shortwave_radiation = average_absorbed_shortwave;
    double air_temperature = temp;
    double stomatal_conductance = Gs * 1e3; // mmol / m^2 / s
  
    const double DdryA = TempToDdryA(air_temperature);               // kg / m^3
    const double LHV = TempToLHV(air_temperature);                   // J / kg
    const double SlopeFS = TempToSFS(air_temperature);               // kg / m^3 / K
    const double SWVP = saturation_vapor_pressure(air_temperature);  // Pa

    // TODO: This is for about 20 degrees C at 100000 Pa. Change it to use the
    // model state. (1 * R * temperature) / pressure
    constexpr double volume_of_one_mole_of_air = 24.39e-3;  // m^3 / mol

    double const minimum_gbw_in_m_per_s = minimum_gbw * volume_of_one_mole_of_air;  // m / s

    if (stomatal_conductance <= 0) {
        throw std::range_error("Thrown in c3EvapoTrans: stomatal conductance is not positive.");
    }

    double conductance_in_m_per_s = stomatal_conductance * 1e-3 * volume_of_one_mole_of_air;  // m / s

    if (rh > 1) {
        throw std::range_error("Thrown in c3EvapoTrans: RH (relative humidity) is greater than 1.");
    }

    /* SOLAR RADIATION COMPONENT*/

    // Convert from vapor pressure to vapor density using the ideal gas law.
    // This is approximately right for temperatures what won't kill plants.
    const double SWVC =
        SWVP / physical_constants::ideal_gas_constant /
        (air_temperature + conversion_constants::celsius_to_kelvin) *
        physical_constants::molar_mass_of_water;  // kg / m^3

    if (SWVC < 0) {
        throw std::range_error("Thrown in c3EvapoTrans: SWVC is less than 0.");
    }

    // Eq. 14.4g from Thornley and Johnson (1990).
    const double PsycParam = DdryA * specific_heat_of_air / LHV;  // kg / m^3 / K

    // Eq. 14.4d from Thornley and Johnson (1990).
    const double DeltaPVa = SWVC * (1 - rh);  // kg / m^3

    /* Calculation of ga */
    const double ga = leaf_boundary_layer_conductance_thornley(
        height,
        windspeed,
        minimum_gbw_in_m_per_s,
        windspeed_height);  // m / s

    if (ga < 0) {
        throw std::range_error("Thrown in c3EvapoTrans: ga is less than zero.");
    }

    /* Temperature of the leaf according to Campbell and Norman (1998) Chp 14.*/
    /* This version is non-iterative and an approximation*/
    /* Stefan-Boltzmann law: B = sigma * T^4. where sigma is the Boltzmann constant. */
    /* From Table A.3 in Campbell and Norman.*/

    /* This is the original from WIMOVAC*/
    Deltat = 0.01;  // degrees C
    double PhiN;
    {
        double ChangeInLeafTemp = 10;  // degrees C
        for (int Counter = 0; (ChangeInLeafTemp > 0.5) && (Counter <= 10); ++Counter) {
            double OldDeltaT = Deltat;

            double rlc = 4.0 * physical_constants::stefan_boltzmann *
                         pow(conversion_constants::celsius_to_kelvin + air_temperature, 3.0) *
                         Deltat;  // W / m^2

            PhiN = absorbed_shortwave_radiation - rlc;  // W / m^2

            // DeltaT equation from Thornley and Johnson 1990, Eq. 14.11e
            double TopValue = PhiN * (1 / ga + 1 / conductance_in_m_per_s) - LHV * DeltaPVa;       // J / m^3
            double BottomValue = LHV * (SlopeFS + PsycParam * (1 + ga / conductance_in_m_per_s));  // J / m^2 / K

            Deltat = TopValue / BottomValue;  // Kelvin. It is also degrees C, because it is a temperature difference.
            Deltat = std::min(std::max(Deltat, -5.0), 5.0);

            ChangeInLeafTemp = fabs(OldDeltaT - Deltat);  // Kelvin. It is also degrees C, because it is a temperature difference.
        }
    }

    if (PhiN < 0) {
        PhiN = 0;
    }

    /* TransR is the Penman-Monteith equation that describe transpiration rate
    * as a function of the energy `PhiN` available for evaporation, the vapour
    * density deficit `DeltaPVa`, the canopy and boundary layer conductances, gc
    * and ga respectively, and the physical parameters `SlopeFS`, `PsycParam`,
    * and `LHV`.
    *
    * Thornley, J.H.M. and Johnson, I.R. (1990) Plant and Crop Modelling. A
    * Mathematical Approach to Plant and Crop Physiology
    */
    TransR =
        (SlopeFS * PhiN + LHV * PsycParam * ga * DeltaPVa) /
        (LHV * (SlopeFS + PsycParam * (1 + ga / conductance_in_m_per_s)));  // kg / m^2 / s

    EPenman =
        (SlopeFS * PhiN + LHV * PsycParam * ga * DeltaPVa) /
        (LHV * (SlopeFS + PsycParam));  // kg / m^2 / s

    EPriestly = 1.26 * (SlopeFS * PhiN / (LHV * (SlopeFS + PsycParam)));  // kg / m^2 / s

    boundary_layer_conductance = ga / volume_of_one_mole_of_air;  // mol / m^2 / s
}
