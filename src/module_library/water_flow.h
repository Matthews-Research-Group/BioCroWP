#ifndef WATER_FLOW_H
#define WATER_FLOW_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace BioCroWP
{

class water_flow : public direct_module
{
    public:
      water_flow(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // get references to input quantities
          root_total_potential{get_input(input_quantities, "root_total_potential")}, // MPa
          stem_total_potential{get_input(input_quantities, "stem_total_potential")},
          leaf_total_potential{get_input(input_quantities, "leaf_total_potential")},
          pods_total_potential{get_input(input_quantities, "pods_total_potential")},

          R_soil_root{get_input(input_quantities, "R_soil_root")},
          R_root_stem{get_input(input_quantities, "R_root_stem")},
          R_stem_leaf{get_input(input_quantities, "R_stem_leaf")},
          R_stem_pods{get_input(input_quantities, "R_stem_pods")},

          root_water_content{get_input(input_quantities, "root_water_content")}, // g
          stem_water_content{get_input(input_quantities, "stem_water_content")},
          leaf_water_content{get_input(input_quantities, "leaf_water_content")},
          pods_water_content{get_input(input_quantities, "pods_water_content")},

          RWC_max{get_input(input_quantities, "RWC_max")},
          SWC_max{get_input(input_quantities, "SWC_max")},
          LWC_max{get_input(input_quantities, "LWC_max")},
          PWC_max{get_input(input_quantities, "PWC_max")},

          soil_potential_avg{get_input(input_quantities, "soil_potential_avg")},

          kGrain{get_input(input_quantities, "kGrain")},

          canopy_transpiration_rate{get_input(input_quantities, "canopy_transpiration_rate")},

          kSeneRoot{get_input(input_quantities, "kSeneRoot")}, // dimensionless
          kSeneStem{get_input(input_quantities, "kSeneStem")}, // fraction of organ senesced
          kSeneLeaf{get_input(input_quantities, "kSeneLeaf")},

          // pointers to output quantities
          F_rwu_op{get_op(output_quantities, "F_rwu")}, // g ha-1 hr-1
          F_root_stem_op{get_op(output_quantities, "F_root_stem")},
          F_stem_leaf_op{get_op(output_quantities, "F_stem_leaf")},
          F_stem_pods_op{get_op(output_quantities, "F_stem_pods")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "water_flow"; }

  private:
    // references to input quantities

    double const& root_total_potential;
    double const& stem_total_potential;
    double const& leaf_total_potential;
    double const& pods_total_potential;

    double const& R_soil_root;
    double const& R_root_stem;
    double const& R_stem_leaf;
    double const& R_stem_pods;

    double const& RWC_max;
    double const& SWC_max;
    double const& LWC_max;
    double const& PWC_max;

    double const& root_water_content;
    double const& stem_water_content;
    double const& leaf_water_content;
    double const& pods_water_content;

    double const& soil_potential_avg;

    double const& kGrain;

    double const& canopy_transpiration_rate;

    double const& kSeneRoot;
    double const& kSeneStem;
    double const& kSeneLeaf;

    // pointers to output quantities
    double* F_rwu_op;
    double* F_root_stem_op;
    double* F_stem_leaf_op;
    double* F_stem_pods_op;

    // main operation
    void do_operation() const;

};

string_vector water_flow::get_inputs()
{
  return {

    "root_total_potential", // g
    "stem_total_potential",
    "leaf_total_potential",
    "pods_total_potential",

    "R_soil_root",          // MPa h g-1 ha
    "R_root_stem",
    "R_stem_leaf",
    "R_stem_pods",

    "RWC_max",
    "SWC_max",
    "LWC_max",
    "PWC_max",

    "root_water_content",              // g
    "stem_water_content",
    "leaf_water_content",
    "pods_water_content",

    "soil_potential_avg",
    "kGrain",

    "canopy_transpiration_rate",

    "kSeneRoot", // dimensionless
    "kSeneStem",
    "kSeneLeaf",
  
  };
}

string_vector water_flow::get_outputs()
{
  return {

    "F_rwu", // g
    "F_root_stem",
    "F_stem_leaf",
    "F_stem_pods"

  };
}

void water_flow::do_operation() const
{
  // Planting density factor
  // (150,000 plants/acre)*(1 acre/ 4046.86 m2)*(10,000 m2/ha) = 370658 plants/ha
  // rounding to the nearest plant
  double n_plants = 370658.0;

  double transpiration = (canopy_transpiration_rate*1000000);
  if (transpiration < 0) {
        transpiration = 0; // transpiration should never be negative, setting to 0 if this is the case
    }

  // calculating senescence
  double sene_rwc = kSeneRoot*root_water_content; // fraction of root wet biomass senesced
  double sene_swc = kSeneStem*stem_water_content;
  double sene_lwc = kSeneLeaf*leaf_water_content;

  
  double F_rwu_new = n_plants*((soil_potential_avg - root_total_potential)/R_soil_root); // g ha-1 hr-1
  double F_root_stem_new = n_plants*((root_total_potential - stem_total_potential)/R_root_stem);
  double F_stem_leaf_new = n_plants*((stem_total_potential - leaf_total_potential)/R_stem_leaf);
  double F_stem_pods_new = n_plants*((stem_total_potential - pods_total_potential)/R_stem_pods);

  // if pod growth hasn't started F_stem_pods should be 0
  if (kGrain < 0.005) {
      F_stem_pods_new = 0.0;
  }

  // adjust if flow is higher than maximum plant water content
  // prevent organ water content from going negative
  if ((root_water_content + F_rwu_new - F_root_stem_new - sene_rwc) > RWC_max) { // if new water content is greater than maximum
    F_rwu_new = RWC_max - root_water_content + F_root_stem_new + sene_rwc;       // set flow equal to difference between maximum and new water content
  } 
  else 
  if ((F_rwu_new - F_root_stem_new - sene_rwc) < -1*root_water_content) { // if flow makes water content negative
    F_root_stem_new = root_water_content + F_rwu_new - sene_rwc;                               // set flow so that water content is 0
  }

  if ((stem_water_content + F_root_stem_new - F_stem_leaf_new - F_stem_pods_new) > SWC_max) {
    F_root_stem_new = SWC_max - stem_water_content + F_stem_leaf_new + F_stem_pods_new + sene_swc;
  } 
  else 
  if ((F_root_stem_new - F_stem_leaf_new - F_stem_pods_new) < -1*stem_water_content) {
    F_stem_leaf_new = stem_water_content + F_root_stem_new - sene_swc;
    F_stem_pods_new = 0.0;
  }

  if ((leaf_water_content + F_stem_leaf_new - transpiration) > LWC_max) {
    F_stem_leaf_new = LWC_max - leaf_water_content + transpiration + sene_lwc;
  } 

  if ((pods_water_content + F_stem_pods_new) > PWC_max) {
    F_stem_pods_new = PWC_max - pods_water_content;
  } 
  else 
  if ((F_stem_pods_new) < -1*pods_water_content) {
    F_stem_pods_new = -pods_water_content;
  }

  // update water flow (g/hr)
  update(F_rwu_op, F_rwu_new);
  update(F_root_stem_op, F_root_stem_new);
  update(F_stem_leaf_op, F_stem_leaf_new);
  update(F_stem_pods_op, F_stem_pods_new);
}

} // end of namespace
#endif