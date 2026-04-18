#include "transport_balance.h"
#include <cmath>
#include <algorithm>
#include <iostream>

using namespace BioCroWP;

string_vector transport_balance::get_inputs()
{
    return {
        "canopy_transpiration_rate", "doy", "hour",
        "root_pressure_potential", "stem_pressure_potential", "leaf_pressure_potential", "pods_pressure_potential",
        "root_osmotic_potential", "stem_osmotic_potential", "leaf_osmotic_potential", "pods_osmotic_potential",
        "root_total_potential", "stem_total_potential", "leaf_total_potential", "pods_total_potential",
        "root_water_content", "stem_water_content", "leaf_water_content", "pods_water_content",
        "root_volume", "stem_volume", "leaf_volume", "pods_volume",
        "root_volume_plastic", "stem_volume_plastic", "leaf_volume_plastic", "pods_volume_plastic",
        "ext_root_x", "ext_root_z", "ext_stem_x", "ext_stem_z", "ext_leaf_y", "ext_leaf_z", "ext_pods_x",
        "mod_root_x", "mod_root_z", "mod_stem_x", "mod_stem_z", "mod_leaf_x", "mod_leaf_y", "mod_leaf_z", "mod_pods_x",
        "wp_crit",
        "kSeneRoot", "kSeneStem", "kSeneLeaf",
        "kRoot", "kStem", "kLeaf", "kGrain",
        "Root", "Stem", "Leaf", "Grain",
        "R_soil_root", "R_root_stem", "R_stem_leaf", "R_stem_pods",
        "soil_potential_avg"
    };
}

string_vector transport_balance::get_outputs()
{
    return {
        "root_pressure_potential", "stem_pressure_potential", "leaf_pressure_potential", "pods_pressure_potential",
        "root_total_potential", "stem_total_potential", "leaf_total_potential", "pods_total_potential",
        "root_water_content", "stem_water_content", "leaf_water_content", "pods_water_content",
        "root_volume", "stem_volume", "leaf_volume", "pods_volume",
        "root_volume_plastic", "stem_volume_plastic", "leaf_volume_plastic", "pods_volume_plastic"
    };
}

void transport_balance::do_operation() const
{
    // Initialize structs
    pressure_potential_outputs potential_calc;
    water_flux_outputs flux_calc;

    // Start by using inputs from previous timestep
    double pressure_root_guess = root_pressure_potential;
    double pressure_stem_guess = stem_pressure_potential;
    double pressure_leaf_guess = leaf_pressure_potential;
    double pressure_pods_guess = pods_pressure_potential;

    int i = 0;
    int i_max = 1000;
    double const tol = 0.01; // g ha-1 hr-1
    // double alpha = 0.3; // Damping factor to prevent infinite bouncing

    // Loop to calculate water flux, W, pressure potential, and V 
    while (i < i_max) {
        
        // Calculate water flux based on guessed pressure potentials
        flux_calc = water_flux_optimization(
            pressure_root_guess, pressure_stem_guess, pressure_leaf_guess, pressure_pods_guess,
            root_osmotic_potential, stem_osmotic_potential, leaf_osmotic_potential, pods_osmotic_potential,
            R_soil_root, R_root_stem, R_stem_leaf, R_stem_pods,
            soil_potential_avg, kGrain
        );

        // Recalculate W, pressure potential, and V based on those fluxes
        potential_calc = pressure_potential_optimization(
            canopy_transpiration_rate, doy, hour, root_pressure_potential, stem_pressure_potential, leaf_pressure_potential, pods_pressure_potential,
            root_osmotic_potential, stem_osmotic_potential, leaf_osmotic_potential, pods_osmotic_potential,
            root_total_potential, stem_total_potential, leaf_total_potential, pods_total_potential,
            root_water_content, stem_water_content, leaf_water_content, pods_water_content, 
            root_volume, stem_volume, leaf_volume, pods_volume, 
            root_volume_plastic, stem_volume_plastic, leaf_volume_plastic, pods_volume_plastic, 
            ext_root_x, ext_root_z, ext_stem_x, ext_stem_z, ext_leaf_y, ext_leaf_z, ext_pods_x, 
            mod_root_x, mod_root_z, mod_stem_x, mod_stem_z, mod_leaf_x, mod_leaf_y, mod_leaf_z, mod_pods_x, 
            wp_crit,
            flux_calc.F_rwu, flux_calc.F_root_stem, flux_calc.F_stem_leaf, flux_calc.F_stem_pods,
            kSeneRoot, kSeneStem, kSeneLeaf, kRoot, kStem,
            kLeaf, kGrain, Root, Stem, Leaf, Grain
        );

        // // Transpiration in flux units (g ha-1 hr-1)
        // double transpiration = canopy_transpiration_rate * 1000000.0;

        // // Senescence outflows (g ha-1 hr-1)
        // double sene_root = kSeneRoot * root_water_content;
        // double sene_stem = kSeneStem * stem_water_content;
        // double sene_leaf = kSeneLeaf * leaf_water_content;

        // Check mass conservation: net flux into organ minus change in water content
        // double root_res = std::abs((flux_calc.F_rwu - flux_calc.F_root_stem - sene_root) - dW_root);
        // double stem_res = std::abs((flux_calc.F_root_stem - flux_calc.F_stem_leaf - flux_calc.F_stem_pods - sene_stem) - dW_stem);
        // double leaf_res = std::abs((flux_calc.F_stem_leaf - transpiration - sene_leaf) - dW_leaf);
        // double pods_res = std::abs(flux_calc.F_stem_pods - dW_pods);

        // Break the loop if mass conservation is satisfied for all organs
        // if (root_res < tol && stem_res < tol && leaf_res < tol && pods_res < tol) {
        //     break;
        // }

        // Update pressure potential guesses
        double root_calc = root_pressure_potential + potential_calc.d_root_pressure_potential;
        double stem_calc = stem_pressure_potential + potential_calc.d_stem_pressure_potential;
        double leaf_calc = leaf_pressure_potential + potential_calc.d_leaf_pressure_potential;
        double pods_calc = pods_pressure_potential + potential_calc.d_pods_pressure_potential;

        double root_res = std::abs(root_calc - pressure_root_guess);
        double stem_res = std::abs(stem_calc - pressure_stem_guess);
        double leaf_res = std::abs(leaf_calc - pressure_leaf_guess);
        double pods_res = std::abs(pods_calc - pressure_pods_guess);

        // Break if all PP residuals are below tolerance
        if (root_res < tol && stem_res < tol && leaf_res < tol && pods_res < tol) {
            break;
        }

        if (i == i_max) {
        // Warning if the model fails to converge within timestep
            std::cout << "Warning: transport_balance reached max iterations without converging.\n";
            std::cout << "DOY: " << doy << " Hour: " << hour << " | Final residuals - Root: " << std::abs(pressure_root_guess - root_pressure_potential) 
                    << ", Stem: " << std::abs(pressure_stem_guess - stem_pressure_potential) 
                    << ", Leaf: " << std::abs(pressure_leaf_guess - leaf_pressure_potential) 
                    << ", Pods: " << std::abs(pressure_pods_guess - pods_pressure_potential) << std::endl;
        }

        pressure_root_guess = root_calc;
        pressure_stem_guess = stem_calc;
        pressure_leaf_guess = leaf_calc;
        pressure_pods_guess = pods_calc;

        ++i;
    };

    std::cout << "DOY: " << doy << " Hour: " << hour << " | Iterations to converge: " << i << std::endl;

    // Update outputs
    
    update(root_pressure_potential_op, potential_calc.d_root_pressure_potential);
    update(stem_pressure_potential_op, potential_calc.d_stem_pressure_potential);
    update(leaf_pressure_potential_op, potential_calc.d_leaf_pressure_potential);
    update(pods_pressure_potential_op, potential_calc.d_pods_pressure_potential);

    update(root_total_potential_op, potential_calc.d_root_total_potential);
    update(stem_total_potential_op, potential_calc.d_stem_total_potential);
    update(leaf_total_potential_op, potential_calc.d_leaf_total_potential);
    update(pods_total_potential_op, potential_calc.d_pods_total_potential);
    
    update(root_water_content_op, potential_calc.d_root_water_content);
    update(stem_water_content_op, potential_calc.d_stem_water_content);
    update(leaf_water_content_op, potential_calc.d_leaf_water_content);
    update(pods_water_content_op, potential_calc.d_pods_water_content);
    
    update(root_volume_op, potential_calc.d_root_volume);
    update(stem_volume_op, potential_calc.d_stem_volume);
    update(leaf_volume_op, potential_calc.d_leaf_volume);
    update(pods_volume_op, potential_calc.d_pods_volume);
    
    update(root_volume_plastic_op, potential_calc.d_root_volume_plastic);
    update(stem_volume_plastic_op, potential_calc.d_stem_volume_plastic);
    update(leaf_volume_plastic_op, potential_calc.d_leaf_volume_plastic);
    update(pods_volume_plastic_op, potential_calc.d_pods_volume_plastic);
}
