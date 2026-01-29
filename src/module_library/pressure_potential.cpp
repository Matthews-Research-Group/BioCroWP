#include <algorithm>
#include <iostream>

#include "../framework/module.h"                  // for direct_module and update
#include "../framework/module_helper_functions.h" 
#include "pressure_potential.h"

pressure_potential::pressure_potential(
        state_map const& input_quantities,
        state_map* output_quantities)
        : 
          differential_module(),

          // Get references to input quantities
          canopy_transpiration_rate{get_input(input_quantities, "canopy_transpiration_rate")}, // Mg/(ha*hr)
        //   uptake_layer_1{get_input(input_quantities, "uptake_layer_1")}, // Mg/(ha*hr)
        //   uptake_layer_2{get_input(input_quantities, "uptake_layer_2")},
        //   uptake_layer_3{get_input(input_quantities, "uptake_layer_3")},
        //   uptake_layer_4{get_input(input_quantities, "uptake_layer_4")},
        //   uptake_layer_5{get_input(input_quantities, "uptake_layer_5")},
        //   uptake_layer_6{get_input(input_quantities, "uptake_layer_6")},

          root_water_content{get_input(input_quantities, "root_water_content")}, // g
          stem_water_content{get_input(input_quantities, "stem_water_content")},
          leaf_water_content{get_input(input_quantities, "leaf_water_content")},
          pods_water_content{get_input(input_quantities, "pods_water_content")},

          soil_potential_avg{get_input(input_quantities, "soil_potential_avg")},
          root_total_potential{get_input(input_quantities, "root_total_potential")}, // MPa
          stem_total_potential{get_input(input_quantities, "stem_total_potential")},
          leaf_total_potential{get_input(input_quantities, "leaf_total_potential")},
          pods_total_potential{get_input(input_quantities, "pods_total_potential")},

          root_pressure_potential{get_input(input_quantities, "root_pressure_potential")}, // MPa
          stem_pressure_potential{get_input(input_quantities, "stem_pressure_potential")},
          leaf_pressure_potential{get_input(input_quantities, "leaf_pressure_potential")},
          pods_pressure_potential{get_input(input_quantities, "pods_pressure_potential")},

          root_volume{get_input(input_quantities, "root_volume")}, // Initial volume of organ, m3
          stem_volume{get_input(input_quantities, "stem_volume")},
          leaf_volume{get_input(input_quantities, "leaf_volume")},
          pods_volume{get_input(input_quantities, "pods_volume")},

          ext_root_x{get_input(input_quantities, "ext_root_x")},
          ext_root_z{get_input(input_quantities, "ext_root_z")},

          ext_stem_x{get_input(input_quantities, "ext_stem_x")},
          ext_stem_z{get_input(input_quantities, "ext_stem_z")},

          ext_leaf_x{get_input(input_quantities, "ext_leaf_x")},
          ext_leaf_y{get_input(input_quantities, "ext_leaf_y")},
          ext_leaf_z{get_input(input_quantities, "ext_leaf_z")},

          ext_pods_x{get_input(input_quantities, "ext_pods_x")},

          mod_root_x{get_input(input_quantities, "mod_root_x")},
          mod_root_z{get_input(input_quantities, "mod_root_z")},

          mod_stem_x{get_input(input_quantities, "mod_stem_x")},
          mod_stem_z{get_input(input_quantities, "mod_stem_z")},

          mod_leaf_x{get_input(input_quantities, "mod_leaf_x")},
          mod_leaf_y{get_input(input_quantities, "mod_leaf_y")},
          mod_leaf_z{get_input(input_quantities, "mod_leaf_z")},

          mod_pods_x{get_input(input_quantities, "mod_pods_x")},

          wp_crit{get_input(input_quantities, "wp_crit")},

          R_soil_root{get_input(input_quantities, "R_soil_root")},
          R_root_stem{get_input(input_quantities, "R_root_stem")},
          R_stem_leaf{get_input(input_quantities, "R_stem_leaf")},
          R_stem_pods{get_input(input_quantities, "R_stem_pods")},

          // Get pointers to output quantities
          root_pressure_potential_op{get_op(output_quantities, "root_pressure_potential")}, // MPa
          root_water_content_op{get_op(output_quantities, "root_water_content")}, // g
          root_volume_op{get_op(output_quantities, "root_volume")}, // m3

          stem_pressure_potential_op{get_op(output_quantities, "stem_pressure_potential")},
          stem_water_content_op{get_op(output_quantities, "stem_water_content")},
          stem_volume_op{get_op(output_quantities, "stem_volume")},

          leaf_pressure_potential_op{get_op(output_quantities, "leaf_pressure_potential")},
          leaf_water_content_op{get_op(output_quantities, "leaf_water_content")},
          leaf_volume_op{get_op(output_quantities, "leaf_volume")},

          pods_pressure_potential_op{get_op(output_quantities, "pods_pressure_potential")},
          pods_water_content_op{get_op(output_quantities, "pods_water_content")},
          pods_volume_op{get_op(output_quantities, "pods_volume")}
    {
    }

string_vector pressure_potential::get_inputs()
{
    return {
        "canopy_transpiration_rate",          // Mg/(ha*hr)
        // "uptake_layer_1",                     // Mg/(ha*hr)
        // "uptake_layer_2",
        // "uptake_layer_3",
        // "uptake_layer_4",
        // "uptake_layer_5",
        // "uptake_layer_6",

        "soil_potential_avg",
        "root_total_potential",         // MPa
        "stem_total_potential",
        "leaf_total_potential",
        "pods_total_potential",

        "root_pressure_potential",         // MPa
        "stem_pressure_potential",
        "leaf_pressure_potential",
        "pods_pressure_potential",

        "root_water_content",              // g
        "stem_water_content",
        "leaf_water_content",
        "pods_water_content",

        "root_volume",                     // m3
        "stem_volume", 
        "leaf_volume",
        "pods_volume",

        "ext_root_x", // MPa-1 hr-1
        "ext_root_z",

        "ext_stem_x",
        "ext_stem_z",

        "ext_leaf_x",
        "ext_leaf_y",
        "ext_leaf_z",

        "ext_pods_x",

        "mod_root_x", // MPa
        "mod_root_z",

        "mod_stem_x",
        "mod_stem_z",

        "mod_leaf_x",
        "mod_leaf_y",
        "mod_leaf_z",

        "mod_pods_x",

        "wp_crit", // MPa

        "R_soil_root",
        "R_root_stem", // MPa h g-1 ha
        "R_stem_leaf",
        "R_stem_pods",

    };
}

string_vector pressure_potential::get_outputs()
{
    return {
        "root_pressure_potential",  // MPa
        "root_water_content",       // g/ha
        "root_volume",              // m3/ha

        "stem_pressure_potential",  // MPa
        "stem_water_content",       // g/ha
        "stem_volume",              // m3/ha

        "leaf_pressure_potential",  // MPa
        "leaf_water_content",       // g/ha
        "leaf_volume",              // m3/ha

        "pods_pressure_potential",  // MPa
        "pods_water_content",       // g/ha
        "pods_volume"              // m3/ha
    };
}

void pressure_potential::do_operation() const
{
    // convert Mg/(ha*hr) to g/(ha*hr)
    double transpiration = (canopy_transpiration_rate*1000000); // keep in terms of ha

    // Density of water
    double pw = 998200; // g/m3

    // *** replacing root water uptake with uptake via soil water potential because uptake is based on transpiration ***
    double F_rwu = (soil_potential_avg - root_total_potential)/R_soil_root; // g ha-1 hr-1

    // Calculate total RWU in g/(ha*hr)
    // Negative sign because RWU is negative in the soil water model, but needs to be positive in this model
    // double F_rwu = -(uptake_layer_1 + uptake_layer_2 + uptake_layer_3 +
    //      uptake_layer_4 + uptake_layer_5 + uptake_layer_6)*1000000; // g ha-1 hr-1


    // Root water potential update
    // Calculate water flow at this time step using the initial values for pressure potential
    double F_root_stem = (root_total_potential - stem_total_potential)/R_root_stem; // g ha-1 hr-1

    double dW_root;
    if (root_water_content < 0) {
        dW_root = 0.0;
    } else {
        dW_root = F_rwu - F_root_stem;
    }

    if (dW_root < -1*root_water_content) {
        dW_root = -root_water_content;
    }
    // double dW_root_temp = F_rwu - F_root_stem; // g ha-1 hr-1
    // if (dW_root_temp < 0) {
    //     dW_root = 0;
    // } else {
    //     dW_root = dW_root_temp;
    // }

    // Change in pressure potential accounts for both elastic and plastic organ growth
    double potential_value_root = std::max(root_pressure_potential, wp_crit); // MPa
    double root_dPP = ((dW_root/(pw*root_volume) - (ext_root_z + 2*ext_root_x)*(potential_value_root - wp_crit))
                    *((mod_root_x*mod_root_z)/(2*mod_root_z + mod_root_x))); // MPa

    // Change in root volume
    double dV_root = root_volume*(((2*mod_root_z + mod_root_x)/(mod_root_z*mod_root_z))*root_dPP 
                        + (ext_root_z + 2*ext_root_x)*(potential_value_root - wp_crit)); // m3
    // 1 g/cm3 = 1e-9 g/m3
    // dV_root = dW_root/density 


    // Stem water potential update
    double F_stem_leaf = (stem_total_potential - leaf_total_potential)/R_stem_leaf; // g ha hr-1

    double dW_stem;
    if (stem_water_content < 0) {
        dW_stem = 0.0;
    } else {
        dW_stem = F_root_stem - F_stem_leaf;
    }

    if (dW_stem < -1*stem_water_content) {
        dW_stem = -stem_water_content;
    }

    // Change in pressure potential accounts for both elastic and plastic organ growth
    double potential_value_stem = std::max(stem_pressure_potential, wp_crit); // MPa
    double stem_dPP = ((dW_stem/(pw*stem_volume) - (ext_stem_z + 2*ext_stem_x)*(potential_value_stem - wp_crit))
                    *((mod_stem_x*mod_stem_z)/(2*mod_stem_z + mod_stem_x))); // MPa

    // Change in stem volume
    double dV_stem = stem_volume*(((2*mod_stem_z + mod_stem_x)/(mod_stem_z*mod_stem_z))*stem_dPP 
                        + (ext_stem_z + 2*ext_stem_x)*(potential_value_stem - wp_crit)); // m3


    // Leaf Water Potential Update
    double dW_leaf;
    if (leaf_water_content < 0) {
        dW_leaf = 0.0;
    } else {
        dW_leaf = F_stem_leaf - transpiration;
    }

    if (dW_leaf < -1*leaf_water_content) {
        dW_leaf = -leaf_water_content;
    }

    double potential_value_leaf = std::max(leaf_pressure_potential, wp_crit);
    double leaf_dPP = ((dW_leaf/(pw*leaf_volume) - (ext_leaf_z + ext_leaf_x + ext_leaf_y)*(potential_value_leaf - wp_crit))
                    *((mod_leaf_x*mod_leaf_z*mod_leaf_y)/(mod_leaf_x*mod_leaf_z + mod_leaf_z*mod_leaf_y + mod_leaf_x*mod_leaf_y)));

    // Change in leaf volume
    double dV_leaf = leaf_volume*(((mod_leaf_x*mod_leaf_z + mod_leaf_z*mod_leaf_y + mod_leaf_x*mod_leaf_y)/(mod_leaf_x*mod_leaf_z*mod_leaf_y))
                                        *leaf_dPP + (ext_leaf_z + ext_leaf_x + ext_leaf_y)*(potential_value_leaf - wp_crit)); // m3

    
    // Pod Water Potential Update
    double F_stem_pods = (stem_total_potential - pods_total_potential)/R_stem_pods;

    double dW_pods;
    if (pods_water_content < 0) {
        dW_pods = 0.0;
    } else {
        dW_pods = F_stem_pods;
    }

    if (dW_pods < -1*pods_water_content) {
        dW_pods = -pods_water_content;
    }
    
    double potential_value_pods = std::max(pods_pressure_potential, wp_crit);
    double pods_dPP = ((dW_pods/(pw*pods_volume) - (3*ext_pods_x)*(potential_value_pods - wp_crit))
                    *(mod_pods_x/3));

    // Change in pods volume
    double dV_pods = dW_pods/1000000; // m3, don't have an equation developed for this


                                        
    // Update the output quantity list
    update(root_pressure_potential_op, root_dPP);
    update(stem_pressure_potential_op, stem_dPP);
    update(leaf_pressure_potential_op, leaf_dPP);
    update(pods_pressure_potential_op, pods_dPP);
    update(root_water_content_op, dW_root);
    update(stem_water_content_op, dW_stem);
    update(leaf_water_content_op, dW_leaf);
    update(pods_water_content_op, dW_pods);
    update(root_volume_op, dV_root);
    update(stem_volume_op, dV_stem);
    update(leaf_volume_op, dV_leaf);
    update(pods_volume_op, dV_pods);
}