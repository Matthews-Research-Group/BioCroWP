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

          doy{get_input(input_quantities, "doy")},
          hour{get_input(input_quantities, "hour")},

          root_water_content{get_input(input_quantities, "root_water_content")}, // g
          stem_water_content{get_input(input_quantities, "stem_water_content")},
          leaf_water_content{get_input(input_quantities, "leaf_water_content")},
          pods_water_content{get_input(input_quantities, "pods_water_content")},

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

          F_rwu{get_input(input_quantities, "F_rwu")},
          F_root_stem{get_input(input_quantities, "F_root_stem")},
          F_stem_leaf{get_input(input_quantities, "F_stem_leaf")},
          F_stem_pods{get_input(input_quantities, "F_stem_pods")},

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

        "doy",
        "hour",

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

        "F_rwu",
        "F_root_stem",
        "F_stem_leaf",
        "F_stem_pods",

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

    // Root water potential update
    double dW_root = F_rwu - F_root_stem; // g ha-1 hr-1
    if (dW_root < -1*root_water_content) {
        dW_root = -root_water_content;
    }

    // Change in pressure potential accounts for both elastic and plastic organ growth
    double potential_value_root = std::max(root_pressure_potential, wp_crit); // MPa
    double root_dPP = ((dW_root/(pw*root_volume) - (ext_root_z + 2*ext_root_x)*(potential_value_root - wp_crit))
                    *((mod_root_x*mod_root_z)/(2*mod_root_z + mod_root_x))); // MPa

    static int step_count = 0;
    step_count++;

    // Printing solver steps
    // if (step_count % 100 == 0) {
    //     std::cout << "Current simulation day: " << doy 
    //               << " | Current simulation hour: " << hour << std::endl;
    // }

    // Change in root volume
    double dV_root = root_volume*(((2*mod_root_z + mod_root_x)/(mod_root_z*mod_root_z))*root_dPP 
                        + (ext_root_z + 2*ext_root_x)*(potential_value_root - wp_crit)); // m3 


    // Stem water potential update
    double dW_stem = F_root_stem - F_stem_leaf - F_stem_pods;
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
    if (transpiration < 0) {
        transpiration = 0; // transpiration should never be negative, setting to 0 if this is the case
    }
    double dW_leaf = F_stem_leaf - transpiration;
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
    double dW_pods = F_stem_pods;
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