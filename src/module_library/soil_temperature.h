#ifndef BioCroWP_SOIL_TEMPERATURE_H
#define BioCroWP_SOIL_TEMPERATURE_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace BioCroWP
{
class soil_temperature : public direct_module
{
   public:
    soil_temperature(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // Get references to input quantities
          
          soil_depth_1{get_input(input_quantities, "soil_depth_1")}, // m
          soil_depth_2{get_input(input_quantities, "soil_depth_2")},
          soil_depth_3{get_input(input_quantities, "soil_depth_3")},
          soil_depth_4{get_input(input_quantities, "soil_depth_4")},
          soil_depth_5{get_input(input_quantities, "soil_depth_5")},
          soil_depth_6{get_input(input_quantities, "soil_depth_6")},

          soil_saturation_capacity_1{get_input(input_quantities, "soil_saturation_capacity_1")}, // Mg/m3
          soil_saturation_capacity_2{get_input(input_quantities, "soil_saturation_capacity_2")},
          soil_saturation_capacity_3{get_input(input_quantities, "soil_saturation_capacity_3")},
          soil_saturation_capacity_4{get_input(input_quantities, "soil_saturation_capacity_4")},
          soil_saturation_capacity_5{get_input(input_quantities, "soil_saturation_capacity_5")},
          soil_saturation_capacity_6{get_input(input_quantities, "soil_saturation_capacity_6")},

          soil_water_content_1{get_input(input_quantities, "soil_water_content_1")}, // volumetric swc
          soil_water_content_2{get_input(input_quantities, "soil_water_content_2")},
          soil_water_content_3{get_input(input_quantities, "soil_water_content_3")},
          soil_water_content_4{get_input(input_quantities, "soil_water_content_4")},
          soil_water_content_5{get_input(input_quantities, "soil_water_content_5")},
          soil_water_content_6{get_input(input_quantities, "soil_water_content_6")},

          max_rooting_layer{get_input(input_quantities, "max_rooting_layer")},

          minimum_temp_day{get_input(input_quantities, "minimum_temp_day")}, // degrees C, daily air temperature
          maximum_temp_day{get_input(input_quantities, "maximum_temp_day")},
          minimum_temp_year{get_input(input_quantities, "minimum_temp_year")}, // degrees C, daily air temperature
          maximum_temp_year{get_input(input_quantities, "maximum_temp_year")},

          soil_clay_content_1{get_input(input_quantities, "soil_clay_content_1")}, // % volume
          soil_clay_content_2{get_input(input_quantities, "soil_clay_content_2")},
          soil_clay_content_3{get_input(input_quantities, "soil_clay_content_3")},
          soil_clay_content_4{get_input(input_quantities, "soil_clay_content_4")},
          soil_clay_content_5{get_input(input_quantities, "soil_clay_content_5")},
          soil_clay_content_6{get_input(input_quantities, "soil_clay_content_6")},

          soil_type_indicator_1{get_input(input_quantities, "soil_type_indicator_1")}, // dimensionless
          soil_type_indicator_2{get_input(input_quantities, "soil_type_indicator_2")},
          soil_type_indicator_3{get_input(input_quantities, "soil_type_indicator_3")},
          soil_type_indicator_4{get_input(input_quantities, "soil_type_indicator_4")},
          soil_type_indicator_5{get_input(input_quantities, "soil_type_indicator_5")},
          soil_type_indicator_6{get_input(input_quantities, "soil_type_indicator_6")},

          hour{get_input(input_quantities, "hour")},
          doy{get_input(input_quantities, "doy")},
          start_doy{get_input(input_quantities, "start_doy")},

          // Get pointers to output quantities
          soil_temperature_1_op{get_op(output_quantities, "soil_temperature_1")}, // K, hourly
          soil_temperature_2_op{get_op(output_quantities, "soil_temperature_2")},
          soil_temperature_3_op{get_op(output_quantities, "soil_temperature_3")},
          soil_temperature_4_op{get_op(output_quantities, "soil_temperature_4")},
          soil_temperature_5_op{get_op(output_quantities, "soil_temperature_5")},
          soil_temperature_6_op{get_op(output_quantities, "soil_temperature_6")},
          soil_temperature_avg_op{get_op(output_quantities, "soil_temperature_avg")}

    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "soil_temperature"; }

   private:
    // References to input quantities
    double const& soil_depth_1;
    double const& soil_depth_2;
    double const& soil_depth_3;
    double const& soil_depth_4;
    double const& soil_depth_5;
    double const& soil_depth_6;

    double const& soil_saturation_capacity_1;
    double const& soil_saturation_capacity_2;
    double const& soil_saturation_capacity_3;
    double const& soil_saturation_capacity_4;
    double const& soil_saturation_capacity_5;
    double const& soil_saturation_capacity_6;

    double const& soil_water_content_1;
    double const& soil_water_content_2;
    double const& soil_water_content_3;
    double const& soil_water_content_4;
    double const& soil_water_content_5;
    double const& soil_water_content_6;

    double const& max_rooting_layer;

    double const& minimum_temp_day;
    double const& maximum_temp_day;
    double const& minimum_temp_year;
    double const& maximum_temp_year;

    double const& soil_clay_content_1;
    double const& soil_clay_content_2;
    double const& soil_clay_content_3;
    double const& soil_clay_content_4;
    double const& soil_clay_content_5;
    double const& soil_clay_content_6;

    double const& soil_type_indicator_1;
    double const& soil_type_indicator_2;
    double const& soil_type_indicator_3;
    double const& soil_type_indicator_4;
    double const& soil_type_indicator_5;
    double const& soil_type_indicator_6;

    double const& hour;
    double const& doy;
    double const& start_doy;

    // Pointers to output quantities
    double* soil_temperature_1_op;
    double* soil_temperature_2_op;
    double* soil_temperature_3_op;
    double* soil_temperature_4_op;
    double* soil_temperature_5_op;
    double* soil_temperature_6_op;
    double* soil_temperature_avg_op;

    // Main operation
    void do_operation() const;
};

}  // namespace BioCroWP
#endif