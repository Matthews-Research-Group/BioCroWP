#ifndef PRESSURE_POTENTIAL_OPTIMIZATION_H
#define PRESSURE_POTENTIAL_OPTIMIZATION_H

/**
 * structure to hold output of W, V, and pressure potential calculations
 */
struct pressure_potential_outputs {
    double d_root_pressure_potential;
    double d_stem_pressure_potential;
    double d_leaf_pressure_potential;
    double d_pods_pressure_potential;
    // double root_total_potential;
    // double stem_total_potential;
    // double leaf_total_potential;
    // double pods_total_potential;
    double d_root_water_content;
    double d_stem_water_content;
    double d_leaf_water_content;
    double d_pods_water_content;
    double d_root_volume;
    double d_stem_volume;
    double d_leaf_volume;
    double d_pods_volume;
    double d_root_volume_plastic;
    double d_stem_volume_plastic;
    double d_leaf_volume_plastic;
    double d_pods_volume_plastic;
};

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
    );

#endif