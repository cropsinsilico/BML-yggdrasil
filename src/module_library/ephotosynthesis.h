#ifndef EPHOTOSYNTHESIS_H
#define EPHOTOSYNTHESIS_H

#include "../framework/state_map.h"
#include "../framework/module.h"

#ifdef WITH_YGGDRASIL

#include "yggdrasil_modules.h"

namespace yggdrasilBML
{
    
struct c3_str {
    double Tleaf;                      // degrees C
    double Ci;                         // micromol / mol
    double co2_assim_ephoto;           // micromol / m^2 / s
    double co2_assimilation_rate;      // micromol / m^2 / s
    double Rd_T;                       // micromol / m^2 / s
    double Gs;                         // mol / m^2 / s
    double Deltat;                     // degress C
    double TransR;                     // kg / m^2 / s
    double EPenman;                    // kg / m^2 / s
    double EPriestly;                  // kg / m^2 / s
    double boundary_layer_conductance; // mol / m^2 / s
};
#define UNPACK_C3_STR(param)                                            \
    double& Tleaf = param.Tleaf;                                        \
    double& Ci = param.Ci;                                              \
    double& co2_assim_ephoto = param.co2_assim_ephoto;                  \
    double& co2_assimilation_rate = param.co2_assimilation_rate;        \
    double& Rd_T = param.Rd_T;                                          \
    double& Gs = param.Gs;                                              \
    double& Deltat = param.Deltat;                                      \
    double& TransR = param.TransR;                                      \
    double& EPenman = param.EPenman;                                    \
    double& EPriestly = param.EPriestly;                                \
    double& boundary_layer_conductance = param.boundary_layer_conductance
    
/**
 * @class leaf_photosynthesis
 *
 * @brief Use yggdrasil (Lang 2019) to connect to a leaf photosynthesis
 *   model that uses ePhotosynthesis  (Zhu et al., 2007, Wang et al.)
 *
 *  References:
 *  Lang, M. M. 2019
 *  Zhu, X et al., 2007
 *
 */
class ephotosynthesis : public ygg_direct_module
{
   public:
    ephotosynthesis(
        state_map const& input_quantities,
        state_map* output_quantities)
        : ygg_direct_module(
              "ephotosynthesis",
              get_inputs(), input_quantities,
              get_outputs(), output_quantities),
          // Get references to input quantities
          incident_ppfd(get_input(input_quantities, "incident_ppfd")),
          temp(get_input(input_quantities, "temp")),
          rh(get_input(input_quantities, "rh")),
          vmax1(get_input(input_quantities, "vmax1")),
          jmax(get_input(input_quantities, "jmax")),
          tpu_rate_max(get_input(input_quantities, "tpu_rate_max")),
          Rd(get_input(input_quantities, "Rd")),
          b0(get_input(input_quantities, "b0")),
          b1(get_input(input_quantities, "b1")),
          Gs_min(get_input(input_quantities, "Gs_min")),
          Catm(get_input(input_quantities, "Catm")),
          atmospheric_pressure(get_input(input_quantities, "atmospheric_pressure")),
          O2(get_input(input_quantities, "O2")),
          theta(get_input(input_quantities, "theta")),
          StomataWS(get_input(input_quantities, "StomataWS")),
          water_stress_approach(get_input(input_quantities, "water_stress_approach")),
          electrons_per_carboxylation(get_input(input_quantities, "electrons_per_carboxylation")),
          electrons_per_oxygenation(get_input(input_quantities, "electrons_per_oxygenation")),
          average_absorbed_shortwave(get_input(input_quantities, "average_absorbed_shortwave")),
          windspeed(get_input(input_quantities, "windspeed")),
          height(get_input(input_quantities, "height")),
          specific_heat_of_air(get_input(input_quantities, "specific_heat_of_air")),
          minimum_gbw(get_input(input_quantities, "minimum_gbw")),
          windspeed_height{get_input(input_quantities, "windspeed_height")},
          enzyme_sf{get_input(input_quantities, "enzyme_sf")},

          // Get pointers to output quantities
          Assim_op(get_op(output_quantities, "Assim")),
          GrossAssim_op(get_op(output_quantities, "GrossAssim")),
          Ci_op(get_op(output_quantities, "Ci")),
          Gs_op(get_op(output_quantities, "Gs")),
          // iterTimes_op(get_op(output_quantities, "iterTimes")),
          // penalty_op(get_op(output_quantities, "penalty")),
          TransR_op(get_op(output_quantities, "TransR")),
          EPenman_op(get_op(output_quantities, "EPenman")),
          EPriestly_op(get_op(output_quantities, "EPriestly")),
          leaf_temperature_op(get_op(output_quantities, "leaf_temperature")),
          gbw_op(get_op(output_quantities, "gbw"))
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "ephotosynthesis"; }

   private:
    // References to input quantities
    double const& incident_ppfd;
    double const& temp;
    double const& rh;
    double const& vmax1;
    double const& jmax;
    double const& tpu_rate_max;
    double const& Rd;
    double const& b0;
    double const& b1;
    double const& Gs_min;
    double const& Catm;
    double const& atmospheric_pressure;
    double const& O2;
    double const& theta;
    double const& StomataWS;
    double const& water_stress_approach;
    double const& electrons_per_carboxylation;
    double const& electrons_per_oxygenation;
    double const& average_absorbed_shortwave;
    double const& windspeed;
    double const& height;
    double const& specific_heat_of_air;
    double const& minimum_gbw;
    double const& windspeed_height;
    double const& enzyme_sf;

    // Pointers to output quantities
    double* Assim_op;
    double* GrossAssim_op;
    double* Ci_op;
    double* Gs_op;
    // double* iterTimes_op;
    // double* penalty_op;
    double* TransR_op;
    double* EPenman_op;
    double* EPriestly_op;
    double* leaf_temperature_op;
    double* gbw_op;

    void prepareInput(rapidjson::Document& state) const override;
    void processOutput(rapidjson::Document& state) const override;
    void do_call(rapidjson::Document& state) const override;
    void c3photoC(struct c3_str& param, rapidjson::Document& state) const;
    void c3photoC_FvCB(struct c3_str& param) const;
    void c3EvapoTrans(struct c3_str& param) const;
    static double solo(double LeafT);
    // void prepareInput(rapidjson::Document& state) const override;
    // void processOutput(rapidjson::Document& state) const override;
};

}  // namespace yggdrasilBML
#endif // WITH_YGGDRASIL
#endif
