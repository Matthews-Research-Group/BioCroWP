### This script optimizes parameters for a turgor driven water transport 
### model based on Coussement et al. (2018) and (2020)

library(BioCro)
library(BioCroWater)
library(BioCroWP)
library(DEoptim)

#############################################################################################################
### Loading observed data

year = 2005

load("C:/Users/natal/OneDrive/Documents/masters/research/data/soybean_weather_wp.rdata")
weatherData <- merged_soybean_weather[[as.character(year)]]

dryBiomass <- read.csv(paste0("C:/Users/natal/OneDrive/Documents/masters/research/data/optimization/dryBiomass_", year, ".csv"),
                       row.names = 1)

# Processing dry biomass data so it is comparable to wet biomass
obsRoot <- dryBiomass$root * 7.333e6
obsStem <- dryBiomass$stem * 7.333e6
obsLeaf <- dryBiomass$leaf * 7.333e6
obsPods <- dryBiomass$pods * 7.333e6

obsBiomass <- data.frame(
  DOY = dryBiomass$DOY,
  hour = dryBiomass$hour,
  Root = obsRoot,
  Stem = obsStem,
  Leaf = obsLeaf,
  Grain = obsPods
)

  
#############################################################################################################

# Specify direct modules from BioCroWater and BioCroWP
# Remove soil evaporation module from main BioCro
direct_modules_soil_water = list("BioCroWater:soil_type_selector", "BioCroWater:soil_surface_runoff",
                                 "BioCroWater:soil_water_downflow", "BioCroWater:soil_water_tiledrain", 
                                 "BioCroWater:soil_water_upflow","BioCroWater:soil_water_uptake",
                                 "BioCroWater:multilayer_soil_profile_avg",
                                 "BioCroWP:water_flow",
                                 "BioCroWP:soil_temperature",
                                 "BioCroWP:soil_potential",
                                 "BioCroWP:longitudinal_extensibility_update",
                                 "BioCroWP:osmotic_potential",
                                 "BioCroWP:total_potential",
                                 "BioCroWP:total_biomass")
direct_modules_new = soybean$direct_modules
old_soil_evapo_index = which(direct_modules_new=="BioCro:soil_evaporation")
direct_modules_new = direct_modules_new[-old_soil_evapo_index] #remove soil evaporation
direct_modules_new = c(direct_modules_new[1:(old_soil_evapo_index-1)],direct_modules_soil_water,
                       direct_modules_new[old_soil_evapo_index:length(direct_modules_new)])

# Repeating with differential modules
differential_modules_soil_water = list("BioCroWater:soil_evaporation","BioCroWater:multi_layer_soil_profile",
                                       "BioCroWP:pressure_potential_senescence"
)
differential_modules_new = soybean$differential_modules
old_soil_profile_index = which(differential_modules_new=="BioCro:two_layer_soil_profile")
differential_modules_new = differential_modules_new[-old_soil_profile_index] #remove soil_profile
differential_modules_new = c(differential_modules_new[1:(old_soil_profile_index-1)],
                             differential_modules_soil_water,
                             differential_modules_new[old_soil_profile_index:length(differential_modules_new)])

init_values =   within(soybean$initial_values,{
  soil_water_content_1 = soybean$initial_values$soil_water_content  #0-5cm
  soil_water_content_2 = soybean$initial_values$soil_water_content  #5-15cm
  soil_water_content_3 = soybean$initial_values$soil_water_content  #15-35cm
  soil_water_content_4 = soybean$initial_values$soil_water_content  #35-55 cm
  soil_water_content_5 = soybean$initial_values$soil_water_content  #55-75 cm
  soil_water_content_6 = soybean$initial_values$soil_water_content  #75-100cm
  sumes1              = 0.125
  sumes2               = 0
  time_factor = 0
  soil_evaporation_rate = 0
  pods_water_content = 0
  root_water_content = 0 #878.416
  stem_water_content = 0 #6912.2555
  leaf_water_content = 0 #2505.24243
  pods_volume = 1 #0.000001
  root_volume = 1 #0.00001
  stem_volume = 1 #0.00005
  leaf_volume = 1 #0.00005 # no leaves in initial development stages
  root_pressure_potential = 0.4
  stem_pressure_potential = 0.2
  leaf_pressure_potential = 0.1
  pods_pressure_potential = 0.1
  kd = 1
})
init_values$soil_water_content=NULL

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
  ext_root_x = 0.075#0.55 #0.055, constant
  ext_root_z_initial = 0.4#1.6 #0.275
  ext_stem_x = 0.075 #0.75 #0.055, constant
  ext_stem_z_initial = 0.4 #2.0 #0.275
  ext_leaf_y_initial = 0.55#0.75 #0.55
  ext_leaf_z_initial = 0.55#0.75 #0.55
  ext_pods_x_initial = 0.2 #1.5 #0.2
  mod_root_x = 36 #57
  mod_root_z = 36#57
  mod_stem_x = 36#15 #57
  mod_stem_z = 36#15 #57
  mod_leaf_x = 2 #9
  mod_leaf_y = 0.5#0.75 #2
  mod_leaf_z = 2#2.25 #9
  mod_pods_x = 2 #2.25 #9
  wp_crit = 0.4
  storage_water_frac = 0.8
  R_soil_root = 0.16 #0.0216217167
  R_root_stem = 0.15  #0.154440833
  R_stem_leaf = 0.1 #0.0926645
  R_stem_pods = 0.1 #0.0926645
  #soil_temperature_avg = 298.15
  t_root_m = 0.0
  t_root_e = 1.0
  t_stem_m = 1.0
  t_stem_e = 2.0
  t_leaf_m = 1.0
  t_leaf_e = 2.0
  t_pods_m = 1.75
  t_pods_e = 2.5
})
parameters[c('soil_field_capacity','soil_saturated_conductivity','soil_saturation_capacity','soil_wilting_point')]=NULL
parameters[c('kShell','net_assimilation_rate_shell')] = NULL



# Parameter optimization via DEoptim

# Objective Function
objective_function <- function(optimParams, init_values, parameters, weatherData,
                               direct_modules_new, differential_modules_new, 
                               obsBiomass) {
  
  parameters$ext_root_x <- optimParams[1]
  parameters$ext_root_z_initial <- optimParams[2]
  parameters$ext_stem_x <- optimParams[3]
  parameters$ext_stem_z_initial <- optimParams[4]
  parameters$ext_leaf_y_initial <- optimParams[5]
  parameters$ext_leaf_z_initial <- optimParams[6]
  parameters$ext_pods_x_initial <- optimParams[7]
  parameters$mod_root_x <- optimParams[8]
  parameters$mod_root_z <- optimParams[9]
  parameters$mod_stem_x <- optimParams[10]
  parameters$mod_stem_z <- optimParams[11]
  parameters$mod_leaf_y <- optimParams[12]
  parameters$mod_leaf_z <- optimParams[13]
  parameters$mod_pods_x <- optimParams[14]
  parameters$wp_crit <- optimParams[15]
  parameters$R_soil_root <- optimParams[16]
  parameters$R_root_stem <- optimParams[17]
  parameters$R_stem_leaf <- optimParams[18]
  parameters$R_stem_pods <- optimParams[19]
  

  
  # Generate simulated data
  simBiomass <- run_biocro(
    init_values,
    parameters,
    weatherData,
    direct_modules_new,
    differential_modules_new,
    ode_solver=list(type ="boost_rosenbrock",
                    output_step_size = 1.0,
                    adaptive_rel_error_tol = 1e-2, #1e-4 try increasing or decreasing
                    adaptive_abs_error_tol = 1e-2, #1e-4
                    adaptive_max_steps = 50)
  )
  
  # Assigning a large error if simulation does not complete
  if (nrow(simBiomass) != nrow(obsBiomass)) {
    return(1e9)
  }
  
  # Assigning a large error if any infinite or nan values are produced 
  if (any(is.na(simBiomass)) || any(is.infinite(as.matrix(simBiomass)))) {
    return(1e9)
  }
  
  error_root <- sqrt((1/nrow(simBiomass))*sum((simBiomass$Root - obsBiomass$Root)^2))
  error_stem <- sqrt((1/nrow(simBiomass))*sum((simBiomass$Stem - obsBiomass$Stem)^2))
  error_leaf <- sqrt((1/nrow(simBiomass))*sum((simBiomass$Leaf - obsBiomass$Leaf)^2))
  error_pods <- sqrt((1/nrow(simBiomass))*sum((simBiomass$Grain - obsBiomass$Grain)^2))
  
  # Normalizing error by the mean of the observed data so errors are a percentage
  # Prevents optimizer from prioritizing errors of organs with larger biomass values
  rel_error_root <- error_root / mean(obsBiomass$Root)
  rel_error_stem <- error_stem / mean(obsBiomass$Stem)
  rel_error_leaf <- error_leaf / mean(obsBiomass$Leaf)
  rel_error_pods <- error_pods / mean(obsBiomass$Grain)
  error <- rel_error_root + rel_error_stem + rel_error_leaf + rel_error_pods
  
  return(error)
}

# Specifying bounds
# Extensibility, elastic modulus, critical turgor pressure, and resistance
lower_bounds <- c(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0.1,0,0,0,0)
upper_bounds <- c(10,10,10,10,10,10,10,100,100,100,100,100,100,100,0.9,10,10,10,10)


# Run differential evolution
set.seed(229) # For reproducibility

calibration_result <- DEoptim(
  fn = objective_function,
  lower = lower_bounds,
  upper = upper_bounds,
  init_values = init_values,
  parameters = parameters,
  weatherData = weatherData,
  direct_modules_new = direct_modules_new,
  differential_modules_new = differential_modules_new,
  obsBiomass = obsBiomass,
  control = DEoptim.control(
    itermax = 140,      # Maximum number of generations
    NP = 100,            # Population size (rule of thumb: 10x the number of parameters)
    CR = 0.9,           # Crossover probability (0 to 1)
    F = 0.8,            # Differential weighting factor (0 to 2)
    trace = 10          # Print progress to the console every 10 iterations
  )
)


############################################################################################################################################

# Extract best parameters
best_params <- calibration_result$optim$bestmem

parameters$ext_root_x <- best_params[1]
parameters$ext_root_z_initial <- best_params[2]
parameters$ext_stem_x <- best_params[3]
parameters$ext_stem_z_initial <- best_params[4]
parameters$ext_leaf_y_initial <- best_params[5]
parameters$ext_leaf_z_initial <- best_params[6]
parameters$ext_pods_x_initial <- best_params[7]
parameters$mod_root_x <- best_params[8]
parameters$mod_root_z <- best_params[9]
parameters$mod_stem_x <- best_params[10]
parameters$mod_stem_z <- best_params[11]
parameters$mod_leaf_y <- best_params[12]
parameters$mod_leaf_z <- best_params[13]
parameters$mod_pods_x <- best_params[14]
parameters$wp_crit <- best_params[15]
parameters$R_soil_root <- best_params[16]
parameters$R_root_stem <- best_params[17]
parameters$R_stem_leaf <- best_params[18]
parameters$R_stem_pods <- best_params[19]

# Running the model with best parameters
result <- run_biocro(
  init_values, parameters, weatherData, direct_modules_new, 
  differential_modules_new,
  ode_solver=list(type ="boost_rosenbrock", output_step_size = 1.0,
                  adaptive_rel_error_tol = 1e-2, adaptive_abs_error_tol = 1e-2, 
                  adaptive_max_steps = 50)
)

# Calculate and print the final raw RMSE for each organ
final_err_root <- sqrt((1/nrow(result)) * sum((result$Root - obsBiomass$Root)^2))
final_err_stem <- sqrt((1/nrow(result)) * sum((result$Stem - obsBiomass$Stem)^2))
final_err_leaf <- sqrt((1/nrow(result)) * sum((result$Leaf - obsBiomass$Leaf)^2))
final_err_pods <- sqrt((1/nrow(result)) * sum((result$Grain - obsBiomass$Grain)^2))

cat("\n--- FINAL CALIBRATION RESULTS ---\n")
cat("Root RMSE:", final_err_root, "g\n")
cat("Stem RMSE:", final_err_stem, "g\n")
cat("Leaf RMSE:", final_err_leaf, "g\n")
cat("Pods RMSE:", final_err_pods, "g\n")

############################################################################################################################################
# pressure potential check
par(mfrow = c(1, 1))

plot(result$time/24, result$root_pressure_potential, col='red',
     ylim=c(-1.0,1.0),
     main='Model Crash Debug- Pressure Potential',
     xlab = 'DOY',
     ylab= 'Pressure Potential (MPa)')
points(result$time/24, result$stem_pressure_potential, col='orange')
points(result$time/24, result$leaf_pressure_potential, col='green')
points(result$time/24, result$pods_pressure_potential, col='black')

legend("bottomleft", 
       legend = c("Root", "Stem", "Leaf", "Pod"),
       col = c("red", "orange", "green", "black"),
       lwd = c(2, 2, 2, 2),
       lty = c(1, 1, 1, 1))

# organ flow check
plot(result$time/24, result$F_rwu, col='red',
     ylim=c(-1000000,3000000),
     #ylim = c(0,20000),
     main='Model Crash Debug- Organ Flows',
     xlab = 'DOY',
     ylab= 'Water Flow (g ha-1 hr-1)')
points(result$time/24, result$F_root_stem, col='orange')
points(result$time/24, result$F_stem_leaf, col='green')
points(result$time/24, result$F_stem_pods, col='black')

legend("bottomleft", 
       legend = c("F_rwu", "F_rs", "F_sl", "F_sp"),
       col = c("red", "orange", "green", "black"),
       lwd = c(2, 2, 2, 2, 2),
       lty = c(1, 1, 1, 1, 1))

# water content check
par(mfrow = c(2, 2))

plot(result$time/24, result$root_water_content, col='blue',
     main="Root",
     xlab='DOY',
     ylab='Root Water Content (g)')
plot(result$time/24, result$stem_water_content, col='blue',
     main="Stem",
     xlab='DOY',
     ylab='Stem Water Content (g)')
plot(result$time/24, result$leaf_water_content, col='blue',
     main="Leaf",
     xlab='DOY',
     ylab='Leaf Water Content (g)')
plot(result$time/24, result$pods_water_content, col='blue',
     main="Pod",
     xlab='DOY',
     ylab='Pod Water Content (g)')

par(mfrow = c(1, 1))

# total potential check
plot(result$time/24, result$root_total_potential, col='red',
     ylim=c(-1.2,0),
     main='Model Crash Debug- Total Potential',
     xlab = 'DOY',
     ylab= 'Total Potential (MPa)')
points(result$time/24, result$stem_total_potential, col='orange')
points(result$time/24, result$leaf_total_potential, col='green')
points(result$time/24, result$pods_total_potential, col='black')

legend("topright", 
       legend = c("Root", "Stem", "Leaf", "Pod"),
       col = c("red", "orange", "green", "black"),
       lwd = c(2, 2, 2, 2),
       lty = c(1, 1, 1, 1))

length(result$time)

############################################################################################################################################
# Saving best parameters to CSV

bestParams_df = data.frame(
  ext_root_x <- best_params[1],
  ext_root_z_initial <- best_params[2],
  ext_stem_x <- best_params[3],
  ext_stem_z_initial <- best_params[4],
  ext_leaf_y_initial <- best_params[5],
  ext_leaf_z_initial <- best_params[6],
  ext_pods_x_initial <- best_params[7],
  mod_root_x <- best_params[8],
  mod_root_z <- best_params[9],
  mod_stem_x <- best_params[10],
  mod_stem_z <- best_params[11],
  mod_leaf_y <- best_params[12],
  mod_leaf_z <- best_params[13],
  mod_pods_x <- best_params[14],
  wp_crit <- best_params[15],
  R_soil_root <- best_params[16],
  R_root_stem <- best_params[17],
  R_stem_leaf <- best_params[18],
  R_stem_pods <- best_params[19]
)

write.csv(bestParams_df, "C:\\Users\\natal\\OneDrive\\Documents\\masters\\research\\thesis\\best_parameters.csv")

# Saving errors to csv

errors_df = data.frame(
  root_RMSE = final_err_root,
  stem_RMSE = final_err_stem,
  leaf_RMSE = final_err_leaf,
  pods_RMSE = final_err_pods
)

write.csv(errors_df, "C:\\Users\\natal\\OneDrive\\Documents\\masters\\research\\thesis\\error.csv")
