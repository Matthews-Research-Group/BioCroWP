library(BioCro)
library(BioCroWater)
library(BioCroWP)

load("C:/Users/natal/OneDrive/Documents/masters/research/data/soybean_weather_wp.rdata")
weatherData <- merged_soybean_weather$'2006'

direct_modules_soil_water = list("BioCroWater:soil_type_selector", "BioCroWater:soil_surface_runoff",
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
direct_modules_new = c(direct_modules_new[1:(old_soil_evapo_index-1)],direct_modules_soil_water,
                       direct_modules_new[old_soil_evapo_index:length(direct_modules_new)]) # insert BioCroWater

differential_modules_soil_water = list("BioCroWater:soil_evaporation","BioCroWater:multi_layer_soil_profile")
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
  #pods_osmotic_potential = -0.1
  #root_total_potential = -0.05  # For initial water flow calculation
  #stem_total_potential = -0.4
  #leaf_total_potential = -0.6
  leaf_temperature = 298.15
  #soil_temperature_avg = 298.15
  kd = 1
  #F_rwu = 50
  F_root_stem = 25
  #F_stem_leaf = 7
  #F_stem_pods = 7
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
  soil_temperature_avg = 298.15
})
parameters[c('soil_field_capacity','soil_saturated_conductivity','soil_saturation_capacity','soil_wilting_point')]=NULL
parameters[c('kShell','net_assimilation_rate_shell')] = NULL

result <- run_biocro(
  init_values,
  parameters,
  weatherData,
  direct_modules_new,
  differential_modules_new
)

plot(result$time, result$soil_water_content, xlab='time',ylab='soil water content')
plot(result$soil_water_content_1, result$soil_pressure_potential_1)
plot(result$soil_water_content_1, result$soil_potential_1)

##################################################################################################################

plot(result$time/24, result$soil_gravitational_potential_1, col='green',
     main='total, pressure, and gravitational potential - layer 1',
     ylim=c(-0.15,0))
points(result$time, result$soil_pressure_potential_1, col='blue')
points(result$time, result$soil_potential_1, col='red')

plot(result$time, result$soil_gravitational_potential_2, col='green',
     main='total, pressure, and gravitational potential - layer 2',
     ylim=c(-0.15,0))
points(result$time, result$soil_pressure_potential_2, col='blue')
points(result$time, result$soil_potential_2, col='red')

plot(result$time, result$soil_gravitational_potential_3, col='green',
     main='total, pressure, and gravitational potential - layer 3',
     ylim=c(-0.15,0))
points(result$time, result$soil_pressure_potential_3, col='blue')
points(result$time, result$soil_potential_3, col='red')

plot(result$time, result$soil_gravitational_potential_4, col='green',
     main='total, pressure, and gravitational potential - layer 4',
     ylim=c(-0.15,0))
points(result$time, result$soil_pressure_potential_4, col='blue')
points(result$time, result$soil_potential_4, col='red')

plot(result$time, result$soil_gravitational_potential_5, col='green',
     main='total, pressure, and gravitational potential - layer 5',
     ylim=c(-0.15,0))
points(result$time, result$soil_pressure_potential_5, col='blue')
points(result$time, result$soil_potential_5, col='red')

plot(result$time, result$soil_gravitational_potential_6, col='green',
     main='total, pressure, and gravitational potential - layer 6',
     ylim=c(-0.15,0))
points(result$time, result$soil_pressure_potential_6, col='blue')
points(result$time, result$soil_potential_6, col='red')

##################################################################################################################

plot(result$time, result$soil_pressure_potential_1,
     ylim=c(-0.15, 0.49), col='red',
     main = "SWP (red) and SWC (blue) over season - layer 1")
points(result$time, result$soil_water_content_1, col="blue")

plot(result$time, result$soil_pressure_potential_2,
     ylim=c(-0.15, 0.49), col='red',
     main = "SWP (red) and SWC (blue) over season - layer 2")
points(result$time, result$soil_water_content_2, col="blue")

plot(result$time, result$soil_pressure_potential_3,
     ylim=c(-0.15, 0.49), col='red',
     main = "SWP (red) and SWC (blue) over season - layer 3")
points(result$time, result$soil_water_content_3, col="blue")

plot(result$time, result$soil_pressure_potential_4,
     ylim=c(-0.15, 0.49), col='red',
     main = "SWP (red) and SWC (blue) over season - layer 4")
points(result$time, result$soil_water_content_4, col="blue")

plot(result$time, result$soil_pressure_potential_5,
     ylim=c(-0.15, 0.49), col='red',
     main = "SWP (red) and SWC (blue) over season - layer 5")
points(result$time, result$soil_water_content_5, col="blue")

plot(result$time, result$soil_pressure_potential_6,
     ylim=c(-0.15, 0.49), col='red',
     main = "SWP (red) and SWC (blue) over season - layer 6")
points(result$time, result$soil_water_content_6, col="blue")

##################################################################################################################

plot(result$time, result$soil_gravitational_potential_1,
     main='gravitational potential in each layer over season',
     ylim=c(-0.01, 0))
points(result$time, result$soil_gravitational_potential_2, col='blue')
points(result$time, result$soil_gravitational_potential_3, col='green')
points(result$time, result$soil_gravitational_potential_4, col='red')
#points(result$time, result$soil_gravitational_potential_5, col='blue')
#points(result$time, result$soil_gravitational_potential_6, col='blue')

##################################################################################################################

plot(result$time, result$soil_pressure_potential_1,
     main='pressure potential in each layer over season',
     ylim=c(-0.20, 0))
points(result$time, result$soil_pressure_potential_2, col='blue')
points(result$time, result$soil_pressure_potential_3, col='green')
points(result$time, result$soil_pressure_potential_4, col='red')

##################################################################################################################

plot(result$time, result$soil_pressure_potential_avg)
plot(result$time, result$soil_potential_avg)

plot(result$time, result$canopy_transpiration_rate)
plot(result$time[0:600], result$canopy_transpiration_rate[0:600])
plot(result$time[601:2600], result$canopy_transpiration_rate[601:2600])
plot(result$time[2601:3288], result$canopy_transpiration_rate[2601:3288])

#############################################################################################################
### extracting simulated dry biomass for optimization script

biomassDF_2006 <- data.frame(
  DOY = result$doy,
  hour = result$hour,
  root = result$Root,
  stem = result$Stem,
  leaf = result$Leaf,
  pods = result$Grain
)

write.csv(biomassDF_2006, "C:\\Users\\natal\\OneDrive\\Documents\\masters\\research\\data\\optimization\\dryBiomass_2006.csv")

