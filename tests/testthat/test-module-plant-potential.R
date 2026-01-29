library(BioCro)
library(BioCroWP)
library(BioCroWater)

load("C:/Users/natal/OneDrive/Documents/masters/research/data/soybean_weather_wp.rdata")
weatherData <- merged_soybean_weather$'2005'


direct_modules_soil_water = list("BioCroWater:soil_type_selector", "BioCroWater:soil_surface_runoff",
                                 "BioCroWater:soil_water_downflow", "BioCroWater:soil_water_tiledrain", 
                                 "BioCroWater:soil_water_upflow","BioCroWater:soil_water_uptake",
                                 "BioCroWater:multilayer_soil_profile_avg",
                                 "BioCroWP:soil_temperature",
                                 "BioCroWP:soil_potential",
                                 "BioCroWP:osmotic_potential",
                                 "BioCroWP:total_potential",
                                 "BioCroWP:total_biomass")
direct_modules_new = soybean$direct_modules
old_soil_evapo_index = which(direct_modules_new=="BioCro:soil_evaporation")
direct_modules_new = direct_modules_new[-old_soil_evapo_index] #remove soil evaporation
direct_modules_new = c(direct_modules_new[1:(old_soil_evapo_index-1)],direct_modules_soil_water,
                       direct_modules_new[old_soil_evapo_index:length(direct_modules_new)]) # insert BioCroWater

differential_modules_soil_water = list("BioCroWater:soil_evaporation","BioCroWater:multi_layer_soil_profile",
                                       "BioCroWP:pressure_potential")
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
  pods_volume = 0.0001 #0.000001
  root_volume = 0.02 #0.00001
  stem_volume = 0.02 #0.00005
  leaf_volume = 0.01 #0.00005 # no leaves in initial development stages
  pods_pressure_potential = -0.5
  root_pressure_potential = -0.1  # Initial value to be updated
  stem_pressure_potential = -0.3
  leaf_pressure_potential = -0.5
  pods_osmotic_potential = -0.1
  #root_total_potential = -0.05  # For initial water flow calculation
  #stem_total_potential = -0.4
  #leaf_total_potential = -0.6
  leaf_temperature = 298.15
  #soil_temperature_avg = 298.15
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
  ext_root_x = 0.2 #0.055
  ext_root_z = 0.8 #0.275
  ext_stem_x = 0.2 #0.055
  ext_stem_z = 0.8 #0.275
  ext_leaf_x = 0.0
  ext_leaf_y = 0.2 #0.55
  ext_leaf_z = 0.2 #0.55
  ext_pods_x = 0.8 #0.2
  mod_root_x = 28.5 #57
  mod_root_z = 28.5 #57
  mod_stem_x = 28.5 #57
  mod_stem_z = 28.5 #57
  mod_leaf_x = 4.5 #9
  mod_leaf_y = 1 #2
  mod_leaf_z = 4.5 #9
  mod_pods_x = 4.5 #9
  wp_crit = 0.4
  storage_water_frac = 0.8
  R_soil_root = 1
  R_root_stem = 2
  R_stem_leaf = 10
  R_stem_pods = 10
  #soil_temperature_avg = 298.15
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


###############################################################################################################################
plot(result$time/24, result$root_total_potential,
     type='p', col = 'red',
     #xlim = c(result$doy[0], result$doy[length(result$doy)]),
     ylim = c(-3,1),
     xlab = 'DOY',
     ylab = 'Potential (MPa)',
     main = 'Water Potential over Growing Season')

points(result$time/24, result$soil_potential_avg, col = 'blue')
points(result$time/24, result$stem_total_potential, col = 'orange')
points(result$time/24, result$leaf_total_potential, col = 'green')
points(result$time/24, result$pods_total_potential, col = 'black')

legend("bottomright", 
       legend = c("Soil", "Root", "Stem", "Leaf", "Pod"),
       col = c("blue", "red", "orange", "green", "black"),
       lwd = c(2, 2, 2, 2, 2),
       lty = c(1, 1, 1, 1, 1))

# now plotting individual to better see variation over season
plot(result$time/24, result$soil_potential_avg,
     type='p', col = 'blue',
     xlab = 'DOY',
     ylab = 'Potential (MPa)',
     main = 'Soil Water Potential over Growing Season')

plot(result$time/24, result$root_total_potential,
     type='p', col = 'red',
     xlab = 'DOY',
     ylab = 'Potential (MPa)',
     main = 'Root Water Potential over Growing Season')

plot(result$time/24, result$stem_total_potential,
     type='p', col = 'orange',
     xlab = 'DOY',
     ylab = 'Potential (MPa)',
     main = 'Stem Water Potential over Growing Season')

plot(result$time/24, result$leaf_total_potential,
     type='p', col = 'green',
     xlab = 'DOY',
     ylab = 'Potential (MPa)',
     main = 'Leaf Water Potential over Growing Season')

plot(result$time/24, result$pods_total_potential,
     type='p', col = 'black',
     xlab = 'DOY',
     ylab = 'Potential (MPa)',
     main = 'Pod Water Potential over Growing Season')

############################################################################################################
# organ volume and wc (split)

plot(result$time, result$root_water_content, col='blue',
     main="Root Water Content and Volume over Growing Season")
points(result$time, result$root_volume, col='green')

plot(result$time, result$stem_water_content, col='blue',
     main="stem Water Content and Volume over Growing Season")
points(result$time, result$stem_volume, col='green')

plot(result$time, result$leaf_water_content, col='blue',
     main="leaf Water Content and Volume over Growing Season")
points(result$time, result$leaf_volume, col='green')

plot(result$time, result$pods_water_content, col='blue',
     main="pods Water Content and Volume over Growing Season")
points(result$time, result$pods_volume, col='green')

plot(result$time, result$root_volume)
plot(result$time, result$stem_volume)
plot(result$time, result$leaf_volume)
plot(result$time, result$pods_volume)

############################################################################################################

# splitting into osmotic and pressure components

plot(result$time/24, result$root_pressure_potential,
     type='p', col = 'red',
     #ylim=c(-1,1),
     xlab = 'DOY',
     ylab = 'Potential (MPa)',
     main = 'Root Pressure Potential over Growing Season')
points(result$time/24, result$root_osmotic_potential, col='blue')

plot(result$time/24, result$stem_pressure_potential,
     type='p', col = 'orange',
     #ylim=c(-1,1),
     xlab = 'DOY',
     ylab = 'Potential (MPa)',
     main = 'Stem Pressure Potential over Growing Season')
points(result$time/24, result$root_osmotic_potential, col='blue')

plot(result$time/24, result$leaf_pressure_potential,
     type='p', col = 'green',
     #ylim=c(-1,1),
     xlab = 'DOY',
     ylab = 'Potential (MPa)',
     main = 'Leaf Pressure Potential over Growing Season')
points(result$time/24, result$leaf_osmotic_potential, col='blue')

plot(result$time/24, result$pods_pressure_potential,
     type='p', col = 'black',
     #ylim=c(-1,1),
     xlab = 'DOY',
     ylab = 'Potential (MPa)',
     main = 'Pod Pressure Potential over Growing Season')

plot(result$time, result$soil_potential_avg,
     ylim=c(-1,1),
     main="Total Soil Potential and Pressure Potential in Organs",
     col='blue')
points(result$time, result$root_pressure_potential, col='red')
points(result$time, result$stem_pressure_potential, col='orange')
points(result$time, result$leaf_pressure_potential, col='green')
points(result$time, result$pods_pressure_potential, col='black')

legend("bottomright", 
       legend = c("Soil", "Root", "Stem", "Leaf", "Pod"),
       col = c("blue", "red", "orange", "green", "black"),
       lwd = c(2, 2, 2, 2, 2),
       lty = c(1, 1, 1, 1, 1))

############################################################################################################

plot(result$time, result$root_osmotic_potential)
plot(result$time, result$soil_temperature_avg)
plot(result$time, result$stem_osmotic_potential)
plot(result$time, result$leaf_osmotic_potential)
plot(result$time, result$pods_osmotic_potential)

############################################################################################################

plot(result$time, result$soil_temperature_avg)

sunlit_avg = (result$sunlit_leaf_temperature_layer_0 +
                result$sunlit_leaf_temperature_layer_1 +
                result$sunlit_leaf_temperature_layer_2 +
                result$sunlit_leaf_temperature_layer_3 +
                result$sunlit_leaf_temperature_layer_4 +
                result$sunlit_leaf_temperature_layer_5 +
                result$sunlit_leaf_temperature_layer_6 +
                result$sunlit_leaf_temperature_layer_7 +
                result$sunlit_leaf_temperature_layer_8 +
                result$sunlit_leaf_temperature_layer_9)/10

shaded_avg = (result$shaded_leaf_temperature_layer_0 +
                result$shaded_leaf_temperature_layer_1 +
                result$shaded_leaf_temperature_layer_2 +
                result$shaded_leaf_temperature_layer_3 +
                result$shaded_leaf_temperature_layer_4 +
                result$shaded_leaf_temperature_layer_5 +
                result$shaded_leaf_temperature_layer_6 +
                result$shaded_leaf_temperature_layer_7 +
                result$shaded_leaf_temperature_layer_8 +
                result$shaded_leaf_temperature_layer_9)/10

leaf_temperature_plot = 273.15 + (sunlit_avg + shaded_avg)/2

plot(result$time, leaf_temperature_plot, col='green')
points(result$time, result$soil_temperature_avg, col='red')
