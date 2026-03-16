#ifndef GROWTH_MODULATOR_H
#define GROWTH_MODULATOR_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace BioCroWP
{

    /**
     * 
     * Turns organ growth on/off depending on whether
     * carbon allocation has exceeded a given threshold
     * 
     * growth_indicator = 0.0: no plastic organ growth
     * growth_indicator = 1.0: carbon is accumulating in organs,
     * plastic growth can occur
     * 
     */

class growth_modulator : public direct_module
{
    public:
      growth_modulator(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module{},

          // get references to input quantities
          kRoot{get_input(input_quantities, "kRoot")}, // dimensionless 
          kStem{get_input(input_quantities, "kStem")}, // fraction of carbon partitioned to organ
          kLeaf{get_input(input_quantities, "kLeaf")},
          kGrain{get_input(input_quantities, "kGrain")},

          growth_indicator_root{get_input(input_quantities, "growth_indicator_root")}, // dimensionless
          growth_indicator_stem{get_input(input_quantities, "growth_indicator_stem")},
          growth_indicator_leaf{get_input(input_quantities, "growth_indicator_leaf")},
          growth_indicator_pods{get_input(input_quantities, "growth_indicator_pods")},

          // pointers to output quantities
          growth_indicator_root_op{get_op(output_quantities, "growth_indicator_root")}, // dimensionless
          growth_indicator_stem_op{get_op(output_quantities, "growth_indicator_stem")},
          growth_indicator_leaf_op{get_op(output_quantities, "growth_indicator_leaf")},
          growth_indicator_pods_op{get_op(output_quantities, "growth_indicator_pods")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "growth_modulator"; }

  private:
    // references to input quantities

    const double& kRoot;
    const double& kStem;
    const double& kLeaf;
    const double& kGrain;

    const double& growth_indicator_root;
    const double& growth_indicator_stem;
    const double& growth_indicator_leaf;
    const double& growth_indicator_pods;

    // pointers to output quantities
    double* growth_indicator_root_op;
    double* growth_indicator_stem_op;
    double* growth_indicator_leaf_op;
    double* growth_indicator_pods_op;

    // main operation
    void do_operation() const;

};

string_vector growth_modulator::get_inputs()
{
  return {

    "kRoot", // dimensionless
    "kStem",
    "kLeaf",
    "kGrain",

    "growth_indicator_root",
    "growth_indicator_stem",
    "growth_indicator_leaf",
    "growth_indicator_pods",
  
  };
}

string_vector growth_modulator::get_outputs()
{
  return {

    "growth_indicator_root", // dimensionless
    "growth_indicator_stem",
    "growth_indicator_leaf",
    "growth_indicator_pods"

  };
}

void growth_modulator::do_operation() const
{

  // initial root growth on (growth_indicator = 1.0)
  // initial stem, leaf, pod growth off (growth_indicator = 0.0)

  // if carbon allocation is below the 0.005 threshold turn growth off
  double growth_state_root = growth_indicator_root;
  if (kRoot <= 0.005 && growth_state_root == 1.0) {
    growth_state_root = 0.0; // turn growth off
  } else if (kRoot <= 0.005 && growth_state_root == 0.0) {
    growth_state_root = 0.0; // growth should stay off
  } else if (kRoot >= 0.005 && growth_state_root == 1.0) {
    growth_state_root = 1.0; // growth should stay on
  }

  // if carbon allocation is above the 0.005 threshold turn growth on
  double growth_state_stem = growth_indicator_stem;
  if (kStem >= 0.005 && growth_state_stem == 0.0) {
    growth_state_stem = 1.0; // turn growth on for the season
  } else if (kStem <= 0.005 && growth_state_stem == 0.0) {
    growth_state_stem = 0.0; // growth should stay off
  } else if (kStem >= 0.005 && growth_state_stem == 1.0) {
    growth_state_stem = 1.0; // growth should stay on
  }

  double growth_state_leaf = growth_indicator_leaf;
  if (kLeaf >= 0.005 && growth_state_leaf == 0.0) {
    growth_state_leaf = 1.0; // turn growth on for the season
  } else if (kLeaf <= 0.005 && growth_state_leaf == 0.0) {
    growth_state_leaf = 0.0; // growth should stay off
  } else if (kLeaf >= 0.005 && growth_state_leaf == 1.0) {
    growth_state_leaf = 1.0; // growth should stay on
  }

  double growth_state_pods = growth_indicator_pods;
  if (kGrain >= 0.005 && growth_state_pods == 0.0) {
    growth_state_pods = 1.0; // turn growth on for the season
  } else if (kGrain <= 0.005 && growth_state_pods == 0.0) {
    growth_state_pods = 0.0; // growth should stay off
  } else if (kGrain >= 0.005 && growth_state_pods == 1.0) {
    growth_state_pods = 1.0; // growth should stay on
  }

  // update total biomass (g)
  update(growth_indicator_root_op, growth_state_root);
  update(growth_indicator_stem_op, growth_state_stem);
  update(growth_indicator_leaf_op, growth_state_leaf);
  update(growth_indicator_pods_op, growth_state_pods);
}

} // end of namespace
#endif