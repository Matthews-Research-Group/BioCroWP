#ifndef LONGITUDINAL_EXTENSIBILITY_UPDATE_H
#define LONGITUDINAL_EXTENSIBILITY_UPDATE_H

#include <cmath>

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace BioCroWP
{

class longitudinal_extensibility_update : public direct_module
{
    public:
      longitudinal_extensibility_update(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // get references to input quantities
          DVI{get_input(input_quantities, "DVI")},
          ext_root_z_initial{get_input(input_quantities, "ext_root_z_initial")},
          ext_stem_z_initial{get_input(input_quantities, "ext_stem_z_initial")},
          ext_leaf_y_initial{get_input(input_quantities, "ext_leaf_y_initial")},
          ext_leaf_z_initial{get_input(input_quantities, "ext_leaf_z_initial")},
          ext_pods_x_initial{get_input(input_quantities, "ext_pods_x_initial")},

          t_root_m{get_input(input_quantities, "t_root_m")}, // explain all the paramters please
          t_root_e{get_input(input_quantities, "t_root_e")},
          t_stem_m{get_input(input_quantities, "t_stem_m")},
          t_stem_e{get_input(input_quantities, "t_stem_e")},
          t_leaf_m{get_input(input_quantities, "t_leaf_m")},
          t_leaf_e{get_input(input_quantities, "t_leaf_e")},
          t_pods_m{get_input(input_quantities, "t_pods_m")},
          t_pods_e{get_input(input_quantities, "t_pods_e")},

          kRoot{get_input(input_quantities, "kRoot")}, // fraction of carbon allocated to each organ
          kStem{get_input(input_quantities, "kStem")}, // using to modulate organ development start time
          kLeaf{get_input(input_quantities, "kLeaf")},
          kGrain{get_input(input_quantities, "kGrain")},

          Root{get_input(input_quantities, "Root")}, // Organ dry biomass (Mg/ha)
          Stem{get_input(input_quantities, "Stem")}, // using to modulate organ development start time
          Leaf{get_input(input_quantities, "Leaf")},
          Grain{get_input(input_quantities, "Grain")},

          // pointers to output quantities
          ext_root_z_op{get_op(output_quantities, "ext_root_z")},
          ext_stem_z_op{get_op(output_quantities, "ext_stem_z")},
          ext_leaf_y_op{get_op(output_quantities, "ext_leaf_y")},
          ext_leaf_z_op{get_op(output_quantities, "ext_leaf_z")},
          ext_pods_x_op{get_op(output_quantities, "ext_pods_x")}
    {
    }
    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "longitudinal_extensibility_update"; }

  private:
    // references to input quantities

    double const& DVI;
    double const& ext_root_z_initial;
    double const& ext_stem_z_initial;
    double const& ext_leaf_y_initial;
    double const& ext_leaf_z_initial;
    double const& ext_pods_x_initial;

    double const& t_root_m;
    double const& t_root_e;
    double const& t_stem_m;
    double const& t_stem_e;
    double const& t_leaf_m;
    double const& t_leaf_e;
    double const& t_pods_m;
    double const& t_pods_e;

    double const& kRoot;
    double const& kStem;
    double const& kLeaf;
    double const& kGrain;

    double const& Root;
    double const& Stem;
    double const& Leaf;
    double const& Grain;

    // pointers to output quantities
    double* ext_root_z_op;
    double* ext_stem_z_op;
    double* ext_leaf_y_op;
    double* ext_leaf_z_op;
    double* ext_pods_x_op;

    // main operation
    void do_operation() const;

};

string_vector longitudinal_extensibility_update::get_inputs()
{
  return {

    "DVI",
    "ext_root_z_initial",
    "ext_stem_z_initial",
    "ext_leaf_y_initial",
    "ext_leaf_z_initial",
    "ext_pods_x_initial",

    "t_root_m",
    "t_root_e",
    "t_stem_m",
    "t_stem_e",
    "t_leaf_m",
    "t_leaf_e",
    "t_pods_m",
    "t_pods_e",

    "kRoot",
    "kStem",
    "kLeaf",
    "kGrain",

    "Root",
    "Stem",
    "Leaf",
    "Grain",
  
  };
}

string_vector longitudinal_extensibility_update::get_outputs()
{
  return {

    "ext_root_z",
    "ext_stem_z",
    "ext_leaf_y",
    "ext_leaf_z",
    "ext_pods_x",

  };
}

void longitudinal_extensibility_update::do_operation() const
{

  // root growth starts at sowing (DVI = -1)
  // root growth endpoint when DVI = 1 (flowering), midpoint when DVI = 0
  // double t_root_m = 0.0;
  // double t_root_e = 1.0;
  double ext_root_z_update = 0; // initialize extensibility to 0
  if (kRoot < 0.005) { // organ is not growing, keep extensibility 0
    ext_root_z_update = 0;
  } else if (kRoot >= 0.005 && Root >= 0.01 && DVI <= t_root_e) { // root is growing and development index is below endpoint, change ext
    ext_root_z_update = ext_root_z_initial*(1 - (1 + ((t_root_e - DVI)/(t_root_e - t_root_m)))
                                                *pow((DVI/t_root_e), (t_root_e/(t_root_e - t_root_m))));
  }
  

  // stem and leaf growth starts with emergence (DVI = 0)
  // stem and leaf growth endpoint when DVI = 2 (maturity), midpoint when DVI = 1.0
  // double t_stem_m = 1.0;
  // double t_stem_e = 2.0;
  double ext_stem_z_update = 0;
  if (kStem < 0.005) { // if organ is not growing extensibility is 0
    double ext_stem_z_update = 0;
  } else if (kStem >= 0.005 && Stem >= 0.01 && DVI <= t_stem_e) { // if organ is growing calculate extensibility
    ext_stem_z_update = ext_stem_z_initial*(1 - (1 + ((t_stem_e - DVI)/(t_stem_e - t_stem_m)))
                                                  *pow((DVI/t_stem_e), (t_stem_e/(t_stem_e - t_stem_m))));

  }

  double ext_leaf_y_update = 0;
  double ext_leaf_z_update = 0;
  if (kLeaf < 0.005) {
    ext_leaf_y_update = 0;
    ext_leaf_z_update = 0;
  } else if (kLeaf >= 0.005 && Leaf >= 0.01 && DVI <= t_leaf_e) {
    ext_leaf_y_update = ext_leaf_y_initial*(1 - (1 + ((t_leaf_e - DVI)/(t_leaf_e - t_leaf_m)))
                                                    *pow((DVI/t_leaf_e), (t_leaf_e/(t_leaf_e - t_leaf_m))));
    ext_leaf_z_update = ext_leaf_z_initial*(1 - (1 + ((t_leaf_e - DVI)/(t_leaf_e - t_leaf_m)))
                                                    *pow((DVI/t_leaf_e), (t_leaf_e/(t_leaf_e - t_leaf_m))));
  }
  

  // double t_leaf_m = 1.0;
  // double t_leaf_e = 2.0;



  // pod growth starts at flowering (DVI = 1)
  // pod growth endpoint when DVI = 2.5 (end of season), midpoint when DVI = 1.75
  // double t_pods_m = 1.75;
  // double t_pods_e = 2.5;
  double ext_pods_x_update;
  if (kGrain < 0.005) {
    ext_pods_x_update = 0;
  } else if (kGrain >= 0.005 && Grain > 0.01) {
    ext_pods_x_update = ext_pods_x_initial*(1 - (1 + ((t_pods_e - DVI)/(t_pods_e - t_pods_m)))
                                                  *pow((DVI/t_pods_e), (t_pods_e/(t_pods_e - t_pods_m))));
  }

  
  

  // update total biomass (g)
  update(ext_root_z_op, ext_root_z_update);
  update(ext_stem_z_op, ext_stem_z_update);
  update(ext_leaf_y_op, ext_leaf_y_update);
  update(ext_leaf_z_op, ext_leaf_z_update);
  update(ext_pods_x_op, ext_pods_x_update);
}

} // end of namespace
#endif