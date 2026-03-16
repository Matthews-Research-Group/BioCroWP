### This script optimizes parameters for a turgor driven water transport 
### model based on Coussement et al. (2018) and (2020)

library(BioCro)
library(BioCroWater)
library(BioCroWP)
library(DEoptim)
library(dplyr)

#############################################################################################################
### Loading observed data

year = 2009

load("C:/Users/natal/OneDrive/Documents/masters/research/data/soybean_weather_wp.rdata")
weatherData <- merged_soybean_weather[[as.character(year)]]

obsPotential_ds <- read.csv(paste0("C:/Users/natal/OneDrive/Documents/masters/research/data/optimization/Table_S12_water_potential/dri", year, "WPringmeans.csv"))

# Processing potential measurements from Gray et al. (2016)
# Measurements were taken at midday between 11:30 and 5:30

# Sort by DOY, subplot, and CO2 level
# Averaging potential measurements for each CO2 + drought treatment across rings and block?
obsPotential <- obsPotential_ds %>%
  # forcing columns to be numeric
  mutate(
    WPcorrect = as.numeric(WPcorrect),
    turgorcorrect = as.numeric(turgorcorrect),
    OPcorrect = as.numeric(OPcorrect)
  ) %>%
  # grouping by treatment and averaging
  group_by(DOY, subplot, CO2) %>%
  summarize(
    WP_mean = mean(WPcorrect, na.rm = TRUE),
    TP_mean = mean(turgorcorrect, na.rm = TRUE),
    OP_mean = mean(OPcorrect, na.rm = TRUE)
  )
  
# Extracting values for comparison 
# Comparing simulated values with the ambient and no drought condition
i_197_obs = which(obsPotential$DOY == 197 & obsPotential$subplot == "control" & obsPotential$CO2 == "ambient")[1]
TP_197_obs = obsPotential$TP_mean[i_197_obs]
OP_197_obs = obsPotential$OP_mean[i_197_obs]

i_210_obs = which(obsPotential$DOY == 210 & obsPotential$subplot == "control" & obsPotential$CO2 == "ambient")[1]
TP_210_obs = obsPotential$TP_mean[i_210_obs]
OP_210_obs = obsPotential$OP_mean[i_210_obs]

i_224_obs = which(obsPotential$DOY == 224 & obsPotential$subplot == "control" & obsPotential$CO2 == "ambient")[1]
TP_224_obs = obsPotential$TP_mean[i_224_obs]
OP_224_obs = obsPotential$OP_mean[i_224_obs]

i_238_obs = which(obsPotential$DOY == 238 & obsPotential$subplot == "control" & obsPotential$CO2 == "ambient")[1]
TP_238_obs = obsPotential$TP_mean[i_238_obs]
OP_238_obs = obsPotential$OP_mean[i_238_obs]

i_252_obs = which(obsPotential$DOY == 252 & obsPotential$subplot == "control" & obsPotential$CO2 == "ambient")[1]
TP_252_obs = obsPotential$TP_mean[i_252_obs]
OP_252_obs = obsPotential$OP_mean[i_252_obs]

TP_obs = c(TP_197_obs, TP_210_obs, TP_224_obs, TP_238_obs, TP_252_obs)
OP_obs = c(OP_197_obs, OP_210_obs, OP_224_obs, OP_238_obs, OP_252_obs)

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
  root_pressure_potential = 0.3
  stem_pressure_potential = 0.05
  leaf_pressure_potential = -0.1
  pods_pressure_potential = -0.1
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
  
  # Extracting data points on DOY 197, 210, 224, and 252 to compare
  # simulated and observed total, tugor, and osmotic potential
  # using simulated values at 3 pm
  i_197_sim = which(simPotential$DOY == 197 & simPotential$hour == 15)[1]
  TP_197_sim = simPotential$leaf_pressure_potential[i_197_sim]
  OP_197_sim = simPotential$leaf_osmotic_potential[i_197_sim]
  
  i_210_sim = which(simPotential$DOY == 210 & simPotential$hour == 15)[1]
  TP_210_sim = simPotential$leaf_pressure_potential[i_210_sim]
  OP_210_sim = simPotential$leaf_osmotic_potential[i_210_sim]
  
  i_224_sim = which(simPotential$DOY == 224 & simPotential$hour == 15)[1]
  TP_224_sim = simPotential$leaf_pressure_potential[i_224_sim]
  OP_224_sim = simPotential$leaf_osmotic_potential[i_224_sim]
  
  i_238_sim = which(simPotential$DOY == 238 & simPotential$hour == 15)[1]
  TP_238_sim = simPotential$leaf_pressure_potential[i_238_sim]
  OP_238_sim = simPotential$leaf_osmotic_potential[i_238_sim]
  
  i_252_sim = which(simPotential$DOY == 252 & simPotential$hour == 15)[1]
  TP_252_sim = simPotential$leaf_pressure_potential[i_252_sim]
  OP_252_sim = simPotential$leaf_osmotic_potential[i_252_sim]
  
  TP_sim = c(TP_197_sim, TP_210_sim, TP_224_sim, TP_238_sim, TP_252_sim)
  OP_sim = c(OP_197_sim, OP_210_sim, OP_224_sim, OP_238_sim, OP_252_sim)
  
  error_TP <- sqrt((1/5)*sum((TP_sim - TP_obs)^2))
  error_OP <- sqrt((1/5)*sum((OP_sim - OP_obs)^2))
  
  # Normalizing error by the mean of the observed data so errors are a percentage
  # Prevents optimizer from prioritizing errors of organs with larger biomass values
  rel_error_TP <- error_TP / mean(TP_obs)
  rel_error_OP <- error_OP / mean(OP_obs)
  error <- rel_error_TP + rel_error_OP
  
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
    itermax = 200,      # Maximum number of generations
    NP = 20,            # Population size (rule of thumb: 10x the number of parameters)
    CR = 0.9,           # Crossover probability (0 to 1)
    F = 0.8,            # Differential weighting factor (0 to 2)
    trace = 10          # Print progress to the console every 10 iterations
  )
)
