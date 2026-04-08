#include <algorithm>
#include <iostream>
#include <cmath>

#include "../framework/module.h"                  // for direct_module and update
#include "../framework/module_helper_functions.h" 
#include "flow_resistance_calculator.h"

flow_resistance_calculator::flow_resistance_calculator(
        state_map const& input_quantities,
        state_map* output_quantities)
        : 
          direct_module(),

          // Get references to input quantities
          root_water_content{get_input(input_quantities, "root_water_content")}, // g
          stem_water_content{get_input(input_quantities, "stem_water_content")},
          leaf_water_content{get_input(input_quantities, "leaf_water_content")},
          pods_water_content{get_input(input_quantities, "pods_water_content")},

          root_volume_plastic{get_input(input_quantities, "root_volume_plastic")}, // Initial volume of organ, m3
          stem_volume_plastic{get_input(input_quantities, "stem_volume_plastic")},
          leaf_volume_plastic{get_input(input_quantities, "leaf_volume_plastic")},
          pods_volume_plastic{get_input(input_quantities, "pods_volume_plastic")},

          n_resistance{get_input(input_quantities, "n_resistance")}, // dimensionless constant

          R_soil_root_base{get_input(input_quantities, "R_soil_root_base")},
          R_root_stem_base{get_input(input_quantities, "R_root_stem_base")},
          R_stem_leaf_base{get_input(input_quantities, "R_stem_leaf_base")},
          R_stem_pods_base{get_input(input_quantities, "R_stem_pods_base")},

          Root{get_input(input_quantities, "Root")}, // Mg, dry biomass
          Stem{get_input(input_quantities, "Stem")},
          Leaf{get_input(input_quantities, "Leaf")},
          Grain{get_input(input_quantities, "Grain")},

          // Get pointers to output quantities
          R_soil_root_op{get_op(output_quantities, "R_soil_root")}, // g ha-1 hr-1
          R_root_stem_op{get_op(output_quantities, "R_root_stem")},
          R_stem_leaf_op{get_op(output_quantities, "R_stem_leaf")},
          R_stem_pods_op{get_op(output_quantities, "R_stem_pods")},

          RWC_max{get_op(output_quantities, "RWC_max")}, // g ha-1 hr-1
          SWC_max{get_op(output_quantities, "SWC_max")},
          LWC_max{get_op(output_quantities, "LWC_max")},
          PWC_max{get_op(output_quantities, "PWC_max")}
    {
    }

string_vector flow_resistance_calculator::get_inputs()
{
    return {

        "root_water_content",              // g
        "stem_water_content",
        "leaf_water_content",
        "pods_water_content",

        "root_volume_plastic",                     // m3
        "stem_volume_plastic", 
        "leaf_volume_plastic",
        "pods_volume_plastic",

        "n_resistance",

        "R_soil_root_base",
        "R_root_stem_base",
        "R_stem_leaf_base",
        "R_stem_pods_base",

        "Root",         // Mg
        "Stem",
        "Leaf",
        "Grain"

    };
}

string_vector flow_resistance_calculator::get_outputs()
{
    return {

        "R_soil_root",
        "R_root_stem",
        "R_stem_leaf",
        "R_stem_pods",

        "RWC_max",
        "SWC_max",
        "LWC_max",
        "PWC_max"

    };
}

void flow_resistance_calculator::do_operation() const
{

   double pw = 998200.0; // g/m3
   double convf = 1000000.0;

   // Determine maximum organ water content
   // assuming up to 95% of organ can be water.... need to check this
   double RWC_max_new = (95/5)*Root*convf;
   double SWC_max_new = (95/5)*Stem*convf;
   double LWC_max_new = (95/5)*Leaf*convf;
   double PWC_max_new = (95/5)*Grain*convf;

   // Determine minimum organ water content from plastic volume
   double RWC_min = root_volume_plastic*pw;
   double SWC_min = stem_volume_plastic*pw;
   double LWC_min = leaf_volume_plastic*pw;
   double PWC_min = pods_volume_plastic*pw;

   // calculate relative organ water content
//    double root_RWC = (root_water_content - RWC_min)/(RWC_max - RWC_min);
//    double stem_RWC = (stem_water_content - SWC_min)/(SWC_max - SWC_min);
//    double leaf_RWC = (leaf_water_content - LWC_min)/(LWC_max - LWC_min);
//    double pods_RWC = (pods_water_content - PWC_min)/(PWC_max - PWC_min);

   // calculate resistance
   double R_soil_root_new = 0.0;
   double R_root_stem_new = 0.0;
   double R_stem_leaf_new = 0.0;
   double R_stem_pods_new = 0.0;

   // tied to organ size
   if (root_water_content > RWC_min) {
        R_soil_root_new = R_soil_root_base*pow(((root_water_content - RWC_min)/RWC_max_new), n_resistance) + 0.01;
   } else if (root_water_content <= RWC_min) {
        R_soil_root_new = 0.01;
   }

   if (stem_water_content > SWC_min) {
        R_root_stem_new = R_root_stem_base*pow(((stem_water_content - SWC_min)/SWC_max_new), n_resistance) + 0.01;
   } else if (stem_water_content <= SWC_min) {
        R_root_stem_new = 0.01;
   }

   if (leaf_water_content > LWC_min) {
        R_stem_leaf_new = R_stem_leaf_base*pow(((leaf_water_content - LWC_min)/LWC_max_new), n_resistance) + 0.01;
   } else if (leaf_water_content <= LWC_min) {
        R_stem_leaf_new = 0.01;
   }

   if (pods_water_content > PWC_min) {
        R_stem_pods_new = R_stem_pods_base*pow(((pods_water_content - PWC_min)/PWC_max_new), n_resistance) + 0.01;
   } else if (pods_water_content <= PWC_min) {
        R_stem_pods_new = 0.01;
   }
                           
    // Update the output quantity list
    update(R_soil_root_op, R_soil_root_new);
    update(R_root_stem_op, R_root_stem_new);
    update(R_stem_leaf_op, R_stem_leaf_new);
    update(R_stem_pods_op, R_stem_pods_new);
    update(RWC_max, RWC_max_new);
    update(SWC_max, SWC_max_new);
    update(LWC_max, LWC_max_new);
    update(PWC_max, PWC_max_new);

}