/* This file will contain functions which are common to several */
/* routines in the BioCro package. These are functions needed */
/* internally. The normal user will not need them */

#include <stdexcept>
#include <string>
#include <cmath>
// #include "BioCro.h"
#include "../framework/constants.h"  // for pi, e, atmospheric_pressure_at_sea_level,
                           // ideal_gas_constant, molar_mass_of_water,
                           // stefan_boltzmann, celsius_to_kelvin

/* Additional Functions needed for EvapoTrans */

/**
 *  @brief Determines the density of dry air from the air temperature.
 *
 *  This function appears to be derived from fitting a linear equation to the
 *  values of `rho` in Table 14.3 on pg. 408 in Thornley and Johnson (1990):
 *
 *  | T (degrees C) | `rho` (kg / m^3) |
 *  | :-----------: | :--------------: |
 *  |  0            | 1.29             |
 *  |  5            | 1.27             |
 *  | 10            | 1.25             |
 *  | 15            | 1.23             |
 *  | 20            | 1.20             |
 *  | 25            | 1.18             |
 *  | 30            | 1.16             |
 *  | 35            | 1.15             |
 *  | 40            | 1.13             |
 *
 *  Thornley, J.H.M. and Johnson, I.R. (1990) Plant and Crop Modelling. A
 *  Mathematical Approach to Plant and Crop Physiology.
 *
 *  @param [in] air_temperature Air temperature in degrees C
 *
 *  @return Density of dry air in kg / m^3
 */
double TempToDdryA(
    double air_temperature  // degrees C
)
{
    return 1.295163636 + -0.004258182 * air_temperature;  // kg / m^3
}

/**
 *  @brief Calculate the density of dry air from temperature and pressure using
 *  the ideal gas law.
 *
 *  See https://en.wikipedia.org/wiki/Density_of_air for details.
 *
 *  @param [in] air_temperature Air temperature in kelvin
 *
 *  @param [in] air_pressure Air pressure in Pa
 *
 *  @return Density of dry air in kg / m^3
 */
double dry_air_density(
    const double air_temperature,  // kelvin
    const double air_pressure      // Pa
)
{
    constexpr double r_specific = physical_constants::ideal_gas_constant /
        physical_constants::molar_mass_of_dry_air; // J / kg / K

    return air_pressure / r_specific / air_temperature;  // kg / m^3
}

/**
 *  @brief Determine the latent heat of vaporization for water from its
 *  temperature.
 *
 *  This function appears to be derived from fitting a linear equation to the
 *  values of `lambda` in Table 14.3 on pg. 408 in Thornley and Johnson (1990):
 *
 *  | T (degrees C) | `lambda` (J / kg) |
 *  | :-----------: | :---------------: |
 *  |  0            | 2.50              |
 *  |  5            | 2.49              |
 *  | 10            | 2.48              |
 *  | 15            | 2.47              |
 *  | 20            | 2.45              |
 *  | 25            | 2.44              |
 *  | 30            | 2.43              |
 *  | 35            | 2.42              |
 *  | 40            | 2.41              |
 *
 *  Thornley, J.H.M. and Johnson, I.R. (1990) Plant and Crop Modelling. A
 *  Mathematical Approach to Plant and Crop Physiology.
 *
 *  @param [in] temperature Water temperature in degrees C
 *
 *  @return Latent heat of vaporization for water in J / kg
 */
double TempToLHV(
    double temperature  // degrees C
)
{
    return 2501000 + -2372.727 * temperature;  // J / kg.
}

/**
 *  @brief Determine the derivative of saturation water vapor pressure with
 *  respect to temperature at a particular value of air temperature.
 *
 *  This function appears to be derived from fitting a quadratic function to the
 *  values of `s` in Table 14.3 on pg. 408 in Thornley and Johnson (1990):
 *
 *  | T (degrees C) | `s` (10^(-3) kg / m^3 / K) |
 *  | :-----------: | :------------------------: |
 *  |  0            | 0.33                       |
 *  |  5            | 0.45                       |
 *  | 10            | 0.60                       |
 *  | 15            | 0.78                       |
 *  | 20            | 1.01                       |
 *  | 25            | 1.30                       |
 *  | 30            | 1.65                       |
 *  | 35            | 2.07                       |
 *  | 40            | 2.57                       |
 *
 *  Thornley, J.H.M. and Johnson, I.R. (1990) Plant and Crop Modelling. A
 *  Mathematical Approach to Plant and Crop Physiology.
 *
 *  @param [in] air_temperature Air temperature in degrees C
 *
 *  @return Derivative of saturation water vapor pressure with respect to
 *  temperature in kg / m^3 / K (equivalent to Pa / K)
 */
double TempToSFS(
    double air_temperature  // degrees C
)
{
    return (0.338376068 + 0.011435897 * air_temperature + 0.001111111 *
            pow(air_temperature, 2)) * 1e-3;  //  kg / m^3 / K
}

/**
 * @brief Determine saturation water vapor pressure (Pa) from air temperature
 * (degrees C) using the Arden Buck equation.
 *
 * Equations of this form were used by Arden Buck to model saturation water
 * vapor pressure in 1981: [A. L. Buck, J. Appl. Meteor. 20, 1527–1532 (1981)]
 * (https://doi.org/10.1175/1520-0450(1981)020%3C1527:NEFCVP%3E2.0.CO;2)
 *
 * In 1996, some of the fitting coefficients were updated. Here we use these values,
 * found in [a Buck hygrometer manual]
 * (http://www.hygrometers.com/wp-content/uploads/CR-1A-users-manual-2009-12.pdf),
 * and also displayed on the [Wikipedia page for the Arden Buck equation]
 * (https://en.wikipedia.org/wiki/Arden_Buck_equation).
 *
 * We use the values for vapor over water (rather than ice) and disregard the
 * "enhancement factor" since we are only concerned with the pressure at
 * saturation.
 *
 *  @param [in] air_temperature Air temperature in degrees C
 *
 *  @return Saturation water vapor pressure in Pa
 */
double saturation_vapor_pressure(
    double air_temperature  // degrees C
)
{
    double a = (18.678 - air_temperature / 234.5) * air_temperature;
    double b = 257.14 + air_temperature;
    return 611.21 * exp(a / b);  // Pa
}

/**
 *  @brief Caluclates the conductance for water vapor flow between the leaf
 *  surface and the atmosphere (AKA, the boundary layer conductance) using a
 *  model described in Thornley and Johnson (1990).
 *
 *  This model considers gas flow due to wind-driven eddy currents. Here, the
 *  conductance is calculated using Equation 14.9 from pages 414 - 416 of the
 *  Thornley textbook. Unfortunately, an electronic version of this reference is
 *  not available.
 *
 *  In this model, the minimum possible boundary layer conductance that could
 *  occur is zero, which would correspond to zero wind speed or canopy height.
 *  In realistic field conditions, boundary layer conductance can never truly be
 *  zero. To accomodate this, an option is provided for setting a minimum value
 *  for the boundary layer counductance.
 *
 *  This model contains two singularities, which occur when either of the
 *  following conditions are met:
 *
 *  - `WindSpeedHeight + Zeta - d = 0`, which is equivalent to `CanopyHeight =
 *     WindSpeedHeight / (dCoef - ZetaCoef) = WindSpeedHeight * 1.34`
 *
 *  - `WindSpeedHeight + ZetaM - d = 0`, which is equivalent to `CanopyHeight =
 *     WindSpeedHeight / (dCoef - ZetaMCoef) = WindSpeedHeight * 1.56`
 *
 *  So, as the canopy height approaches or exceeds the height at which wind
 *  speed was measured, the calculated boundary layer conductance becomes
 *  unbounded. For even larger canopy heights, the conductance eventually begins
 *  to decrease. For tall crops, this is a severe limitation to this model. Here
 *  we address this issue by limiting the canopy height to
 *  `0.98 * WindSpeedHeight`.
 *
 *  References:
 *
 *  - Thornley, J. H. M. & Johnson, I. R. "Plant and Crop Modelling: A
 *    Mathematical Approach to Plant and Crop Physiology" (1990)
 *
 *  @param [in] CanopyHeight The height of the canopy above the ground in m
 *
 *  @param [in] WindSpeed The wind speed in m / s as measured above the canopy
 *              at a reference height of five meters
 *
 *  @param [in] minimum_gbw The lowest possible value for boundary layer
 *              conductance in m / s that should be returned
 *
 *  @param [in] WindSpeedHeight The height in m at which the wind speed was
 *              measured
 *
 *  @return The boundary layer conductance in m / s
 */
double leaf_boundary_layer_conductance_thornley(
    double CanopyHeight,    // m
    double WindSpeed,       // m / s
    double minimum_gbw,     // m / s
    double WindSpeedHeight  // m
)
{
    // Define constants used in the model
    constexpr double kappa = 0.41;         // dimensionless. von Karmon's constant. Thornley and Johnson pgs 414 and 416.
    constexpr double ZetaCoef = 0.026;     // dimensionless, Thornley and Johnson 1990, Eq. 14.9o
    constexpr double ZetaMCoef = 0.13;     // dimensionless, Thornley and Johnson 1990, Eq. 14.9o
    constexpr double dCoef = 0.77;         // dimensionless, Thornley and Johnson 1990, Eq. 14.9o.
                                           // In the original text this value is reported as 0.64.
                                           // In the 2000 reprinting of this text, the authors state that this value should be 0.77.
                                           // See "Errata to the 2000 printing" on the page after the preface of the 2000 reprinting of the 1990 text.

    // Apply the height limit
    CanopyHeight = std::min(CanopyHeight, 0.98 * WindSpeedHeight);  // meters

    // Calculate terms that depend on the canopy height
    const double Zeta = ZetaCoef * CanopyHeight;    // meters
    const double Zetam = ZetaMCoef * CanopyHeight;  // meters
    const double d = dCoef * CanopyHeight;          // meters

    // Calculate the boundary layer conductance `ga` according to Thornley and
    // Johnson Eq. 14.9n, pg. 416
    const double ga0 = pow(kappa, 2) * WindSpeed;                   // m / s
    const double ga1 = log((WindSpeedHeight + Zeta - d) / Zeta);    // dimensionless
    const double ga2 = log((WindSpeedHeight + Zetam - d) / Zetam);  // dimensionless
    const double gbv = ga0 / (ga1 * ga2);                           // m / s

    // Apply the minimum
    return std::max(gbv, minimum_gbw);  // m / s
}
