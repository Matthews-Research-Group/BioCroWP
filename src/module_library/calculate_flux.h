#ifndef BIOCROWP_CALCULATE_FLUX_H
#define BIOCROWP_CALCULATE_FLUX_H

#include "../framework/module.h"
#include "../framework/state_map.h"

namespace BioCroWP
{

class calculate_flux : public direct_module
{
   public:
    calculate_flux(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module(),

          // get references to input quantities
          root_total_potential{get_input(input_quantities, "root_total_potential")},
          stem_total_potential{get_input(input_quantities, "stem_total_potential")},
          leaf_total_potential{get_input(input_quantities, "leaf_total_potential")},
          pods_total_potential{get_input(input_quantities, "pods_total_potential")},

          R_soil_root{get_input(input_quantities, "R_soil_root")},
          R_root_stem{get_input(input_quantities, "R_root_stem")},
          R_stem_leaf{get_input(input_quantities, "R_stem_leaf")},
          R_stem_pods{get_input(input_quantities, "R_stem_pods")},

          soil_potential_avg{get_input(input_quantities, "soil_potential_avg")},

          kGrain{get_input(input_quantities, "kGrain")},

          n_plants{get_input(input_quantities, "n_plants")},

          // pointers to output quantities
          F_rwu_op{get_op(output_quantities, "F_rwu")},
          F_root_stem_op{get_op(output_quantities, "F_root_stem")},
          F_stem_leaf_op{get_op(output_quantities, "F_stem_leaf")},
          F_stem_pods_op{get_op(output_quantities, "F_stem_pods")}
    {
    }

    static string_vector get_inputs();
    static string_vector get_outputs();
    static std::string get_name() { return "calculate_flux"; }

   private:
    // references to input quantities
    double const& root_total_potential;
    double const& stem_total_potential;
    double const& leaf_total_potential;
    double const& pods_total_potential;

    double const& R_soil_root;
    double const& R_root_stem;
    double const& R_stem_leaf;
    double const& R_stem_pods;

    double const& soil_potential_avg;

    double const& kGrain;

    double const& n_plants;

    // pointers to output quantities
    double* F_rwu_op;
    double* F_root_stem_op;
    double* F_stem_leaf_op;
    double* F_stem_pods_op;

    // main operation
    void do_operation() const;
};

string_vector calculate_flux::get_inputs()
{
    return {
        "root_total_potential",  // MPa
        "stem_total_potential",
        "leaf_total_potential",
        "pods_total_potential",

        "R_soil_root",           // MPa h g-1 ha
        "R_root_stem",
        "R_stem_leaf",
        "R_stem_pods",

        "soil_potential_avg",    // MPa

        "kGrain",

        "n_plants"              // plants per hectare
    };
}

string_vector calculate_flux::get_outputs()
{
    return {
        "F_rwu",          // g ha-1 hr-1
        "F_root_stem",
        "F_stem_leaf",
        "F_stem_pods"
    };
}

void calculate_flux::do_operation() const
{
    // Calculate hectare-scale fluxes via Ohm's law (display only)
    double F_rwu_new = n_plants * ((soil_potential_avg - root_total_potential) / R_soil_root);           // g ha-1 hr-1
    double F_root_stem_new = n_plants * ((root_total_potential - stem_total_potential) / R_root_stem);
    double F_stem_leaf_new = n_plants * ((stem_total_potential - leaf_total_potential) / R_stem_leaf);
    double F_stem_pods_new = n_plants * ((stem_total_potential - pods_total_potential) / R_stem_pods);

    // if pod growth hasn't started F_stem_pods should be 0
    if (kGrain < 0.005) {
        F_stem_pods_new = 0.0;
    }

    update(F_rwu_op, F_rwu_new);
    update(F_root_stem_op, F_root_stem_new);
    update(F_stem_leaf_op, F_stem_leaf_new);
    update(F_stem_pods_op, F_stem_pods_new);
}

}  // namespace BioCroWP
#endif
