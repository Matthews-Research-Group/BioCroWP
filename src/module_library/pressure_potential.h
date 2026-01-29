#ifndef PRESSURE_POTENTIAL_H
#define PRESSURE_POTENTIAL_H

#include "../framework/module.h"
#include "../framework/state_map.h"

class pressure_potential : public differential_module
{
   public:
    pressure_potential(
        state_map const& input_quantities,
        state_map* output_quantities);

    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "pressure_potential"; }

   private:
    // References to input quantities
    double const& canopy_transpiration_rate;
    // double const& uptake_layer_1;
    // double const& uptake_layer_2;
    // double const& uptake_layer_3;
    // double const& uptake_layer_4;
    // double const& uptake_layer_5;
    // double const& uptake_layer_6;

    double const& soil_potential_avg;
    double const& root_total_potential;
    double const& stem_total_potential;
    double const& leaf_total_potential;
    double const& pods_total_potential;

    double const& root_pressure_potential;
    double const& stem_pressure_potential;
    double const& leaf_pressure_potential;
    double const& pods_pressure_potential;

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

    double const& ext_leaf_x;
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

    // Main operation
    void do_operation() const;
};

#endif