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
    double pw = 998200.0; // g/m3

    // Convert canopy transpiration from Mg/(ha*hr) to g/(ha*hr)
    double transpiration_ha = canopy_transpiration_rate * 1000000.0; // g ha-1 hr-1
    if (transpiration_ha < 0) {
        transpiration_ha = 0;
    }

    // Per-plant transpiration partitioned between leaf and pods
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

    // Senescence rates
    double sene_lwc = kSeneLeaf * W_leaf;
    double sene_swc = kSeneStem * W_stem;
    double sene_rwc = kSeneRoot * W_root;

    // Original potential values
    double potential_value_leaf = std::max(leaf_pressure_potential, wp_crit);
    double potential_value_pods = std::max(pods_pressure_potential, wp_crit);
    double potential_value_stem = std::max(stem_pressure_potential, wp_crit);
    double potential_value_root = std::max(root_pressure_potential, wp_crit);

    // Iteration parameters
    int max_iter = 1000;
    double tol = 0.1; // g plant-1 hr-1

    // Pre-compute elastic modulus combinations
    double leaf_mod_denom = mod_leaf_x * mod_leaf_z + mod_leaf_z * mod_leaf_y + mod_leaf_x * mod_leaf_y;
    double leaf_mod_factor = (mod_leaf_x * mod_leaf_z * mod_leaf_y) / leaf_mod_denom;
    double leaf_compliance = leaf_mod_denom / (mod_leaf_x * mod_leaf_z * mod_leaf_y);

    double pods_mod_factor = mod_pods_x / 3.0;

    double stem_mod_factor = (mod_stem_x * mod_stem_z) / (2.0 * mod_stem_z + mod_stem_x);
    double stem_compliance = (2.0 * mod_stem_z + mod_stem_x) / (mod_stem_z * mod_stem_z);

    double root_mod_factor = (mod_root_x * mod_root_z) / (2.0 * mod_root_z + mod_root_x);
    double root_compliance = (2.0 * mod_root_z + mod_root_x) / (mod_root_z * mod_root_z);

    // Activity flags
    bool leaf_active = (kLeaf >= 0.005) || (Leaf >= 0.1);
    bool leaf_growing = (kLeaf >= 0.005);
    bool pods_active = (kGrain >= 0.005) || (Grain >= 0.01);
    bool stem_active = (kStem >= 0.005) || (Stem >= 0.01);
    bool stem_growing = (kStem >= 0.005);
    bool root_active = (kRoot >= 0.005) || (Root >= 0.01);
    bool root_growing = (kRoot >= 0.005);

    double dW_leaf = 0.0, leaf_dPP = 0.0, dV_leaf_elastic = 0.0, dV_leaf_plastic = 0.0;
    double dW_pods = 0.0, pods_dPP = 0.0, dV_pods_elastic = 0.0, dV_pods_plastic = 0.0;
    double dW_stem = 0.0, stem_dPP = 0.0, dV_stem_elastic = 0.0, dV_stem_plastic = 0.0;
    double dW_root = 0.0, root_dPP = 0.0, dV_root_elastic = 0.0, dV_root_plastic = 0.0;
    double F_stem_leaf = 0.0, F_stem_pods = 0.0, F_root_stem = 0.0, F_rwu = 0.0;

    // Converge LEAF given upstream stem total potential (stem_tp)
    //   1) dW_leaf, dPP_leaf from outflow only (transpiration + senescence)
    //   2) F_stem_leaf from lowered leaf potential
    //   3) Iterate: recalculate dPP_leaf with inflow until F_stem_leaf converges

    auto converge_leaf = [&](double stem_tp) {
        if (!leaf_active) {
            dW_leaf = 0.0; leaf_dPP = 0.0;
            dV_leaf_elastic = 0.0; dV_leaf_plastic = 0.0;
            F_stem_leaf = 0.0;
            return;
        }

        // Step 1: Outflow only (transpiration + senescence)
        dW_leaf = -sene_lwc - T_leaf;
        if (dW_leaf < -W_leaf) { dW_leaf = -W_leaf; }

        if (leaf_growing) {
            leaf_dPP = (dW_leaf / (pw * V_leaf)
                        - (ext_leaf_z + ext_leaf_y) * (potential_value_leaf - wp_crit))
                       * leaf_mod_factor;
        } else {
            leaf_dPP = (dW_leaf / (pw * V_leaf)) * leaf_mod_factor;
        }

        // Step 2: Calculate inflow based on lowered leaf potential
        double leaf_tp_est = leaf_total_potential + leaf_dPP;
        F_stem_leaf = (stem_tp - leaf_tp_est) / R_stem_leaf;

        // Step 3: Iterate until F_stem_leaf converges
        for (int i = 0; i < max_iter; ++i) {
            dW_leaf = F_stem_leaf - sene_lwc - T_leaf;
            if (dW_leaf < -W_leaf) { dW_leaf = -W_leaf; }

            double pv_ext = std::max(leaf_tp_est, wp_crit);
            if (leaf_growing) {
                leaf_dPP = (dW_leaf / (pw * V_leaf)
                            - (ext_leaf_z + ext_leaf_y) * (pv_ext - wp_crit))
                           * leaf_mod_factor;
                dV_leaf_elastic = V_leaf * leaf_compliance * leaf_dPP;
                dV_leaf_plastic = (ext_leaf_z + ext_leaf_y) * (pv_ext - wp_crit);
            } else {
                leaf_dPP = (dW_leaf / (pw * V_leaf)) * leaf_mod_factor;
                dV_leaf_elastic = V_leaf * leaf_compliance * leaf_dPP;
                dV_leaf_plastic = 0.0;
            }

            leaf_tp_est = leaf_total_potential + leaf_dPP;
            double F_new = (stem_tp - leaf_tp_est) / R_stem_leaf;

            if (std::abs(F_new - F_stem_leaf) < tol) {
                F_stem_leaf = F_new;
                break;
            }
            F_stem_leaf = F_new;
        }
    };

    // Converge PODS given upstream stem total potential (stem_tp)
    //   1) dW_pods, dPP_pods from outflow only (transpiration)
    //   2) F_stem_pods from lowered pods potential
    //   3) Iterate: recalculate dPP_pods with inflow until F_stem_pods converges
    //   Note: F_stem_pods = 0 when kGrain < 0.005 (grain fill not started)

    auto converge_pods = [&](double stem_tp) {
        F_stem_pods = 0.0;

        if (!pods_active) {
            dW_pods = 0.0; pods_dPP = 0.0;
            dV_pods_elastic = 0.0; dV_pods_plastic = 0.0;
            return;
        }

        // Pods exist but grain fill not started: transpiration loss only, no stem inflow
        if (kGrain < 0.005) {
            dW_pods = -T_pods;
            if (dW_pods < -W_pods) { dW_pods = -W_pods; }
            pods_dPP = (dW_pods / (pw * V_pods)
                        - 3.0 * ext_pods_x * (potential_value_pods - wp_crit))
                       * pods_mod_factor;
            dV_pods_elastic = V_pods * mod_pods_x * pods_dPP;
            dV_pods_plastic = ext_pods_x * (potential_value_pods - wp_crit);
            return;
        }

        // kGrain >= 0.005: Full iteration with inflow from stem
        // Step 1: Outflow only (transpiration through pods)
        dW_pods = -T_pods;
        if (dW_pods < -W_pods) { dW_pods = -W_pods; }
        pods_dPP = (dW_pods / (pw * V_pods)
                    - 3.0 * ext_pods_x * (potential_value_pods - wp_crit))
                   * pods_mod_factor;

        // Step 2: Calculate inflow based on lowered pods potential
        double pods_tp_est = pods_total_potential + pods_dPP;
        F_stem_pods = (stem_tp - pods_tp_est) / R_stem_pods;

        // Step 3: Iterate until F_stem_pods converges
        for (int i = 0; i < max_iter; ++i) {
            dW_pods = F_stem_pods - T_pods;
            if (dW_pods < -W_pods) { dW_pods = -W_pods; }

            double pv_ext = std::max(pods_tp_est, wp_crit);
            pods_dPP = (dW_pods / (pw * V_pods)
                        - 3.0 * ext_pods_x * (pv_ext - wp_crit))
                       * pods_mod_factor;
            dV_pods_elastic = V_pods * mod_pods_x * pods_dPP;
            dV_pods_plastic = ext_pods_x * (pv_ext - wp_crit);

            pods_tp_est = pods_total_potential + pods_dPP;
            double F_new = (stem_tp - pods_tp_est) / R_stem_pods;

            if (std::abs(F_new - F_stem_pods) < tol) {
                F_stem_pods = F_new;
                break;
            }
            F_stem_pods = F_new;
        }
    };

    // Level 2: Converge STEM + all organs above (leaf, pods)
    //   Given upstream root total potential (root_tp):
    //   1) Converge leaf and pods with current stem potential
    //   2) dW_stem, dPP_stem from outflow only (F_stem_leaf + F_stem_pods + sene)
    //   3) F_root_stem from lowered stem potential
    //   4) Iterate: recalculate dPP_stem with inflow, re-converge leaf & pods
    //      with updated stem potential, until F_root_stem converges

    auto converge_stem = [&](double root_tp) {
        // Initial leaf & pods convergence with current (unchanged) stem potential
        converge_leaf(stem_total_potential);
        converge_pods(stem_total_potential);

        if (!stem_active) {
            dW_stem = 0.0; stem_dPP = 0.0;
            dV_stem_elastic = 0.0; dV_stem_plastic = 0.0;
            F_root_stem = 0.0;
            return;
        }

        // Step 1: Stem outflow only
        dW_stem = -F_stem_leaf - F_stem_pods - sene_swc;
        if (dW_stem < -W_stem) { dW_stem = -W_stem; }

        if (stem_growing) {
            stem_dPP = (dW_stem / (pw * V_stem)
                        - (ext_stem_z + 2.0 * ext_stem_x) * (potential_value_stem - wp_crit))
                       * stem_mod_factor;
        } else {
            stem_dPP = (dW_stem / (pw * V_stem)) * stem_mod_factor;
        }

        // Step 2: Calculate inflow from root based on lowered stem potential
        double stem_tp_est = stem_total_potential + stem_dPP;
        F_root_stem = (root_tp - stem_tp_est) / R_root_stem;

        // Step 3: Iterate until F_root_stem converges
        for (int i = 0; i < max_iter; ++i) {
            dW_stem = F_root_stem - F_stem_leaf - F_stem_pods - sene_swc;
            if (dW_stem < -W_stem) { dW_stem = -W_stem; }

            double pv_ext = std::max(stem_tp_est, wp_crit);
            if (stem_growing) {
                stem_dPP = (dW_stem / (pw * V_stem)
                            - (ext_stem_z + 2.0 * ext_stem_x) * (pv_ext - wp_crit))
                           * stem_mod_factor;
                dV_stem_elastic = V_stem * stem_compliance * stem_dPP;
                dV_stem_plastic = (ext_stem_z + 2.0 * ext_stem_x) * (pv_ext - wp_crit);
            } else {
                stem_dPP = (dW_stem / (pw * V_stem)) * stem_mod_factor;
                dV_stem_elastic = V_stem * stem_compliance * stem_dPP;
                dV_stem_plastic = 0.0;
            }

            stem_tp_est = stem_total_potential + stem_dPP;

            // Re-converge leaf and pods with updated stem potential
            converge_leaf(stem_tp_est);
            converge_pods(stem_tp_est);

            double F_new = (root_tp - stem_tp_est) / R_root_stem;

            if (std::abs(F_new - F_root_stem) < tol) {
                F_root_stem = F_new;
                break;
            }
            F_root_stem = F_new;
        }
    };

    // Level 3: ROOT CONVERGENCE + all organs above (stem, leaf, pods)
    //   1) Converge stem (which converges leaf, pods) with current root potential
    //   2) dW_root, dPP_root from outflow only (F_root_stem + senescence)
    //   3) F_rwu from lowered root potential
    //   4) Iterate: recalculate dPP_root with inflow, re-converge stem/leaf/pods
    //      with updated root potential, until F_rwu converges

    // Initial stem convergence (leaf/pods converge inside) with original root potential
    converge_stem(root_total_potential);

    if (root_active) {
        // Step 1: Root outflow only
        dW_root = -F_root_stem - sene_rwc;
        if (dW_root < -W_root) { dW_root = -W_root; }

        if (root_growing) {
            root_dPP = (dW_root / (pw * V_root)
                        - (ext_root_z + 2.0 * ext_root_x) * (potential_value_root - wp_crit))
                       * root_mod_factor;
        } else {
            root_dPP = (dW_root / (pw * V_root)) * root_mod_factor;
        }

        // Step 2: Calculate inflow from soil based on lowered root potential
        double root_tp_est = root_total_potential + root_dPP;
        F_rwu = (soil_potential_avg - root_tp_est) / R_soil_root;

        // Step 3: Iterate until F_rwu converges
        for (int i = 0; i < max_iter; ++i) {
            dW_root = F_rwu - F_root_stem - sene_rwc;
            if (dW_root < -W_root) { dW_root = -W_root; }

            double pv_ext = std::max(root_tp_est, wp_crit);
            if (root_growing) {
                root_dPP = (dW_root / (pw * V_root)
                            - (ext_root_z + 2.0 * ext_root_x) * (pv_ext - wp_crit))
                           * root_mod_factor;
                dV_root_elastic = V_root * root_compliance * root_dPP;
                dV_root_plastic = (ext_root_z + 2.0 * ext_root_x) * (pv_ext - wp_crit);
            } else {
                root_dPP = (dW_root / (pw * V_root)) * root_mod_factor;
                dV_root_elastic = V_root * root_compliance * root_dPP;
                dV_root_plastic = 0.0;
            }

            root_tp_est = root_total_potential + root_dPP;

            // Re-converge stem (which re-converges leaf/pods) with updated root potential
            converge_stem(root_tp_est);

            double F_new = (soil_potential_avg - root_tp_est) / R_soil_root;

            if (std::abs(F_new - F_rwu) < tol) {
                F_rwu = F_new;
                break;
            }
            F_rwu = F_new;
        }
    }

    // Compute final total volume changes
    double dV_leaf = dV_leaf_elastic + dV_leaf_plastic;
    double dV_pods = dV_pods_elastic + dV_pods_plastic;
    double dV_stem = dV_stem_elastic + dV_stem_plastic;
    double dV_root = dV_root_elastic + dV_root_plastic;

    update(root_pressure_potential_op, root_dPP);
    update(stem_pressure_potential_op, stem_dPP);
    update(leaf_pressure_potential_op, leaf_dPP);
    update(pods_pressure_potential_op, pods_dPP);

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
