#ifndef BIOCROWP_TRANSPORT_BALANCE_H
#define BIOCROWP_TRANSPORT_BALANCE_H

#include "../framework/module.h"
#include "../framework/state_map.h"

#include "water_flux_optimization.h"
#include "pressure_potential_optimization.h"

namespace BioCroWP
{
class transport_balance : public differential_module
{
   public:
    transport_balance(
        state_map const& input_quantities,
        state_map* output_quantities)
        : differential_module(),

          // Get references to input quantities
          canopy_transpiration_rate{get_input(input_quantities, "canopy_transpiration_rate")},
          doy{get_input(input_quantities, "doy")},
          hour{get_input(input_quantities, "hour")},
          root_pressure_potential{get_input(input_quantities, "root_pressure_potential")},
          stem_pressure_potential{get_input(input_quantities, "stem_pressure_potential")},
          leaf_pressure_potential{get_input(input_quantities, "leaf_pressure_potential")},
          pods_pressure_potential{get_input(input_quantities, "pods_pressure_potential")},
          root_osmotic_potential{get_input(input_quantities, "root_osmotic_potential")},
          stem_osmotic_potential{get_input(input_quantities, "stem_osmotic_potential")},
          leaf_osmotic_potential{get_input(input_quantities, "leaf_osmotic_potential")},
          pods_osmotic_potential{get_input(input_quantities, "pods_osmotic_potential")},
          root_water_content{get_input(input_quantities, "root_water_content")},
          stem_water_content{get_input(input_quantities, "stem_water_content")},
          leaf_water_content{get_input(input_quantities, "leaf_water_content")},
          pods_water_content{get_input(input_quantities, "pods_water_content")},
          root_volume{get_input(input_quantities, "root_volume")},
          stem_volume{get_input(input_quantities, "stem_volume")},
          leaf_volume{get_input(input_quantities, "leaf_volume")},
          pods_volume{get_input(input_quantities, "pods_volume")},
          root_volume_plastic{get_input(input_quantities, "root_volume_plastic")},
          stem_volume_plastic{get_input(input_quantities, "stem_volume_plastic")},
          leaf_volume_plastic{get_input(input_quantities, "leaf_volume_plastic")},
          pods_volume_plastic{get_input(input_quantities, "pods_volume_plastic")},
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
          kSeneRoot{get_input(input_quantities, "kSeneRoot")},
          kSeneStem{get_input(input_quantities, "kSeneStem")},
          kSeneLeaf{get_input(input_quantities, "kSeneLeaf")},
          kRoot{get_input(input_quantities, "kRoot")},
          kStem{get_input(input_quantities, "kStem")},
          kLeaf{get_input(input_quantities, "kLeaf")},
          kGrain{get_input(input_quantities, "kGrain")},
          Root{get_input(input_quantities, "Root")},
          Stem{get_input(input_quantities, "Stem")},
          Leaf{get_input(input_quantities, "Leaf")},
          Grain{get_input(input_quantities, "Grain")},
          R_soil_root{get_input(input_quantities, "R_soil_root")},
          R_root_stem{get_input(input_quantities, "R_root_stem")},
          R_stem_leaf{get_input(input_quantities, "R_stem_leaf")},
          R_stem_pods{get_input(input_quantities, "R_stem_pods")},
          soil_potential_avg{get_input(input_quantities, "soil_potential_avg")},

          // Get pointers to output quantities
          root_pressure_potential_op{get_op(output_quantities, "root_pressure_potential")},
          stem_pressure_potential_op{get_op(output_quantities, "stem_pressure_potential")},
          leaf_pressure_potential_op{get_op(output_quantities, "leaf_pressure_potential")},
          pods_pressure_potential_op{get_op(output_quantities, "pods_pressure_potential")},
          root_water_content_op{get_op(output_quantities, "root_water_content")},
          stem_water_content_op{get_op(output_quantities, "stem_water_content")},
          leaf_water_content_op{get_op(output_quantities, "leaf_water_content")},
          pods_water_content_op{get_op(output_quantities, "pods_water_content")},
          root_volume_op{get_op(output_quantities, "root_volume")},
          stem_volume_op{get_op(output_quantities, "stem_volume")},
          leaf_volume_op{get_op(output_quantities, "leaf_volume")},
          pods_volume_op{get_op(output_quantities, "pods_volume")},
          root_volume_plastic_op{get_op(output_quantities, "root_volume_plastic")},
          stem_volume_plastic_op{get_op(output_quantities, "stem_volume_plastic")},
          leaf_volume_plastic_op{get_op(output_quantities, "leaf_volume_plastic")},
          pods_volume_plastic_op{get_op(output_quantities, "pods_volume_plastic")}
    {
    }

    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "transport_balance"; }

   private:
    // References to input quantities
    double const& canopy_transpiration_rate;
    double const& doy;
    double const& hour;
    double const& root_pressure_potential;
    double const& stem_pressure_potential;
    double const& leaf_pressure_potential;
    double const& pods_pressure_potential;
    double const& root_osmotic_potential;
    double const& stem_osmotic_potential;
    double const& leaf_osmotic_potential;
    double const& pods_osmotic_potential;
    double const& root_water_content;
    double const& stem_water_content;
    double const& leaf_water_content;
    double const& pods_water_content;
    double const& root_volume;
    double const& stem_volume;
    double const& leaf_volume;
    double const& pods_volume;
    double const& root_volume_plastic;
    double const& stem_volume_plastic;
    double const& leaf_volume_plastic;
    double const& pods_volume_plastic;
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
    double const& R_soil_root;
    double const& R_root_stem;
    double const& R_stem_leaf;
    double const& R_stem_pods;
    double const& soil_potential_avg;

    // Pointers to output quantities
    double* root_pressure_potential_op;
    double* stem_pressure_potential_op;
    double* leaf_pressure_potential_op;
    double* pods_pressure_potential_op;
    double* root_water_content_op;
    double* stem_water_content_op;
    double* leaf_water_content_op;
    double* pods_water_content_op;
    double* root_volume_op;
    double* stem_volume_op;
    double* leaf_volume_op;
    double* pods_volume_op;
    double* root_volume_plastic_op;
    double* stem_volume_plastic_op;
    double* leaf_volume_plastic_op;
    double* pods_volume_plastic_op;

    // Main operation
    void do_operation() const override;
};

}  // namespace BioCroWP
#endif