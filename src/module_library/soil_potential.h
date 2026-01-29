#ifndef SOIL_POTENTIAL_H
#define SOIL_POTENTIAL_H

#include "../framework/module.h"
#include "../framework/state_map.h"
#include <cmath>
#include <vector>

namespace BioCroWP
{

class soil_potential : public direct_module
{
    public:
      soil_potential(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

            // get references to input quantities
            max_rooting_layer{get_input(input_quantities, "max_rooting_layer")}, // dimensionless

            soil_n_1{get_input(input_quantities, "soil_n_1")}, // dimensionless
            soil_m_1{get_input(input_quantities, "soil_m_1")}, // dimensionless
            soil_alpha_1{get_input(input_quantities, "soil_alpha_1")}, // cm-1
            soil_residual_wc_1{get_input(input_quantities, "soil_residual_wc_1")}, // dimensionless
            soil_saturated_wc_1{get_input(input_quantities, "soil_saturated_wc_1")}, // dimensionless

            soil_n_2{get_input(input_quantities, "soil_n_2")},
            soil_m_2{get_input(input_quantities, "soil_m_2")},
            soil_alpha_2{get_input(input_quantities, "soil_alpha_2")},
            soil_residual_wc_2{get_input(input_quantities, "soil_residual_wc_2")},
            soil_saturated_wc_2{get_input(input_quantities, "soil_saturated_wc_2")},

            soil_n_3{get_input(input_quantities, "soil_n_3")},
            soil_m_3{get_input(input_quantities, "soil_m_3")},
            soil_alpha_3{get_input(input_quantities, "soil_alpha_3")},
            soil_residual_wc_3{get_input(input_quantities, "soil_residual_wc_3")},
            soil_saturated_wc_3{get_input(input_quantities, "soil_saturated_wc_3")},

            soil_n_4{get_input(input_quantities, "soil_n_4")},
            soil_m_4{get_input(input_quantities, "soil_m_4")},
            soil_alpha_4{get_input(input_quantities, "soil_alpha_4")},
            soil_residual_wc_4{get_input(input_quantities, "soil_residual_wc_4")},
            soil_saturated_wc_4{get_input(input_quantities, "soil_saturated_wc_4")},

            soil_n_5{get_input(input_quantities, "soil_n_5")},
            soil_m_5{get_input(input_quantities, "soil_m_5")},
            soil_alpha_5{get_input(input_quantities, "soil_alpha_5")},
            soil_residual_wc_5{get_input(input_quantities, "soil_residual_wc_5")},
            soil_saturated_wc_5{get_input(input_quantities, "soil_saturated_wc_5")},

            soil_n_6{get_input(input_quantities, "soil_n_6")},
            soil_m_6{get_input(input_quantities, "soil_m_6")},
            soil_alpha_6{get_input(input_quantities, "soil_alpha_6")},
            soil_residual_wc_6{get_input(input_quantities, "soil_residual_wc_6")},
            soil_saturated_wc_6{get_input(input_quantities, "soil_saturated_wc_6")},

            soil_depth_1{get_input(input_quantities, "soil_depth_1")}, // cm
            soil_water_content_1{get_input(input_quantities, "soil_water_content_1")}, // cm3 [water] / cm3 [soil]

            soil_depth_2{get_input(input_quantities, "soil_depth_2")},
            soil_water_content_2{get_input(input_quantities, "soil_water_content_2")},

            soil_depth_3{get_input(input_quantities, "soil_depth_3")},
            soil_water_content_3{get_input(input_quantities, "soil_water_content_3")},

            soil_depth_4{get_input(input_quantities, "soil_depth_4")},
            soil_water_content_4{get_input(input_quantities, "soil_water_content_4")},

            soil_depth_5{get_input(input_quantities, "soil_depth_5")},
            soil_water_content_5{get_input(input_quantities, "soil_water_content_5")},

            soil_depth_6{get_input(input_quantities, "soil_depth_6")},
            soil_water_content_6{get_input(input_quantities, "soil_water_content_6")},

            soil_water_content{get_input(input_quantities, "soil_water_content")},

            // pointers to output quantities
            soil_potential_1_op{get_op(output_quantities, "soil_potential_1")}, // MPa
            soil_potential_2_op{get_op(output_quantities, "soil_potential_2")},
            soil_potential_3_op{get_op(output_quantities, "soil_potential_3")},
            soil_potential_4_op{get_op(output_quantities, "soil_potential_4")},
            soil_potential_5_op{get_op(output_quantities, "soil_potential_5")},
            soil_potential_6_op{get_op(output_quantities, "soil_potential_6")},
            soil_potential_avg_op{get_op(output_quantities, "soil_potential_avg")},

            soil_gravitational_potential_1_op{get_op(output_quantities, "soil_gravitational_potential_1")}, // MPa
            soil_gravitational_potential_2_op{get_op(output_quantities, "soil_gravitational_potential_2")},
            soil_gravitational_potential_3_op{get_op(output_quantities, "soil_gravitational_potential_3")},
            soil_gravitational_potential_4_op{get_op(output_quantities, "soil_gravitational_potential_4")},
            soil_gravitational_potential_5_op{get_op(output_quantities, "soil_gravitational_potential_5")},
            soil_gravitational_potential_6_op{get_op(output_quantities, "soil_gravitational_potential_6")},

            soil_pressure_potential_1_op{get_op(output_quantities, "soil_pressure_potential_1")}, // MPa
            soil_pressure_potential_2_op{get_op(output_quantities, "soil_pressure_potential_2")},
            soil_pressure_potential_3_op{get_op(output_quantities, "soil_pressure_potential_3")},
            soil_pressure_potential_4_op{get_op(output_quantities, "soil_pressure_potential_4")},
            soil_pressure_potential_5_op{get_op(output_quantities, "soil_pressure_potential_5")},
            soil_pressure_potential_6_op{get_op(output_quantities, "soil_pressure_potential_6")},
            soil_pressure_potential_avg_op{get_op(output_quantities, "soil_pressure_potential_avg")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "soil_potential"; }

  private:
    // references to input quantities

    double const& max_rooting_layer;

    double const& soil_n_1;
    double const& soil_m_1;
    double const& soil_alpha_1;
    double const& soil_residual_wc_1;
    double const& soil_saturated_wc_1;

    double const& soil_n_2;
    double const& soil_m_2;
    double const& soil_alpha_2;
    double const& soil_residual_wc_2;
    double const& soil_saturated_wc_2;

    double const& soil_n_3;
    double const& soil_m_3;
    double const& soil_alpha_3;
    double const& soil_residual_wc_3;
    double const& soil_saturated_wc_3;

    double const& soil_n_4;
    double const& soil_m_4;
    double const& soil_alpha_4;
    double const& soil_residual_wc_4;
    double const& soil_saturated_wc_4;

    double const& soil_n_5;
    double const& soil_m_5;
    double const& soil_alpha_5;
    double const& soil_residual_wc_5;
    double const& soil_saturated_wc_5;

    double const& soil_n_6;
    double const& soil_m_6;
    double const& soil_alpha_6;
    double const& soil_residual_wc_6;
    double const& soil_saturated_wc_6;

    double const& soil_depth_1;
    double const& soil_water_content_1;

    double const& soil_depth_2;
    double const& soil_water_content_2;

    double const& soil_depth_3;
    double const& soil_water_content_3;

    double const& soil_depth_4;
    double const& soil_water_content_4;

    double const& soil_depth_5;
    double const& soil_water_content_5;

    double const& soil_depth_6;
    double const& soil_water_content_6;

    double const& soil_water_content;

    // pointers to output quantities
    double* soil_potential_1_op;
    double* soil_potential_2_op;
    double* soil_potential_3_op;
    double* soil_potential_4_op;
    double* soil_potential_5_op;
    double* soil_potential_6_op;
    double* soil_potential_avg_op;

    double* soil_gravitational_potential_1_op;
    double* soil_gravitational_potential_2_op;
    double* soil_gravitational_potential_3_op;
    double* soil_gravitational_potential_4_op;
    double* soil_gravitational_potential_5_op;
    double* soil_gravitational_potential_6_op;

    double* soil_pressure_potential_1_op;
    double* soil_pressure_potential_2_op;
    double* soil_pressure_potential_3_op;
    double* soil_pressure_potential_4_op;
    double* soil_pressure_potential_5_op;
    double* soil_pressure_potential_6_op;
    double* soil_pressure_potential_avg_op;

    // main operation
    void do_operation() const;

};

} // end of namespace
#endif