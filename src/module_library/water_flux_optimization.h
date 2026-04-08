#ifndef WATER_FLUX_OPTIMIZATION_H
#define WATER_FLUX_OPTIMIZATION_H

/**
 * structure to hold output of water flux calculations
 */
struct water_flux_outputs {
    double F_rwu;  // g ha-1 hr-1
    double F_root_stem; 
    double F_stem_leaf;  
    double F_stem_pods;  
};

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

    double kGrain);

#endif