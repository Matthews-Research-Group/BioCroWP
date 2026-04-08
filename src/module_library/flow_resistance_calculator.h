#ifndef FLOW_RESISTANCE_CALCULATOR_H
#define FLOW_RESISTANCE_CALCULATOR_H

#include "../framework/module.h"
#include "../framework/state_map.h"

class flow_resistance_calculator : public direct_module
{
   public:
    flow_resistance_calculator(
        state_map const& input_quantities,
        state_map* output_quantities);

    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "flow_resistance_calculator"; }

   private:
    // References to input quantities

    double const& root_water_content;
    double const& stem_water_content;
    double const& leaf_water_content;
    double const& pods_water_content;

    double const& root_volume_plastic;
    double const& stem_volume_plastic;
    double const& leaf_volume_plastic;
    double const& pods_volume_plastic;

    double const& n_resistance;

    double const& R_soil_root_base;
    double const& R_root_stem_base;
    double const& R_stem_leaf_base;
    double const& R_stem_pods_base;

    double const& Root;
    double const& Stem;
    double const& Leaf;
    double const& Grain;

    // Pointers to output quantities
    double* R_soil_root_op;
    double* R_root_stem_op;
    double* R_stem_leaf_op;
    double* R_stem_pods_op;

    double* RWC_max;
    double* SWC_max;
    double* LWC_max;
    double* PWC_max;

    // Main operation
    void do_operation() const;
};

#endif