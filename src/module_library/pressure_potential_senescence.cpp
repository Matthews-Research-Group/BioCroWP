#include <algorithm>
#include <cmath>
#include <iostream>

#include "../framework/module.h"
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

          root_total_potential{get_input(input_quantities, "root_total_potential")}, // MPa
          stem_total_potential{get_input(input_quantities, "stem_total_potential")},
          leaf_total_potential{get_input(input_quantities, "leaf_total_potential")},
          pods_total_potential{get_input(input_quantities, "pods_total_potential")},

          root_water_content{get_input(input_quantities, "root_water_content")}, // g/ha
          stem_water_content{get_input(input_quantities, "stem_water_content")},
          leaf_water_content{get_input(input_quantities, "leaf_water_content")},
          pods_water_content{get_input(input_quantities, "pods_water_content")},

          root_volume{get_input(input_quantities, "root_volume")}, // m3/ha
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

          R_soil_root{get_input(input_quantities, "R_soil_root")},
          R_root_stem{get_input(input_quantities, "R_root_stem")},
          R_stem_leaf{get_input(input_quantities, "R_stem_leaf")},
          R_stem_pods{get_input(input_quantities, "R_stem_pods")},

          soil_potential_avg{get_input(input_quantities, "soil_potential_avg")},

          n_plants{get_input(input_quantities, "n_plants")},
          f_leaf{get_input(input_quantities, "f_leaf")},
          f_pods{get_input(input_quantities, "f_pods")},

          kSeneRoot{get_input(input_quantities, "kSeneRoot")}, // dimensionless
          kSeneStem{get_input(input_quantities, "kSeneStem")}, // fraction of organ senesced
          kSeneLeaf{get_input(input_quantities, "kSeneLeaf")},

          kRoot{get_input(input_quantities, "kRoot")}, // fraction of carbon allocated to each organ
          kStem{get_input(input_quantities, "kStem")}, // using to modulate organ development start time
          kLeaf{get_input(input_quantities, "kLeaf")},
          kGrain{get_input(input_quantities, "kGrain")},

          Root{get_input(input_quantities, "Root")}, // Organ dry biomass (Mg/ha)
          Stem{get_input(input_quantities, "Stem")},
          Leaf{get_input(input_quantities, "Leaf")},
          Grain{get_input(input_quantities, "Grain")},

          // Get pointers to output quantities
          root_pressure_potential_op{get_op(output_quantities, "root_pressure_potential")}, // MPa
          root_water_content_op{get_op(output_quantities, "root_water_content")}, // g/ha
          root_volume_op{get_op(output_quantities, "root_volume")}, // m3/ha

          stem_pressure_potential_op{get_op(output_quantities, "stem_pressure_potential")},
          stem_water_content_op{get_op(output_quantities, "stem_water_content")},
          stem_volume_op{get_op(output_quantities, "stem_volume")},

          leaf_pressure_potential_op{get_op(output_quantities, "leaf_pressure_potential")},
          leaf_water_content_op{get_op(output_quantities, "leaf_water_content")},
          leaf_volume_op{get_op(output_quantities, "leaf_volume")},

          pods_pressure_potential_op{get_op(output_quantities, "pods_pressure_potential")},
          pods_water_content_op{get_op(output_quantities, "pods_water_content")},
          pods_volume_op{get_op(output_quantities, "pods_volume")},

          root_volume_plastic_op{get_op(output_quantities, "root_volume_plastic")},
          stem_volume_plastic_op{get_op(output_quantities, "stem_volume_plastic")},
          leaf_volume_plastic_op{get_op(output_quantities, "leaf_volume_plastic")},
          pods_volume_plastic_op{get_op(output_quantities, "pods_volume_plastic")}
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

        "root_total_potential",            // MPa (from total_potential direct module)
        "stem_total_potential",
        "leaf_total_potential",
        "pods_total_potential",

        "root_water_content",              // g/ha
        "stem_water_content",
        "leaf_water_content",
        "pods_water_content",

        "root_volume",                     // m3/ha
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

        "R_soil_root",  // MPa hr g-1 (per-plant resistance)
        "R_root_stem",
        "R_stem_leaf",
        "R_stem_pods",

        "soil_potential_avg", // MPa

        "n_plants",  // plants per hectare
        "f_leaf",    // fraction of transpiration through leaves
        "f_pods",    // fraction of transpiration through pods

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
        "pods_volume",               // m3/ha

        "root_volume_plastic",      // m3/ha
        "stem_volume_plastic",
        "leaf_volume_plastic",
        "pods_volume_plastic"


    };
}

void pressure_potential_senescence::do_operation() const
{

    static int step_count = 0;
    step_count++;

    // Printing solver steps
    if (step_count % 100 == 0) {
        std::cout << "Current simulation day: " << doy 
                  << " | Current simulation hour: " << hour << std::endl;
    }

    // Density of water
    double pw = 998200; // g/m3

    // Convert canopy transpiration from Mg/(ha*hr) to g/(ha*hr), then to per-plant
    double transpiration_ha = canopy_transpiration_rate * 1000000.0; // g ha-1 hr-1
    if (transpiration_ha < 0) {
        transpiration_ha = 0; // transpiration should never be negative
    }

    // Per-plant transpiration partitioned between leaf and pods (Sinha et al. 2023)
    double T_leaf = (f_leaf * transpiration_ha) / n_plants;  // g plant-1 hr-1
    double T_pods = (f_pods * transpiration_ha) / n_plants;  // g plant-1 hr-1

    // Convert hectare-scale state variables to per-plant
    double W_root = root_water_content / n_plants;  // g plant-1
    double W_stem = stem_water_content / n_plants;
    double W_leaf = leaf_water_content / n_plants;
    double W_pods = pods_water_content / n_plants;

    double V_root = root_volume / n_plants;  // m3 plant-1
    double V_stem = stem_volume / n_plants;
    double V_leaf = leaf_volume / n_plants;
    double V_pods = pods_volume / n_plants;

    // Compute per-plant fluxes via Ohm's law
    double F_rwu        = (soil_potential_avg - root_total_potential) / R_soil_root;    // g plant-1 hr-1
    double F_root_stem  = (root_total_potential - stem_total_potential) / R_root_stem;
    double F_stem_leaf  = (stem_total_potential - leaf_total_potential) / R_stem_leaf;
    double F_stem_pods  = (stem_total_potential - pods_total_potential) / R_stem_pods;

    // Guard: no flow to pods before pod growth begins
    if (kGrain < 0.005) {
        F_stem_pods = 0.0;
    }

    // ========================================================================
    // Root water potential update
    // ========================================================================

    // Case 1: roots not growing yet — no change
    double dW_root = 0.0;
    double root_dPP = 0.0;
    double dV_root_elastic = 0.0;
    double dV_root_plastic = 0.0;

    double sene_rwc = kSeneRoot * W_root; // per-plant senescence outflow
    double potential_value_root = std::max(root_pressure_potential, wp_crit);

    if (kRoot >= 0.005) { // Case 2: plastic growth active

        dW_root = F_rwu - F_root_stem - sene_rwc; // g plant-1 hr-1
        if (stem_water_content == 0.0) {
            if (dW_root < -1 * W_root) {
                dW_root = -W_root;
            }
        }
        root_dPP = ((dW_root / (pw * V_root) - (ext_root_z + 2 * ext_root_x) * (potential_value_root - wp_crit))
                    * ((mod_root_x * mod_root_z) / (2 * mod_root_z + mod_root_x))); // MPa hr-1
        dV_root_plastic = (ext_root_z + 2 * ext_root_x) * (potential_value_root - wp_crit); // m3 plant-1 hr-1
        dV_root_elastic = V_root * ((2 * mod_root_z + mod_root_x) / (mod_root_z * mod_root_z)) * root_dPP;

    } else if (kRoot < 0.005 && Root >= 0.01) { // Case 3: elastic only

        dW_root = F_rwu - F_root_stem - sene_rwc;
        if (stem_water_content == 0.0) {
            if (dW_root < -1 * W_root) {
                dW_root = -W_root;
            }
        }
        root_dPP = ((dW_root / (pw * V_root))
                    * ((mod_root_x * mod_root_z) / (2 * mod_root_z + mod_root_x)));
        dV_root_elastic = V_root * (((2 * mod_root_z + mod_root_x) / (mod_root_z * mod_root_z)) * root_dPP);
        dV_root_plastic = 0.0;
    }

    double dV_root = dV_root_elastic + dV_root_plastic;

    // ========================================================================
    // Stem water potential update
    // ========================================================================

    // Case 1: stem not growing yet
    double dW_stem = 0.0;
    double stem_dPP = 0.0;
    double dV_stem_plastic = 0.0;
    double dV_stem_elastic = 0.0;

    double sene_swc = kSeneStem * W_stem;
    double potential_value_stem = std::max(stem_pressure_potential, wp_crit);

    if (kStem >= 0.005) { // Case 2: plastic growth active

        dW_stem = F_root_stem - F_stem_leaf - F_stem_pods - sene_swc;
        if (leaf_water_content == 0.0) {
            if (dW_stem < -1 * W_stem) {
                dW_stem = -W_stem;
            }
        }
        stem_dPP = ((dW_stem / (pw * V_stem) - (ext_stem_z + 2 * ext_stem_x) * (potential_value_stem - wp_crit))
                        * ((mod_stem_x * mod_stem_z) / (2 * mod_stem_z + mod_stem_x)));
        dV_stem_elastic = V_stem * ((2 * mod_stem_z + mod_stem_x) / (mod_stem_z * mod_stem_z)) * stem_dPP;
        dV_stem_plastic = (ext_stem_z + 2 * ext_stem_x) * (potential_value_stem - wp_crit);

    } else if (kStem < 0.005 && Stem >= 0.01) { // Case 3: elastic only

        dW_stem = F_root_stem - F_stem_leaf - F_stem_pods - sene_swc;
        if (leaf_water_content == 0.0) {
            if (dW_stem < -1 * W_stem) {
                dW_stem = -W_stem;
            }
        }
        stem_dPP = ((dW_stem / (pw * V_stem))
                    * ((mod_stem_x * mod_stem_z) / (2 * mod_stem_z + mod_stem_x)));
        dV_stem_elastic = V_stem * ((2 * mod_stem_z + mod_stem_x) / (mod_stem_z * mod_stem_z)) * stem_dPP;
        dV_stem_plastic = 0.0;
    }

    double dV_stem = dV_stem_plastic + dV_stem_elastic;

    // ========================================================================
    // Leaf water potential update
    // ========================================================================

    // Case 1: leaf not growing yet
    double dW_leaf = 0.0;
    double leaf_dPP = 0.0;
    double dV_leaf_plastic = 0.0;
    double dV_leaf_elastic = 0.0;

    double sene_lwc = kSeneLeaf * W_leaf;
    double potential_value_leaf = std::max(leaf_pressure_potential, wp_crit);

    if (kLeaf >= 0.005) { // Case 2: plastic growth active

        dW_leaf = F_stem_leaf - sene_lwc - T_leaf;
        if (dW_leaf < -1 * W_leaf) {
            dW_leaf = -W_leaf;
        }
        leaf_dPP = ((dW_leaf / (pw * V_leaf) - (ext_leaf_z + ext_leaf_y) * (potential_value_leaf - wp_crit))
                        * ((mod_leaf_x * mod_leaf_z * mod_leaf_y) / (mod_leaf_x * mod_leaf_z + mod_leaf_z * mod_leaf_y + mod_leaf_x * mod_leaf_y)));
        dV_leaf_elastic = V_leaf * ((mod_leaf_x * mod_leaf_z + mod_leaf_z * mod_leaf_y + mod_leaf_x * mod_leaf_y) / (mod_leaf_x * mod_leaf_z * mod_leaf_y)) * leaf_dPP;
        dV_leaf_plastic = (ext_leaf_z + ext_leaf_y) * (potential_value_leaf - wp_crit);

    } else if (kLeaf < 0.005 && Leaf >= 0.1) { // Case 3: elastic only

        dW_leaf = F_stem_leaf - sene_lwc - T_leaf;
        if (dW_leaf < -1 * W_leaf) {
            dW_leaf = -W_leaf;
        }
        leaf_dPP = ((dW_leaf / (pw * V_leaf))
                    * ((mod_leaf_x * mod_leaf_z * mod_leaf_y) / (mod_leaf_x * mod_leaf_z + mod_leaf_z * mod_leaf_y + mod_leaf_x * mod_leaf_y)));
        dV_leaf_elastic = V_leaf * ((mod_leaf_x * mod_leaf_z + mod_leaf_z * mod_leaf_y + mod_leaf_x * mod_leaf_y) / (mod_leaf_x * mod_leaf_z * mod_leaf_y)) * leaf_dPP;
        dV_leaf_plastic = 0.0;
    }

    double dV_leaf = dV_leaf_plastic + dV_leaf_elastic;

    // ========================================================================
    // Pod water potential update
    // ========================================================================

    // Case 1: pod growth not started yet
    double dW_pods = 0.0;
    double pods_dPP = 0.0;
    double dV_pods_elastic = 0.0;
    double dV_pods_plastic = 0.0;

    double potential_value_pods = std::max(pods_pressure_potential, wp_crit);

    if (kGrain >= 0.005) { // Case 2: plastic growth active

        dW_pods = F_stem_pods - T_pods;
        pods_dPP = ((dW_pods / (pw * V_pods) - (3 * ext_pods_x) * (potential_value_pods - wp_crit))
                        * (mod_pods_x / 3));
        dV_pods_elastic = V_pods * mod_pods_x * pods_dPP;
        dV_pods_plastic = ext_pods_x * (potential_value_pods - wp_crit);

    } else if (kGrain < 0.005 && Grain >= 0.01) { // Case 3: elastic only

        dW_pods = F_stem_pods - T_pods;
        pods_dPP = ((dW_pods / (pw * V_pods) - (3 * ext_pods_x) * (potential_value_pods - wp_crit))
                        * (mod_pods_x / 3));
        dV_pods_elastic = V_pods * mod_pods_x * pods_dPP;
        dV_pods_plastic = 0.0;
    }

    double dV_pods = dV_pods_elastic + dV_pods_plastic;

    // ========================================================================
    // Update outputs
    // ========================================================================
    // PP is intensive (MPa hr-1) — no scaling needed
    update(root_pressure_potential_op, root_dPP);
    update(stem_pressure_potential_op, stem_dPP);
    update(leaf_pressure_potential_op, leaf_dPP);
    update(pods_pressure_potential_op, pods_dPP);

    // W and V are extensive — scale from per-plant back to per-hectare
    update(root_water_content_op, dW_root * n_plants);  // g ha-1 hr-1
    update(stem_water_content_op, dW_stem * n_plants);
    update(leaf_water_content_op, dW_leaf * n_plants);
    update(pods_water_content_op, dW_pods * n_plants);

    update(root_volume_op, dV_root * n_plants);  // m3 ha-1 hr-1
    update(stem_volume_op, dV_stem * n_plants);
    update(leaf_volume_op, dV_leaf * n_plants);
    update(pods_volume_op, dV_pods * n_plants);

    update(root_volume_plastic_op, dV_root_plastic * n_plants);
    update(stem_volume_plastic_op, dV_stem_plastic * n_plants);
    update(leaf_volume_plastic_op, dV_leaf_plastic * n_plants);
    update(pods_volume_plastic_op, dV_pods_plastic * n_plants);
}