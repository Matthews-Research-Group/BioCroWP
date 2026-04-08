#include "water_flux_optimization.h"

water_flux_outputs water_flux_optimization(
    double root_pressure_potential,
    double stem_pressure_potential,
    double leaf_pressure_potential,
    double pods_pressure_potential,
    double root_osmotic_potential,
    double stem_osmotic_potential,
    double leaf_osmotic_potential,
    double pods_osmotic_potential,

    double R_soil_root,
    double R_root_stem,
    double R_stem_leaf,
    double R_stem_pods,

    double soil_potential_avg,

    double kGrain

)
{

    // Initialize
    water_flux_outputs result;

    // calculate total potential based on guessed pressure potential value
    double root_total_potential = root_pressure_potential + root_osmotic_potential;
    double stem_total_potential = stem_pressure_potential + stem_osmotic_potential;
    double leaf_total_potential = leaf_pressure_potential + leaf_osmotic_potential;
    double pods_total_potential = pods_pressure_potential + pods_osmotic_potential;

    // Planting density factor
    // (150,000 plants/acre)*(1 acre/ 4046.86 m2)*(10,000 m2/ha) = 370658 plants/ha
    // rounding to the nearest plant
    double n_plants = 370658.0;

    // Calculate fluxes
    double F_rwu_new = n_plants*((soil_potential_avg - root_total_potential)/R_soil_root); // g ha-1 hr-1
    double F_root_stem_new = n_plants*((root_total_potential - stem_total_potential)/R_root_stem);
    double F_stem_leaf_new = n_plants*((stem_total_potential - leaf_total_potential)/R_stem_leaf);
    double F_stem_pods_new = n_plants*((stem_total_potential - pods_total_potential)/R_stem_pods);

    // if pod growth hasn't started F_stem_pods should be 0
    if (kGrain < 0.005) {
        F_stem_pods_new = 0.0;
    }

    // Store results
    result.F_rwu = F_rwu_new;
    result.F_root_stem = F_root_stem_new;
    result.F_stem_leaf = F_stem_leaf_new;
    result.F_stem_pods = F_stem_pods_new;
  
    return result;
}