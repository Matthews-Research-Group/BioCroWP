#include "soil_potential.h"

using BioCroWP::soil_potential;

string_vector soil_potential::get_inputs()
{
  return {
    

    "max_rooting_layer",

    "soil_n_1",
    "soil_m_1",
    "soil_alpha_1",
    "soil_residual_wc_1",
    "soil_saturated_wc_1",

    "soil_n_2",
    "soil_m_2",
    "soil_alpha_2",
    "soil_residual_wc_2",
    "soil_saturated_wc_2",

    "soil_n_3",
    "soil_m_3",
    "soil_alpha_3",
    "soil_residual_wc_3",
    "soil_saturated_wc_3",

    "soil_n_4",
    "soil_m_4",
    "soil_alpha_4",
    "soil_residual_wc_4",
    "soil_saturated_wc_4",

    "soil_n_5",
    "soil_m_5",
    "soil_alpha_5",
    "soil_residual_wc_5",
    "soil_saturated_wc_5",

    "soil_n_6",
    "soil_m_6",
    "soil_alpha_6",
    "soil_residual_wc_6",
    "soil_saturated_wc_6",

    "soil_depth_1",
    "soil_water_content_1",

    "soil_depth_2",
    "soil_water_content_2",

    "soil_depth_3",
    "soil_water_content_3",

    "soil_depth_4",
    "soil_water_content_4",

    "soil_depth_5",
    "soil_water_content_5",

    "soil_depth_6",
    "soil_water_content_6",

    "soil_water_content",
  
  };
}

string_vector soil_potential::get_outputs()
{
  return {

    "soil_potential_1",
    "soil_potential_2",
    "soil_potential_3",
    "soil_potential_4",
    "soil_potential_5",
    "soil_potential_6",
    "soil_potential_avg",

    "soil_gravitational_potential_1",
    "soil_gravitational_potential_2",
    "soil_gravitational_potential_3",
    "soil_gravitational_potential_4",
    "soil_gravitational_potential_5",
    "soil_gravitational_potential_6",

    "soil_pressure_potential_1",
    "soil_pressure_potential_2",
    "soil_pressure_potential_3",
    "soil_pressure_potential_4",
    "soil_pressure_potential_5",
    "soil_pressure_potential_6",
    "soil_pressure_potential_avg"

  };
}

void soil_potential::do_operation() const
{

  // create variable arrays
  double soil_depth[] = { // m
      soil_depth_1/100,
      soil_depth_2/100,
      soil_depth_3/100,
      soil_depth_4/100,
      soil_depth_5/100,
      soil_depth_6/100
    };

  double soil_wc[] = {
    soil_water_content_1,
    soil_water_content_2,
    soil_water_content_3,
    soil_water_content_4,
    soil_water_content_5,
    soil_water_content_6
  };// dimensionless

  double soil_n[] = {
    soil_n_1,
    soil_n_2,
    soil_n_3,
    soil_n_4,
    soil_n_5,
    soil_n_6
  }; // dimensionless

  double soil_m[] = {
    soil_m_1,
    soil_m_2,
    soil_m_3,
    soil_m_4,
    soil_m_5,
    soil_m_6
  }; // dimensionless

  double soil_alpha[] = {
    soil_alpha_1,
    soil_alpha_2,
    soil_alpha_3,
    soil_alpha_4,
    soil_alpha_5,
    soil_alpha_6
  }; // cm-1

  double soil_residual_wc[] = {
    soil_residual_wc_1,
    soil_residual_wc_2,
    soil_residual_wc_3,
    soil_residual_wc_4,
    soil_residual_wc_5,
    soil_residual_wc_6
  }; // dimensionless

  double soil_saturated_wc[] = {
    soil_saturated_wc_1,
    soil_saturated_wc_2,
    soil_saturated_wc_3,
    soil_saturated_wc_4,
    soil_saturated_wc_5,
    soil_saturated_wc_6
  }; // dimensionless

  // define empty arrays
  double pressure_potential[6] = {0};
  double gravitational_potential[6] = {0};
  double total_potential[6] = {0};
  double tot_soil_depth[6] = {soil_depth[0], 0, 0, 0, 0, 0};

  // 1 m = 0.00979 MPa
  // 1 cm * density of water (998.2 kg/ m3) * gravity (9.81 m/s2) = 98.1 kg/(m*s2) = 98.1 Pa = 0.0000979 MPa
  double convf = 0.0000979; // head (cm) to potential (MPa)

  // calculate cummulative depth at each soil layer
  for(int l = 1; l < max_rooting_layer; l++){
    tot_soil_depth[l] = tot_soil_depth[l-1] + soil_depth[l];
  }

  // calculate pressure head and gravitational potential for a given soil layer and convert from cm to MPa
  int saturated_layer_counter = 0;
  double z_wt;
  for(int l = 0; l < max_rooting_layer; l++){

    // unsaturated soil calculation, pressure (matric) potential is negative
    if (soil_wc[l] < soil_saturated_wc[l]) { 
      saturated_layer_counter = 0; // if a layer is not saturated reset index to 0
      pressure_potential[l] = -convf*(1.0/soil_alpha[l])*pow((pow(((soil_saturated_wc[l] - soil_residual_wc[l])/(soil_wc[l] - soil_residual_wc[l])),
                                                                  (1.0/soil_m[l])) - 1.0),
                                                                  (1.0/soil_n[l])); // MPa, Van Genuchten (1980)

    // saturated soil calculation, pressure potential is positive
    } else {
      if (l == 0){
        z_wt = 0.0;
      } else if (saturated_layer_counter == 0) {
        z_wt = tot_soil_depth[l-1]; // setting water table height to be the top of the first saturated soil layer
      }
      saturated_layer_counter += 1;
      pressure_potential[l] = -(convf*100)*(z_wt - (tot_soil_depth[l] - (soil_depth[l]/2)));
    }
    gravitational_potential[l] = (convf*100)*(0 - (tot_soil_depth[l] - (soil_depth[l]/2))); // calculated using soil depth in the middle of the layer
    total_potential[l] = pressure_potential[l] + gravitational_potential[l];
  }

  // weighted averages
  // double pressure_potential_avg;
  int l_max = max_rooting_layer - 1;

  double pressure_sum = 0.0;
  double grav_sum = 0.0;
  double total_sum = 0.0;

  for (int l = 0; l < max_rooting_layer; l++){
    pressure_sum += pressure_potential[l]*soil_depth[l];
    grav_sum += gravitational_potential[l]*soil_depth[l];
    total_sum += total_potential[l]*soil_depth[l];
  }

  double pressure_potential_avg = pressure_sum/tot_soil_depth[l_max];
  double grav_potential_avg = grav_sum/tot_soil_depth[l_max];
  double total_potential_avg = total_sum/tot_soil_depth[l_max];

  // update soil potential
  update(soil_potential_1_op, total_potential[0]);
  update(soil_potential_2_op, total_potential[1]);
  update(soil_potential_3_op, total_potential[2]);
  update(soil_potential_4_op, total_potential[3]);
  update(soil_potential_5_op, total_potential[4]);
  update(soil_potential_6_op, total_potential[5]);
  update(soil_potential_avg_op, total_potential_avg);

  update(soil_gravitational_potential_1_op, gravitational_potential[0]);
  update(soil_gravitational_potential_2_op, gravitational_potential[1]);
  update(soil_gravitational_potential_3_op, gravitational_potential[2]);
  update(soil_gravitational_potential_4_op, gravitational_potential[3]);
  update(soil_gravitational_potential_5_op, gravitational_potential[4]);
  update(soil_gravitational_potential_6_op, gravitational_potential[5]);

  update(soil_pressure_potential_1_op, pressure_potential[0]);
  update(soil_pressure_potential_2_op, pressure_potential[1]);
  update(soil_pressure_potential_3_op, pressure_potential[2]);
  update(soil_pressure_potential_4_op, pressure_potential[3]);
  update(soil_pressure_potential_5_op, pressure_potential[4]);
  update(soil_pressure_potential_6_op, pressure_potential[5]);
  update(soil_pressure_potential_avg_op, pressure_potential_avg);
}