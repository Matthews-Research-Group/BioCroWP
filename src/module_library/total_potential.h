#ifndef TOTAL_POTENTIAL_H
#define TOTAL_POTENTIAL_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace BioCroWP
{

class total_potential : public direct_module
{
    public:
      total_potential(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // get references to input quantities
          root_pressure_potential{get_input(input_quantities, "root_pressure_potential")}, // MPa
          stem_pressure_potential{get_input(input_quantities, "stem_pressure_potential")},
          leaf_pressure_potential{get_input(input_quantities, "leaf_pressure_potential")},
          pods_pressure_potential{get_input(input_quantities, "pods_pressure_potential")},

          root_osmotic_potential{get_input(input_quantities, "root_osmotic_potential")}, // MPa
          stem_osmotic_potential{get_input(input_quantities, "stem_osmotic_potential")},
          leaf_osmotic_potential{get_input(input_quantities, "leaf_osmotic_potential")},
          pods_osmotic_potential{get_input(input_quantities, "pods_osmotic_potential")},

          // pointers to output quantities
          root_total_potential_op{get_op(output_quantities, "root_total_potential")}, // MPa
          stem_total_potential_op{get_op(output_quantities, "stem_total_potential")},
          leaf_total_potential_op{get_op(output_quantities, "leaf_total_potential")},
          pods_total_potential_op{get_op(output_quantities, "pods_total_potential")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "total_potential"; }

  private:
    // references to input quantities

    double const& root_pressure_potential;
    double const& stem_pressure_potential;
    double const& leaf_pressure_potential;
    double const& pods_pressure_potential;

    double const& root_osmotic_potential;
    double const& stem_osmotic_potential;
    double const& leaf_osmotic_potential;
    double const& pods_osmotic_potential;

    // pointers to output quantities
    double* root_total_potential_op;
    double* stem_total_potential_op;
    double* leaf_total_potential_op;
    double* pods_total_potential_op;

    // main operation
    void do_operation() const;

};

string_vector total_potential::get_inputs()
{
  return {

    "root_pressure_potential",
    "stem_pressure_potential",
    "leaf_pressure_potential",
    "pods_pressure_potential",

    "root_osmotic_potential",         // KPa
    "stem_osmotic_potential",
    "leaf_osmotic_potential",
    "pods_osmotic_potential"
  
  };
}

string_vector total_potential::get_outputs()
{
  return {

    "root_total_potential",
    "stem_total_potential",
    "leaf_total_potential",
    "pods_total_potential"

  };
}

void total_potential::do_operation() const
{
  // update total potential
  update(root_total_potential_op, root_pressure_potential + root_osmotic_potential);
  update(stem_total_potential_op, stem_pressure_potential + stem_osmotic_potential);
  update(leaf_total_potential_op, leaf_pressure_potential + leaf_osmotic_potential);
  update(pods_total_potential_op, pods_pressure_potential + pods_osmotic_potential);
}

} // end of namespace
#endif