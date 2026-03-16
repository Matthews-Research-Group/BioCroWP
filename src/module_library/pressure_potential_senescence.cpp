#include <algorithm>
#include <iostream>

#include "../framework/module.h"                  // for direct_module and update
#include "../framework/module_helper_functions.h" 
#include "pressure_potential_senescence.h"

pressure_potential_senescence::pressure_potential_senescence(
        state_map const& input_quantities,
        state_map* output_quantities)
        : 
          differential_module(),

          // Get references to input quantities
          canopy_transpiration_rate{get_input(input_quantities, "canopy_transpiration_rate")}, // Mg/(ha*hr)

          doy{get_input(input_quantities, "doy")},
          hour{get_input(input_quantities, "hour")},

          root_pressure_potential{get_input(input_quantities, "root_pressure_potential")}, // MPa
          stem_pressure_potential{get_input(input_quantities, "stem_pressure_potential")},
          leaf_pressure_potential{get_input(input_quantities, "leaf_pressure_potential")},
          pods_pressure_potential{get_input(input_quantities, "pods_pressure_potential")},

          root_water_content{get_input(input_quantities, "root_water_content")}, // g
          stem_water_content{get_input(input_quantities, "stem_water_content")},
          leaf_water_content{get_input(input_quantities, "leaf_water_content")},
          pods_water_content{get_input(input_quantities, "pods_water_content")},

          root_volume{get_input(input_quantities, "root_volume")}, // Initial volume of organ, m3
          stem_volume{get_input(input_quantities, "stem_volume")},
          leaf_volume{get_input(input_quantities, "leaf_volume")},
          pods_volume{get_input(input_quantities, "pods_volume")},

          ext_root_x{get_input(input_quantities, "ext_root_x")},
          ext_root_z{get_input(input_quantities, "ext_root_z")},

          ext_stem_x{get_input(input_quantities, "ext_stem_x")},
          ext_stem_z{get_input(input_quantities, "ext_stem_z")},

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

          kSeneRoot{get_input(input_quantities, "kSeneRoot")}, // dimensionless
          kSeneStem{get_input(input_quantities, "kSeneStem")}, // fraction of organ senesced
          kSeneLeaf{get_input(input_quantities, "kSeneLeaf")},

          kRoot{get_input(input_quantities, "kRoot")}, // fraction of carbon allocated to each organ
          kStem{get_input(input_quantities, "kStem")}, // using to modulate organ development start time
          kLeaf{get_input(input_quantities, "kLeaf")},
          kGrain{get_input(input_quantities, "kGrain")},

          Root{get_input(input_quantities, "Root")}, // Organ dry biomass (Mg/ha)
          Stem{get_input(input_quantities, "Stem")}, // using to modulate organ development start time
          Leaf{get_input(input_quantities, "Leaf")},
          Grain{get_input(input_quantities, "Grain")},

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

string_vector pressure_potential_senescence::get_inputs()
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

        "kSeneRoot", // dimensionless
        "kSeneStem",
        "kSeneLeaf",

        "kRoot",
        "kStem",
        "kLeaf",
        "kGrain",

        "Root",
        "Stem",
        "Leaf",
        "Grain",

    };
}

string_vector pressure_potential_senescence::get_outputs()
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

void pressure_potential_senescence::do_operation() const
{

    static int step_count = 0;
    step_count++;

    // Printing solver steps
    // if (step_count % 100 == 0) {
    //     std::cout << "Current simulation day: " << doy 
    //               << " | Current simulation hour: " << hour << std::endl;
    // }

    // convert Mg/(ha*hr) to g/(ha*hr)
    double transpiration = (canopy_transpiration_rate*1000000); // keep in terms of ha

    // Density of water
    double pw = 998200; // g/m3

    // Root water potential update

    // Case 1: If for some reason roots are not growing at season start, then no change occurs
    double dW_root = 0.0;
    double root_dPP = 0.0;
    double dV_root = 0.0;

    // These calculations apply to cases 2 and 3
    double dW_root_pre = F_rwu - F_root_stem; // g ha-1 hr-1
    double sene_rwc = kSeneRoot*dW_root_pre; // fraction of root wet biomass senesced
    double potential_value_root = std::max(root_pressure_potential, wp_crit); // MPa

    if (kRoot >= 0.005){ // Case 2: organ growth has begun and carbon allocation is sufficient for plastic growth

        dW_root = dW_root_pre - sene_rwc;
        root_dPP = ((dW_root/(pw*root_volume) - (ext_root_z + 2*ext_root_x)*(potential_value_root - wp_crit))
                    *((mod_root_x*mod_root_z)/(2*mod_root_z + mod_root_x))); // MPa
        dV_root = root_volume*(((2*mod_root_z + mod_root_x)/(mod_root_z*mod_root_z))*root_dPP 
                            + (ext_root_z + 2*ext_root_x)*(potential_value_root - wp_crit)); // m3

    } else if (kRoot < 0.005 && Root >= 0.01) { // Case 3: Carbon allocation is not sufficient for plastic growth

        dW_root = dW_root_pre - sene_rwc;
        root_dPP = ((dW_root/(pw*root_volume))
                    *((mod_root_x*mod_root_z)/(2*mod_root_z + mod_root_x))); // MPa
        dV_root = root_volume*(((2*mod_root_z + mod_root_x)/(mod_root_z*mod_root_z))*root_dPP); // m3 

    }


    // Stem water potential update

    // Case 1: Stem growth has not started yet
    double dW_stem = 0.0;
    double stem_dPP = 0.0;
    double dV_stem = 0.0;

    double dW_stem_pre = F_root_stem - F_stem_leaf - F_stem_pods;
    double sene_swc = kSeneStem*dW_stem_pre;
    double potential_value_stem = std::max(stem_pressure_potential, wp_crit);

    if (kStem >= 0.005) { // Case 2: Carbon allocation is sufficient for plastic growth
        
        dW_stem = dW_stem_pre - sene_swc; 
        stem_dPP = ((dW_stem/(pw*stem_volume) - (ext_stem_z + 2*ext_stem_x)*(potential_value_stem - wp_crit))
                        *((mod_stem_x*mod_stem_z)/(2*mod_stem_z + mod_stem_x))); // MPa
        dV_stem = stem_volume*(((2*mod_stem_z + mod_stem_x)/(mod_stem_z*mod_stem_z))*stem_dPP 
                            + (ext_stem_z + 2*ext_stem_x)*(potential_value_stem - wp_crit)); // m3

    } else if (kStem < 0.005 && Stem >= 0.01){ // Case 3: Carbon allocation is not sufficient for plastic growth
        
        dW_stem = dW_stem_pre - sene_swc; 
        stem_dPP = ((dW_stem/(pw*stem_volume))
                    *((mod_stem_x*mod_stem_z)/(2*mod_stem_z + mod_stem_x))); // MPa
        dV_stem = stem_volume*(((2*mod_stem_z + mod_stem_x)/(mod_stem_z*mod_stem_z))*stem_dPP); // m3 

    }
    


    // Leaf Water Potential Update

    //Case 1: Leaf growth has not started yet
    double dW_leaf = 0.0;
    double leaf_dPP = 0.0;
    double dV_leaf = 0.0;

    if (transpiration < 0) {
        transpiration = 0; // transpiration should never be negative, setting to 0 if this is the case
    }

    double dW_leaf_pre = F_stem_leaf - transpiration;
    double sene_lwc = kSeneLeaf*leaf_water_content;
    double potential_value_leaf = std::max(leaf_pressure_potential, wp_crit);

    if (kLeaf >= 0.005) { // Case 2: Carbon allocation is sufficient for plastic growth
        
        dW_leaf = dW_leaf_pre - sene_lwc;
        leaf_dPP = ((dW_leaf/(pw*leaf_volume) - (ext_leaf_z + ext_leaf_y)*(potential_value_leaf - wp_crit))
                        *((mod_leaf_x*mod_leaf_z*mod_leaf_y)/(mod_leaf_x*mod_leaf_z + mod_leaf_z*mod_leaf_y + mod_leaf_x*mod_leaf_y)));
        dV_leaf = leaf_volume*(((mod_leaf_x*mod_leaf_z + mod_leaf_z*mod_leaf_y + mod_leaf_x*mod_leaf_y)/(mod_leaf_x*mod_leaf_z*mod_leaf_y))
                                            *leaf_dPP + (ext_leaf_z + ext_leaf_y)*(potential_value_leaf - wp_crit)); // m3

    } else if (kLeaf < 0.005 && Leaf >= 0.1) { // Case 3: Carbon allocation is not sufficient for plastic growth

        dW_leaf = dW_leaf_pre - sene_lwc;
        leaf_dPP = ((dW_leaf/(pw*leaf_volume))
                    *((mod_leaf_x*mod_leaf_z*mod_leaf_y)/(mod_leaf_x*mod_leaf_z + mod_leaf_z*mod_leaf_y + mod_leaf_x*mod_leaf_y)));
        dV_leaf = leaf_volume*(((mod_leaf_x*mod_leaf_z + mod_leaf_z*mod_leaf_y + mod_leaf_x*mod_leaf_y)/(mod_leaf_x*mod_leaf_z*mod_leaf_y))
                                            *leaf_dPP); // m3

    }
    
    // Pod Water Potential Update

    // Case 1: Pod growth has not started yet
    double dW_pods = 0.0;
    double pods_dPP = 0.0;
    double dV_pods = 0.0;

    // There is no pod senescence
    double potential_value_pods = std::max(pods_pressure_potential, wp_crit);

    if (kGrain >= 0.005) { // Case 2: Carbon allocation is sufficient for plastic growth

        dW_pods = F_stem_pods;
        pods_dPP = ((dW_pods/(pw*pods_volume) - (3*ext_pods_x)*(potential_value_pods - wp_crit))
                        *(mod_pods_x/3)); 
        dV_pods = (pods_volume*mod_pods_x*pods_dPP) + (ext_pods_x*(potential_value_pods - pods_pressure_potential)); // m3, don't have an equation developed for this      

    } else if (kGrain < 0.005 && Grain >= 0.01) { // Case 3: Carbon allocation is not sufficient for plastic growth
        
        dW_pods = F_stem_pods;
        pods_dPP = ((dW_pods/(pw*pods_volume) - (3*ext_pods_x)*(potential_value_pods - wp_crit))
                        *(mod_pods_x/3)); 
        dV_pods = (pods_volume*mod_pods_x*pods_dPP); // m3, don't have an equation developed for this        

    }

                              
    // Update the output quantity list
    update(root_pressure_potential_op, root_dPP); // MPa
    update(stem_pressure_potential_op, stem_dPP);
    update(leaf_pressure_potential_op, leaf_dPP);
    update(pods_pressure_potential_op, pods_dPP);
    update(root_water_content_op, dW_root); // g ha-1 hr-1
    update(stem_water_content_op, dW_stem);
    update(leaf_water_content_op, dW_leaf);
    update(pods_water_content_op, dW_pods);
    update(root_volume_op, dV_root); // m3
    update(stem_volume_op, dV_stem);
    update(leaf_volume_op, dV_leaf);
    update(pods_volume_op, dV_pods);
}