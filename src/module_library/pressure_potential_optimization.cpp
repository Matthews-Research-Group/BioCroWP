#include <algorithm>
#include <iostream>
#include <cmath>

#include "../framework/module.h"                  // for direct_module and update
#include "../framework/module_helper_functions.h" 
#include "pressure_potential_optimization.h"


// Smooth approximation of max(a, b) with continuous derivatives.
// k controls the smoothing width (in same units as a, b).
static double smooth_max(double a, double b, double k = 0.01) {
    double diff = a - b;
    return 0.5 * (a + b + std::sqrt(diff * diff + k * k));
}

pressure_potential_outputs pressure_potential_optimization(
    double canopy_transpiration_rate,
    double doy,
    double hour,
    double root_pressure_potential,
    double stem_pressure_potential,
    double leaf_pressure_potential,
    double pods_pressure_potential,
    double root_osmotic_potential,
    double stem_osmotic_potential,
    double leaf_osmotic_potential,
    double pods_osmotic_potential,
    double root_total_potential,
    double stem_total_potential,
    double leaf_total_potential,
    double pods_total_potential,
    double root_water_content,
    double stem_water_content,
    double leaf_water_content,
    double pods_water_content,
    double root_volume,
    double stem_volume,
    double leaf_volume,
    double pods_volume,
    double root_volume_plastic,
    double stem_volume_plastic,
    double leaf_volume_plastic,
    double pods_volume_plastic,
    double ext_root_x,
    double ext_root_z,
    double ext_stem_x,
    double ext_stem_z,
    double ext_leaf_y,
    double ext_leaf_z,
    double ext_pods_x,
    double mod_root_x,
    double mod_root_z,
    double mod_stem_x,
    double mod_stem_z,
    double mod_leaf_x,
    double mod_leaf_y,
    double mod_leaf_z,
    double mod_pods_x,
    double wp_crit,
    double F_rwu,
    double F_root_stem,
    double F_stem_leaf,
    double F_stem_pods,
    double kSeneRoot,
    double kSeneStem,
    double kSeneLeaf,
    double kRoot,
    double kStem,
    double kLeaf,
    double kGrain,
    double Root,
    double Stem,
    double Leaf,
    double Grain
)
{

    // Initialize
    pressure_potential_outputs result;

    // Begin W, V, and pressure potential calculation for each organ

    // Partion between pod and leaf
    double transpiration = (canopy_transpiration_rate*1000000); // keep in terms of ha

    // Density of water
    double pw = 998200; // g/m3

    // Root water potential update

    // Case 1: If for some reason roots are not growing at season start, then no change occurs
    double dW_root = 0.0;
    double root_dPP = 0.0;
    double dV_root_elastic = 0.0;
    double dV_root_plastic = 0.0;

    // These calculations apply to cases 2 and 3
    double sene_rwc = kSeneRoot*root_water_content; // fraction of root wet biomass senesced
    double potential_value_root = smooth_max(root_pressure_potential, wp_crit); // MPa

    if (kRoot >= 0.005){ // Case 2: organ growth has begun and carbon allocation is sufficient for plastic growth

        dW_root = F_rwu - F_root_stem - sene_rwc; // g ha-1 hr-1
        if (dW_root < -1*root_water_content) {
            dW_root = -root_water_content;
        }
  
        root_dPP = ((dW_root/(pw*root_volume) - (ext_root_z + 2*ext_root_x)*(potential_value_root - wp_crit))
                    *((mod_root_x*mod_root_z)/(2*mod_root_z + mod_root_x))); // MPa
        dV_root_plastic = (ext_root_z + 2*ext_root_x)*(potential_value_root - wp_crit);
        dV_root_elastic = root_volume*((2*mod_root_z + mod_root_x)/(mod_root_z*mod_root_z))*root_dPP; // m3

    } else if (kRoot < 0.005 && Root >= 0.01) { // Case 3: Carbon allocation is not sufficient for plastic growth

        dW_root = F_rwu - F_root_stem - sene_rwc; // g ha-1 hr-1
        if (dW_root < -1*root_water_content) {
            dW_root = -root_water_content;
        }

        root_dPP = ((dW_root/(pw*root_volume))
                    *((mod_root_x*mod_root_z)/(2*mod_root_z + mod_root_x))); // MPa
        dV_root_elastic = root_volume*(((2*mod_root_z + mod_root_x)/(mod_root_z*mod_root_z))*root_dPP); // m3 
        dV_root_plastic = 0.0;

    }

    double dV_root = dV_root_elastic + dV_root_plastic;


    // Stem water potential update

    // Case 1: Stem growth has not started yet
    double dW_stem = 0.0;
    double stem_dPP = 0.0;
    double dV_stem_plastic = 0.0;
    double dV_stem_elastic = 0.0;

    double sene_swc = kSeneStem*stem_water_content;
    double potential_value_stem = smooth_max(stem_pressure_potential, wp_crit);

    if (kStem >= 0.005) { // Case 2: Carbon allocation is sufficient for plastic growth
        
        dW_stem = F_root_stem - F_stem_leaf - F_stem_pods - sene_swc;
        if (dW_stem < -1*stem_water_content) {
            dW_stem = -stem_water_content;
        }

        stem_dPP = ((dW_stem/(pw*stem_volume) - (ext_stem_z + 2*ext_stem_x)*(potential_value_stem - wp_crit))
                        *((mod_stem_x*mod_stem_z)/(2*mod_stem_z + mod_stem_x))); // MPa
        dV_stem_elastic = stem_volume*((2*mod_stem_z + mod_stem_x)/(mod_stem_z*mod_stem_z))*stem_dPP;
        dV_stem_plastic = (ext_stem_z + 2*ext_stem_x)*(potential_value_stem - wp_crit);

    } else if (kStem < 0.005 && Stem >= 0.01){ // Case 3: Carbon allocation is not sufficient for plastic growth
        dW_stem = F_root_stem - F_stem_leaf - F_stem_pods - sene_swc;

        if (dW_stem < -1*stem_water_content) {
            dW_stem = -stem_water_content;
        }
        stem_dPP = ((dW_stem/(pw*stem_volume))
                    *((mod_stem_x*mod_stem_z)/(2*mod_stem_z + mod_stem_x))); // MPa
        dV_stem_elastic = stem_volume*((2*mod_stem_z + mod_stem_x)/(mod_stem_z*mod_stem_z))*stem_dPP;
        dV_stem_plastic = 0.0;

    }

    double dV_stem = dV_stem_plastic + dV_stem_elastic;
    


    // Leaf Water Potential Update

    //Case 1: Leaf growth has not started yet
    double dW_leaf = 0.0;
    double leaf_dPP = 0.0;
    double dV_leaf_plastic = 0.0;
    double dV_leaf_elastic = 0.0;

    if (transpiration < 0) {
        transpiration = 0; // transpiration should never be negative, setting to 0 if this is the case
    }

    double sene_lwc = kSeneLeaf*leaf_water_content;
    double potential_value_leaf = smooth_max(leaf_pressure_potential, wp_crit);

    if (kLeaf >= 0.005) { // Case 2: Carbon allocation is sufficient for plastic growth
        
        dW_leaf = F_stem_leaf - sene_lwc - transpiration;
        if (dW_leaf < -1*leaf_water_content) {
            dW_leaf = -leaf_water_content;
        }
        leaf_dPP = ((dW_leaf/(pw*leaf_volume) - (ext_leaf_z + ext_leaf_y)*(potential_value_leaf - wp_crit))
                        *((mod_leaf_x*mod_leaf_z*mod_leaf_y)/(mod_leaf_x*mod_leaf_z + mod_leaf_z*mod_leaf_y + mod_leaf_x*mod_leaf_y)));
        dV_leaf_elastic = leaf_volume*((mod_leaf_x*mod_leaf_z + mod_leaf_z*mod_leaf_y + mod_leaf_x*mod_leaf_y)/(mod_leaf_x*mod_leaf_z*mod_leaf_y))*leaf_dPP;
        dV_leaf_plastic = (ext_leaf_z + ext_leaf_y)*(potential_value_leaf - wp_crit);

    } else if (kLeaf < 0.005 && Leaf >= 0.1) { // Case 3: Carbon allocation is not sufficient for plastic growth

        dW_leaf = F_stem_leaf - sene_lwc - transpiration;
        if (dW_leaf < -1*leaf_water_content) {
            dW_leaf = -leaf_water_content;
        }
        leaf_dPP = ((dW_leaf/(pw*leaf_volume))
                    *((mod_leaf_x*mod_leaf_z*mod_leaf_y)/(mod_leaf_x*mod_leaf_z + mod_leaf_z*mod_leaf_y + mod_leaf_x*mod_leaf_y)));
        dV_leaf_elastic = leaf_volume*((mod_leaf_x*mod_leaf_z + mod_leaf_z*mod_leaf_y + mod_leaf_x*mod_leaf_y)/(mod_leaf_x*mod_leaf_z*mod_leaf_y))*leaf_dPP;
        dV_leaf_plastic = 0.0;

    }

    double dV_leaf = dV_leaf_plastic + dV_leaf_elastic;
    
    // Pod Water Potential Update
    // Pod transpiration fraction (constant) scaled by the amount of pod

    // Case 1: Pod growth has not started yet
    double dW_pods = 0.0;
    double pods_dPP = 0.0;
    double dV_pods_elastic = 0.0;
    double dV_pods_plastic = 0.0;

    // There is no pod senescence
    double potential_value_pods = smooth_max(pods_pressure_potential, wp_crit);

    if (kGrain >= 0.005) { // Case 2: Carbon allocation is sufficient for plastic growth

        dW_pods = F_stem_pods;
        if (dW_pods < -1*pods_water_content) {
            dW_pods = -pods_water_content;
        }
        pods_dPP = ((dW_pods/(pw*pods_volume))*(mod_pods_x/3)); 
        dV_pods_elastic = pods_volume*mod_pods_x*pods_dPP;
        dV_pods_plastic = ext_pods_x*(potential_value_pods - wp_crit);

    } else if (kGrain < 0.005 && Grain >= 0.01) { // Case 3: Carbon allocation is not sufficient for plastic growth
        
        dW_pods = F_stem_pods;
        if (dW_pods < -1*pods_water_content) {
            dW_pods = -pods_water_content;
        }
        pods_dPP = ((dW_pods/(pw*pods_volume) - (3*ext_pods_x)*(potential_value_pods - wp_crit))
                        *(mod_pods_x/3)); 
        dV_pods_elastic = pods_volume*mod_pods_x*pods_dPP;
        dV_pods_plastic = 0.0;        

    }

    double dV_pods = dV_pods_elastic + dV_pods_plastic;

    // Update values
    double root_pressure_potential_new = root_pressure_potential + root_dPP;
    double stem_pressure_potential_new = stem_pressure_potential + stem_dPP;
    double leaf_pressure_potential_new = leaf_pressure_potential + leaf_dPP;
    double pods_pressure_potential_new = pods_pressure_potential + pods_dPP;

    // Calculate new total potential based on calculated pressure potential value
    double root_total_potential_new = root_pressure_potential_new + root_osmotic_potential;
    double stem_total_potential_new = stem_pressure_potential_new + stem_osmotic_potential;
    double leaf_total_potential_new = leaf_pressure_potential_new + leaf_osmotic_potential;
    double pods_total_potential_new = pods_pressure_potential_new + pods_osmotic_potential;

    double root_dTP = root_total_potential_new - root_total_potential;
    double stem_dTP = stem_total_potential_new - stem_total_potential;
    double leaf_dTP = leaf_total_potential_new - leaf_total_potential;
    double pods_dTP = pods_total_potential_new - pods_total_potential;

    // Store results
    result.d_root_pressure_potential = root_dPP;
    result.d_stem_pressure_potential = stem_dPP;
    result.d_leaf_pressure_potential = leaf_dPP;
    result.d_pods_pressure_potential = pods_dPP;
    result.d_root_total_potential = root_dTP;
    result.d_stem_total_potential = stem_dTP;
    result.d_leaf_total_potential = leaf_dTP;
    result.d_pods_total_potential = pods_dTP;
    result.d_root_water_content = dW_root;
    result.d_stem_water_content = dW_stem;
    result.d_leaf_water_content = dW_leaf;
    result.d_pods_water_content = dW_pods;
    result.d_root_volume = dV_root;
    result.d_stem_volume = dV_stem;
    result.d_leaf_volume = dV_leaf;
    result.d_pods_volume = dV_pods;
    result.d_root_volume_plastic = dV_root_plastic;
    result.d_stem_volume_plastic = dV_stem_plastic;
    result.d_leaf_volume_plastic = dV_leaf_plastic;
    result.d_pods_volume_plastic = dV_pods_plastic;
  
    return result;
}