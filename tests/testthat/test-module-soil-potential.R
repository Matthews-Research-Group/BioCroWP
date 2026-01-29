library(BioCro)
library(BioCroWP)
library(BioCroWater)

load("C:/Users/natal/OneDrive/Documents/masters/research/data/soybean_weather_wp.rdata")
weatherData <- merged_soybean_weather$'2002'
#simulated <- seq(from = 0.1, to = 0.44, length.out = 907)
#leftover <- rep(0.44, 3288 - 907)
#x <- c(simulated, leftover)
#weatherData$soil_water_content <- x

# listing direct modules
direct_modules_water_potential = list("BioCroWater:soil_type_selector", "BioCroWater:soil_surface_runoff",
                                      "BioCroWater:soil_water_downflow", "BioCroWater:soil_water_tiledrain", 
                                      "BioCroWater:soil_water_upflow","BioCroWater:soil_water_uptake",
                                      "BioCroWater:multilayer_soil_profile_avg",
                                      "BioCroWP:soil_potential",
                                      "BioCroWP:osmotic_potential",
                                      "BioCroWP:total_potential",
                                      "BioCroWP:total_biomass")
direct_modules_new = soybean$direct_modules
old_soil_evapo_index = which(direct_modules_new=="BioCro:soil_evaporation")
direct_modules_new = direct_modules_new[-old_soil_evapo_index] #remove soil evaporation
direct_modules = c(direct_modules_new[1:(old_soil_evapo_index-1)],direct_modules_water_potential,
                   direct_modules_new[old_soil_evapo_index:length(direct_modules_new)]) # insert BioCroWP

# listing differential modules
differential_modules_water_potential = list("BioCroWater:soil_evaporation","BioCroWater:multi_layer_soil_profile",
                                            "BioCroWP:pressure_potential")
differential_modules_new = soybean$differential_modules
old_soil_profile_index = which(differential_modules_new=="BioCro:two_layer_soil_profile")
differential_modules_new = differential_modules_new[-old_soil_profile_index] #remove soil_profile
differential_modules = c(differential_modules_new[1:(old_soil_profile_index-1)],
                         differential_modules_water_potential,
                         differential_modules_new[old_soil_profile_index:length(differential_modules_new)])

# set initial values
init_values =   within(soybean$initial_values,{
  #canopy_transpiration_rate = 0
  soil_water_content_1 = soybean$initial_values$soil_water_content  #0-5cm
  soil_water_content_2 = soybean$initial_values$soil_water_content  #5-15cm
  soil_water_content_3 = soybean$initial_values$soil_water_content  #15-35cm
  soil_water_content_4 = soybean$initial_values$soil_water_content  #35-55 cm
  soil_water_content_5 = soybean$initial_values$soil_water_content  #55-75 cm
  soil_water_content_6 = soybean$initial_values$soil_water_content  #75-100cm
  sumes1               = 0.125
  sumes2               = 0
  time_factor = 0
  soil_evaporation_rate = 0
  pods_water_content = 0
  root_water_content = 0 #878.416
  stem_water_content = 0 #6912.2555
  leaf_water_content = 0 #2505.24243
  pods_volume = 1
  root_volume = 1
  stem_volume = 1
  leaf_volume = 1 # no leaves in initial development stages
  pods_pressure_potential = -0.5
  root_pressure_potential = -0.04  # Initial value to be updated
  stem_pressure_potential = -0.3
  leaf_pressure_potential = -0.5
  pods_osmotic_potential = -0.1
  #root_total_potential = -0.05  # For initial water flow calculation
  #stem_total_potential = -0.4
  #leaf_total_potential = -0.6
  leaf_temperature = 298.15
  #soil_temperature_avg = 298.15
  kd = 1 # Not sure what this should be
})
init_values$soil_water_content=NULL

# set parameters
soiltype = 3 
parameters =   within(soybean$parameters, {
  irrigation = 0.0
  swcon = 0.05/24
  curve_number = 61
  soil_type_indicator_1 = soiltype
  soil_type_indicator_2 = soiltype
  soil_type_indicator_3 = soiltype
  soil_type_indicator_4 = soiltype
  soil_type_indicator_5 = soiltype
  soil_type_indicator_6 = soiltype
  soil_depth_1 = 5
  soil_depth_2 = 10
  soil_depth_3 = 20
  soil_depth_4 = 20
  soil_depth_5 = 20
  soil_depth_6 = 25
  tile_drainage_rate          = 0.2 # dimensionless, as in DSSAT (1/d ?)
  tile_drain_depth            = 90  # cm (typical value 3-4 ft)
  skc    = 0.3
  kcbmax = 0.25
  elevation                   = 219
  bare_soil_albedo            = 0.15
  max_rooting_layer = 4
  ext_root_x = 0.055
  ext_root_z = 0.275
  ext_stem_x = 0.055
  ext_stem_z = 0.275
  ext_leaf_x = 0.0
  ext_leaf_y = 0.55
  ext_leaf_z = 0.55
  ext_pods_x = 0.2
  mod_root_x = 57
  mod_root_z = 57
  mod_stem_x = 57
  mod_stem_z = 57
  mod_leaf_x = 9
  mod_leaf_y = 2
  mod_leaf_z = 9
  mod_pods_x = 9
  wp_crit = 0.4
  storage_water_frac = 0.8
  R_soil_root = 1
  R_root_stem = 1
  R_stem_leaf = 1
  R_stem_pods = 1
  soil_temperature_avg = 298.15
  #minimum_temp_day = 20.9
  #maximum_temp_day = 32.3
})
parameters[c('soil_field_capacity','soil_saturated_conductivity','soil_saturation_capacity','soil_wilting_point')]= NULL
parameters[c('kShell','net_assimilation_rate_shell')] = NULL
  
result <- run_biocro(
  init_values,
  parameters,
  weatherData,
  direct_modules,
  differential_modules,
  ode_solver=list(
    type = 'boost_rosenbrock',
    output_step_size = 1.0,
    adaptive_rel_error_tol = 1e-4,
    adaptive_abs_error_tol = 1e-4,
    adaptive_max_steps = 200
  )
)

plot(result$time, result$soil_water_content, xlab='time',ylab='soil water content')
plot(result$soil_water_content_1, result$soil_pressure_potential_1)

#plot(-result$soil_water_content_1, result$soil_pressure_potential_1, col = "blue",
 #    xlim = c(0,0.5),
  #   ylim = c(0, -0.1),
   #  log = "y",
    # type = "p",
   #  xlab = "Volumetric Soil Water Content (%)",
    # ylab = "- Matric Potential (MPa)",
   #  main = "Soil Water Retention Curve",
   #  cex = 1)

legend("bottomleft", 
       legend = c("Layer 1", "Layer 2", "Layer 3", "Layer 4"),
       col = c("blue", "red", "green", "orange"),
       lwd = c(2, 2, 2, 2),
       lty = c(1, 1, 1, 1))

points(result$soil_water_content_2, -result$soil_pressure_potential_2, col = "red", cex = 1)
points(result$soil_water_content_3, -result$soil_pressure_potential_3, col = "green", cex = 1)
points(result$soil_water_content_4, -result$soil_pressure_potential_4, col = "orange", cex = 1)


plot(-result$soil_pressure_potential_1, result$soil_water_content_1, col = "blue",
     #xlim = c(0, -0.1),
     #ylim = c(0, -0.1),
     log = "y",
     type = "p",
     xlab = "Pressure Potential (MPa)",
     ylab = "Volumetric Water Content (%)",
     main = "Soil Water Retention Curve",
     cex = 1)

legend("topright", 
       legend = c("Layer 1", "Layer 2", "Layer 3", "Layer 4"),
       col = c("blue", "red", "green", "orange"),
       lwd = c(2, 2, 2, 2),
       lty = c(1, 1, 1, 1))

points(-result$soil_pressure_potential_2, result$soil_water_content_2, col = "red", cex = 1)
points(-result$soil_pressure_potential_3, result$soil_water_content_3, col = "green", cex = 1)
points(-result$soil_pressure_potential_4, result$soil_water_content_4, col = "orange", cex = 1)

result$soil_water_content_1



plot(result$soil_water_content, result$soil_potential_avg, col = "blue",
     xlim = c(0,0.45),
     ylim = c(0, -0.1),
     type = "p",
     xlab = "Volumetric Soil Water Content (%)",
     ylab = "Total Potential (MPa)",
     main = "Soil Water Retention Curve",
     cex = 1)


wc_df <- data.frame(
  wc = result$soil_water_content_1,
  alpha = result$soil_alpha_1,
  n = result$soil_n_1,
  m = result$soil_m_1,
  theta_s = result$soil_saturated_wc_1,
  theta_r = result$soil_residual_wc_1,
  swp = result$soil_pressure_potential_1
)

wc_df
write.csv(wc_df, "C:\\Users\\natal\\OneDrive\\documents\\masters\\research\\literature\\soil_water_potential\\soil_water_potential_rosenbrock_results.csv")

evaluate_module('soil_potential.cpp',
                input_quantities = )


####################################################################################################################################################################
f_rwu = (result$soil_potential_avg - result$root_total_potential)/1
plot(result$time/24, f_rwu)

average_uptake = -(result$uptake_layer_1 + result$uptake_layer_2 + result$uptake_layer_3 +
                           result$uptake_layer_4 + result$uptake_layer_5 + result$uptake_layer_6)
points(result$time/24, average_uptake)
average_uptake
