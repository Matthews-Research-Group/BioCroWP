#ifndef TOTAL_BIOMASS_H
#define TOTAL_BIOMASS_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace BioCroWP
{

class total_biomass : public direct_module
{
    public:
      total_biomass(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // get references to input quantities
          root_water_content{get_input(input_quantities, "root_water_content")}, // g, wet biomass
          stem_water_content{get_input(input_quantities, "stem_water_content")},
          leaf_water_content{get_input(input_quantities, "leaf_water_content")},
          pods_water_content{get_input(input_quantities, "pods_water_content")},

          Root{get_input(input_quantities, "Root")}, // Mg, dry biomass
          Stem{get_input(input_quantities, "Stem")},
          Leaf{get_input(input_quantities, "Leaf")},
          Grain{get_input(input_quantities, "Grain")},

          // pointers to output quantities
          root_total_biomass_op{get_op(output_quantities, "root_total_biomass")}, // g
          stem_total_biomass_op{get_op(output_quantities, "stem_total_biomass")},
          leaf_total_biomass_op{get_op(output_quantities, "leaf_total_biomass")},
          pods_total_biomass_op{get_op(output_quantities, "pods_total_biomass")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "total_biomass"; }

  private:
    // references to input quantities

    double const& root_water_content;
    double const& stem_water_content;
    double const& leaf_water_content;
    double const& pods_water_content;

    double const& Root;
    double const& Stem;
    double const& Leaf;
    double const& Grain;

    // pointers to output quantities
    double* root_total_biomass_op;
    double* stem_total_biomass_op;
    double* leaf_total_biomass_op;
    double* pods_total_biomass_op;

    // main operation
    void do_operation() const;

};

string_vector total_biomass::get_inputs()
{
  return {

    "root_water_content", // g
    "stem_water_content",
    "leaf_water_content",
    "pods_water_content",

    "Root",         // Mg
    "Stem",
    "Leaf",
    "Grain"
  
  };
}

string_vector total_biomass::get_outputs()
{
  return {

    "root_total_biomass", // g
    "stem_total_biomass",
    "leaf_total_biomass",
    "pods_total_biomass"

  };
}

void total_biomass::do_operation() const
{

  // converting dry biomass units from Mg to g
  double root_g = Root*100000;
  double stem_g = Stem*100000;
  double leaf_g = Leaf*100000;
  double pods_g = Grain*100000;

  // update total biomass (g)
  update(root_total_biomass_op, root_g + root_water_content);
  update(stem_total_biomass_op, stem_g + stem_water_content);
  update(leaf_total_biomass_op, leaf_g + leaf_water_content);
  update(pods_total_biomass_op, pods_g + pods_water_content);
}

} // end of namespace
#endif