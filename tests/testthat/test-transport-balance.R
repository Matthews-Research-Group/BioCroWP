library(BioCro)
library(BioCroWP)
library(BioCroWater)

#load("/Users/natalie/Documents/data/soybean_weather_wp.rdata")
load("C:/Users/natal/OneDrive/Documents/masters/research/data/soybean_weather_wp.rdata")
weatherData <- merged_soybean_weather$'2005'

#weatherData_top <- head(weatherData,600)
#weatherData_slice <- weatherData[601:2600,]
#weatherData_bottom <- weatherData[2601:2952,]


direct_modules_soil_water = list("BioCroWater:soil_type_selector", "BioCroWater:soil_surface_runoff",
                                 "BioCroWater:soil_water_downflow", "BioCroWater:soil_water_tiledrain", 
                                 "BioCroWater:soil_water_upflow","BioCroWater:soil_water_uptake",
                                 "BioCroWater:multilayer_soil_profile_avg",
                                 #"BioCroWP:flow_resistance_calculator",
                                 #"BioCroWP:water_flow",
                                 "BioCroWP:soil_temperature",
                                 "BioCroWP:soil_potential",
                                 "BioCroWP:longitudinal_extensibility_update",
                                 "BioCroWP:osmotic_potential",
                                 #"BioCroWP:total_potential",
                                 "BioCroWP:calculate_flux"
                                 #"BioCroWP:total_biomass"
)
direct_modules_new = soybean$direct_modules
old_soil_evapo_index = which(direct_modules_new=="BioCro:soil_evaporation")
direct_modules_new = direct_modules_new[-old_soil_evapo_index] #remove soil evaporation
direct_modules_new = c(direct_modules_new[1:(old_soil_evapo_index-1)],direct_modules_soil_water,
                       direct_modules_new[old_soil_evapo_index:length(direct_modules_new)]) # insert BioCroWater

differential_modules_soil_water = list("BioCroWater:soil_evaporation","BioCroWater:multi_layer_soil_profile",
                                       #"BioCroWP:pressure_potential_senescence"
                                       "BioCroWP:transport_balance")
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
  root_volume_plastic = 1
  stem_volume_plastic = 1
  leaf_volume_plastic = 1
  pods_volume_plastic = 1
  root_pressure_potential = 0.3
  stem_pressure_potential = 0.2
  leaf_pressure_potential = 0.1
  pods_pressure_potential = 0.1
  root_total_potential = -0.1
  stem_total_potential = -0.4
  leaf_total_potential = -0.6
  pods_total_potential = -0.6
  leaf_temperature = 298.15
  kd = 1
  #F_rwu = 15000000
  #F_root_stem = 10000000
  #F_stem_leaf = 5000000
  #F_stem_pods = 5000000
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
  ext_root_x = 0.55#0.55 #0.055, constant
  ext_root_z_initial = 0.4#1.6 #0.275
  ext_stem_x = 0.1 #0.75 #0.055, constant
  ext_stem_z_initial = 0.3 #2.0 #0.275
  ext_leaf_x = 0   #constant
  ext_leaf_y_initial = 0.8#0.75 #0.55
  ext_leaf_z_initial = 0.8#0.75 #0.55
  ext_pods_x_initial = 0.6 #1.5 #0.2
  mod_root_x = 15 #57
  mod_root_z = 15#57
  mod_stem_x = 15#15 #57
  mod_stem_z = 15#15 #57
  mod_leaf_x = 3 #9
  mod_leaf_y = 1 #0.75 #2
  mod_leaf_z = 3 #2.25 #9
  mod_pods_x = 5 #2.25 #9
  wp_crit = 0.45
  storage_water_frac = 0.8
  #R_soil_root_base = 0.2 #0.0216217167
  #R_root_stem_base = 0.02  #0.154440833
  #R_stem_leaf_base = 0.1 #0.0926645
  #R_stem_pods_base = 0.25 #0.0926645
  R_soil_root = 0.15
  R_root_stem = 0.07
  R_stem_leaf = 0.03
  R_stem_pods = 0.04
  #n_resistance = 0.25
  #soil_temperature_avg = 298.15
  t_root_m = 0.0
  t_root_e = 1.0
  t_stem_m = 1.0
  t_stem_e = 2.0
  t_leaf_m = 1.0
  t_leaf_e = 2.0
  t_pods_m = 1.75
  t_pods_e = 2.5
  minimum_temp_year = (28.916 - 32.)/1.8
  maximum_temp_year = (76.910 - 32.0)/1.8
  start_doy = weatherData$doy[1]
  n_plants = 370658  # plants per hectare: (150000 plants/acre)*(1 acre/4046.86 m2)*(10000 m2/ha)
  f_leaf = 0.956522  # fraction of transpiration through leaves (Sinha et al. 2023)
  f_pods = 0.043478  # fraction of transpiration through pods (Sinha et al. 2023)
})
parameters[c('soil_field_capacity','soil_saturated_conductivity','soil_saturation_capacity','soil_wilting_point')]=NULL
parameters[c('kShell','net_assimilation_rate_shell')] = NULL

sim_start <- Sys.time()
#result <- run_biocro(
#  init_values,
#  parameters,
#  weatherData,
#  direct_modules_new,
#  differential_modules_new
#)
result <- run_biocro(
  init_values,
  parameters,
  weatherData,
  direct_modules_new,
  differential_modules_new,
  ode_solver=list(type ="boost_rosenbrock",
                  output_step_size = 1.0,
                  adaptive_rel_error_tol = 1e-2, #1e-4 try increasing or decreasing
                  adaptive_abs_error_tol = 1e-2, #1e-4
                  adaptive_max_steps = 10000)
)
sim_end <- Sys.time()
sim_time = sim_end - sim_start
cat('Simulation Time:', sim_time, 'min')



############################################################################################################
# Debug block

# pressure potential check
par(mfrow = c(1, 1))

plot(result$time/24, result$root_pressure_potential, col='red',
     ylim=c(-1.0,1.0),
     main='Pressure Potential',
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
canopy_transpiration_g = result$canopy_transpiration_rate*10^6

plot(result$time/24, result$F_rwu, col='red',
     ylim=c(-1000000,60000000),
     #ylim = c(0,20000),
     main='Organ Flows',
     xlab = 'DOY',
     ylab= 'Water Flow (g ha-1 hr-1)')
points(result$time/24, result$F_root_stem, col='orange')
points(result$time/24, result$F_stem_leaf, col='green')
points(result$time/24, result$F_stem_pods, col='black')
points(result$time/24, canopy_transpiration_g, col = 'pink')

legend("topleft", 
       legend = c("F_rwu", "F_rs", "F_sl", "F_sp", 'transpiration'),
       col = c("red", "orange", "green", "black", 'pink'),
       lwd = c(2, 2, 2, 2, 2,2),
       lty = c(1, 1, 1, 1, 1,1))

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
     main='Total Potential',
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

par(mfrow = c(1, 1))

# resistance
#plot(result$time/24, result$R_soil_root, col='red',
#     ylim = c(0,200),
#     main='Resistance',
#     xlab = 'DOY',
#     ylab= 'Resistance')
#points(result$time/24, result$R_root_stem, col='orange')
#points(result$time/24, result$R_stem_leaf, col='green')
#points(result$time/24, result$R_stem_pods, col='black')

legend("topright", 
       legend = c("Root", "Stem", "Leaf", "Pod"),
       col = c("red", "orange", "green", "black"),
       lwd = c(2, 2, 2, 2),
       lty = c(1, 1, 1, 1))

par(mfrow = c(1, 1))

length(result$time)

plot(result$fractional_doy, result$root_total_potential)
points(result$fractional_doy, result$root_pressure_potential + result$root_osmotic_potential)
