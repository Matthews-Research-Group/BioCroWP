#ifndef PRESSURE_POTENTIAL_SENESCENCE_H
#define PRESSURE_POTENTIAL_SENESCENCE_H

#include "../framework/module.h"
#include "../framework/state_map.h"

class pressure_potential_senescence : public differential_module
{
   public:
    pressure_potential_senescence(
        state_map const& input_quantities,
        state_map* output_quantities);

    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "pressure_potential_senescence"; }

   private:
    // References to input quantities
    double const& canopy_transpiration_rate;

    double const& doy;
    double const& hour;

    double const& root_pressure_potential;
    double const& stem_pressure_potential;
    double const& leaf_pressure_potential;
    double const& pods_pressure_potential;

    double const& root_total_potential;
    double const& stem_total_potential;
    double const& leaf_total_potential;
    double const& pods_total_potential;

    double const& root_water_content;
    double const& stem_water_content;
    double const& leaf_water_content;
    double const& pods_water_content;

    double const& root_volume;
    double const& stem_volume;
    double const& leaf_volume;
    double const& pods_volume;

    double const& ext_root_x;
    double const& ext_root_z;

    double const& ext_stem_x;
    double const& ext_stem_z;

    double const& ext_leaf_y;
    double const& ext_leaf_z;

    double const& ext_pods_x;

    double const& mod_root_x;
    double const& mod_root_z;

    double const& mod_stem_x;
    double const& mod_stem_z;

    double const& mod_leaf_x;
    double const& mod_leaf_y;
    double const& mod_leaf_z;

    double const& mod_pods_x;

    double const& wp_crit;

    double const& R_soil_root;
    double const& R_root_stem;
    double const& R_stem_leaf;
    double const& R_stem_pods;

    double const& soil_potential_avg;

    double const& n_plants;
    double const& f_leaf;
    double const& f_pods;

    double const& kSeneRoot;
    double const& kSeneStem;
    double const& kSeneLeaf;

    double const& kRoot;
    double const& kStem;
    double const& kLeaf;
    double const& kGrain;

    double const& Root;
    double const& Stem;
    double const& Leaf;
    double const& Grain;

    // Pointers to output quantities
    double* root_pressure_potential_op;
    double* root_water_content_op;
    double* root_volume_op;

    double* stem_pressure_potential_op;
    double* stem_water_content_op;
    double* stem_volume_op;

    double* leaf_pressure_potential_op;
    double* leaf_water_content_op;
    double* leaf_volume_op;

    double* pods_pressure_potential_op;
    double* pods_water_content_op;
    double* pods_volume_op;

    double* root_volume_plastic_op;
    double* stem_volume_plastic_op;
    double* leaf_volume_plastic_op;
    double* pods_volume_plastic_op;

    // Main operation
    void do_operation() const;
};

#endif