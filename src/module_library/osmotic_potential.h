#ifndef BioCroWP_OSMOTIC_POTENTIAL_H
#define BioCroWP_OSMOTIC_POTENTIAL_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace BioCroWP
{
class osmotic_potential : public direct_module
{
   public:
    osmotic_potential(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // Get references to input quantities
          sunlit_leaf_temperature_layer_0{get_input(input_quantities, "sunlit_leaf_temperature_layer_0")},
          sunlit_leaf_temperature_layer_1{get_input(input_quantities, "sunlit_leaf_temperature_layer_1")},
          sunlit_leaf_temperature_layer_2{get_input(input_quantities, "sunlit_leaf_temperature_layer_2")},
          sunlit_leaf_temperature_layer_3{get_input(input_quantities, "sunlit_leaf_temperature_layer_3")},
          sunlit_leaf_temperature_layer_4{get_input(input_quantities, "sunlit_leaf_temperature_layer_4")},
          sunlit_leaf_temperature_layer_5{get_input(input_quantities, "sunlit_leaf_temperature_layer_5")},
          sunlit_leaf_temperature_layer_6{get_input(input_quantities, "sunlit_leaf_temperature_layer_6")},
          sunlit_leaf_temperature_layer_7{get_input(input_quantities, "sunlit_leaf_temperature_layer_7")},
          sunlit_leaf_temperature_layer_8{get_input(input_quantities, "sunlit_leaf_temperature_layer_8")},
          sunlit_leaf_temperature_layer_9{get_input(input_quantities, "sunlit_leaf_temperature_layer_9")},

          shaded_leaf_temperature_layer_0{get_input(input_quantities, "shaded_leaf_temperature_layer_0")},
          shaded_leaf_temperature_layer_1{get_input(input_quantities, "shaded_leaf_temperature_layer_1")},
          shaded_leaf_temperature_layer_2{get_input(input_quantities, "shaded_leaf_temperature_layer_2")},
          shaded_leaf_temperature_layer_3{get_input(input_quantities, "shaded_leaf_temperature_layer_3")},
          shaded_leaf_temperature_layer_4{get_input(input_quantities, "shaded_leaf_temperature_layer_4")},
          shaded_leaf_temperature_layer_5{get_input(input_quantities, "shaded_leaf_temperature_layer_5")},
          shaded_leaf_temperature_layer_6{get_input(input_quantities, "shaded_leaf_temperature_layer_6")},
          shaded_leaf_temperature_layer_7{get_input(input_quantities, "shaded_leaf_temperature_layer_7")},
          shaded_leaf_temperature_layer_8{get_input(input_quantities, "shaded_leaf_temperature_layer_8")},
          shaded_leaf_temperature_layer_9{get_input(input_quantities, "shaded_leaf_temperature_layer_9")},

          //leaf_temperature{get_input(input_quantities, "leaf_temperature")},
          soil_temperature_avg{get_input(input_quantities, "soil_temperature_avg")},

        //   stem_soluble_sugar{get_input(input_qunatities, "stem_soluble_sugar")}, // waiting on partioning fraction
        //   leaf_soluble_sugar{get_input(input_qunatities, "leaf_soluble_sugar")},
        //   root_soluble_sugar{get_input(input_qunatities, "root_soluble_sugar")},

          root_volume{get_input(input_quantities, "root_volume")}, // Initial volume of organ, m3
          stem_volume{get_input(input_quantities, "stem_volume")},
          leaf_volume{get_input(input_quantities, "leaf_volume")},

          storage_water_frac{get_input(input_quantities, "storage_water_frac")},

          // Get pointers to output quantities
          stem_osmotic_potential_op{get_op(output_quantities, "stem_osmotic_potential")},
          leaf_osmotic_potential_op{get_op(output_quantities, "leaf_osmotic_potential")},
          root_osmotic_potential_op{get_op(output_quantities, "root_osmotic_potential")}    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "osmotic_potential"; }

   private:
    // References to input quantities
    double const & sunlit_leaf_temperature_layer_0;
    double const & sunlit_leaf_temperature_layer_1;
    double const & sunlit_leaf_temperature_layer_2;
    double const & sunlit_leaf_temperature_layer_3;
    double const & sunlit_leaf_temperature_layer_4;
    double const & sunlit_leaf_temperature_layer_5;
    double const & sunlit_leaf_temperature_layer_6;
    double const & sunlit_leaf_temperature_layer_7;
    double const & sunlit_leaf_temperature_layer_8;
    double const & sunlit_leaf_temperature_layer_9;

    double const & shaded_leaf_temperature_layer_0;
    double const & shaded_leaf_temperature_layer_1;
    double const & shaded_leaf_temperature_layer_2;
    double const & shaded_leaf_temperature_layer_3;
    double const & shaded_leaf_temperature_layer_4;
    double const & shaded_leaf_temperature_layer_5;
    double const & shaded_leaf_temperature_layer_6;
    double const & shaded_leaf_temperature_layer_7;
    double const & shaded_leaf_temperature_layer_8;
    double const & shaded_leaf_temperature_layer_9;

    // double const& leaf_temperature;
    double const& soil_temperature_avg;

    double const& root_volume;
    double const& stem_volume;
    double const& leaf_volume;

    double const& storage_water_frac;

    // Pointers to output quantities
    double* stem_osmotic_potential_op;
    double* leaf_osmotic_potential_op;
    double* root_osmotic_potential_op;

    // Main operation
    void do_operation() const;
};

}  // namespace BioCroWP
#endif