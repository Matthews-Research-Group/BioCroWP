library(BioCro)
library(BioCroWP)
library(BioCroWater)
library(data.table)
library(dplyr)
library(lubridate)

year = 2005

# Using weather file with daily max/min air temps to validate soil temp module
load("C:/Users/natal/OneDrive/Documents/masters/research/data/soybean_weather_wp.rdata")
weatherData <- merged_soybean_weather$'2005'
#weatherData <- soybean_weather$'2002'

# listing direct modules
direct_modules_water_potential = list("BioCroWater:soil_type_selector", "BioCroWater:soil_surface_runoff",
                                      "BioCroWater:soil_water_downflow", "BioCroWater:soil_water_tiledrain", 
                                      "BioCroWater:soil_water_upflow","BioCroWater:soil_water_uptake",
                                      "BioCroWater:multilayer_soil_profile_avg",
                                      "BioCroWP:soil_temperature",
                                      "BioCroWP:soil_potential",
                                      #"BioCroWP:total_potential",
                                      "BioCroWP:osmotic_potential")
direct_modules_new = soybean$direct_modules
old_soil_evapo_index = which(direct_modules_new=="BioCro:soil_evaporation")
direct_modules_new = direct_modules_new[-old_soil_evapo_index] #remove soil evaporation
direct_modules = c(direct_modules_new[1:(old_soil_evapo_index-1)],direct_modules_water_potential,
                   direct_modules_new[old_soil_evapo_index:length(direct_modules_new)]) # insert BioCroWP

# listing differential modules
differential_modules_water_potential = list("BioCroWater:soil_evaporation",
                                            "BioCroWater:multi_layer_soil_profile"
                                            #"BioCroWP:pressure_potential"
                                            )
differential_modules_new = soybean$differential_modules
old_soil_profile_index = which(differential_modules_new=="BioCro:two_layer_soil_profile")
differential_modules_new = differential_modules_new[-old_soil_profile_index] #remove soil_profile
differential_modules = c(differential_modules_new[1:(old_soil_profile_index-1)],
                         differential_modules_water_potential,
                         differential_modules_new[old_soil_profile_index:length(differential_modules_new)])

# Set Initial Values
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
  root_water_content = 878.416
  stem_water_content = 6912.2555
  leaf_water_content = 2505.24243
  root_volume = 1
  stem_volume = 1
  leaf_volume = 1 # no leaves in initial development stages
  pods_volume = 1
  root_pressure_potential = -0.04  # Initial value to be updated
  stem_pressure_potential = -0.3
  leaf_pressure_potential = -0.5
  #root_total_potential = -0.05  # For initial water flow calculation
  #stem_total_potential = -0.4
  #leaf_total_potential = -0.6
  leaf_temperature = 298.15
  kd = 1 # Not sure what this should be
})
init_values$soil_water_content=NULL

# Set Parameters
soiltype = 4 
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
  mod_root_x = 57
  mod_root_z = 57
  mod_stem_x = 57
  mod_stem_z = 57
  mod_leaf_x = 9
  mod_leaf_y = 2
  mod_leaf_z = 9
  wp_crit = 0.4
  storage_water_frac = 0.8
  R_soil_root = 1
  R_root_stem = 1
  R_stem_leaf = 1
  minimum_temp_year = (28.916 - 32.)/1.8
  maximum_temp_year = (76.910 - 32.0)/1.8
  start_doy = weatherData$doy[1]
})
parameters[c('soil_field_capacity','soil_saturated_conductivity','soil_saturation_capacity','soil_wilting_point')]=NULL
parameters[c('kShell','net_assimilation_rate_shell')] = NULL

###############################################################################################################

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

###############################################################################################################
# Soil Temperature Plot
plot(result$time, result$soil_temperature_1, xlab="DOY", ylab="soil temperature (K)")
plot(result$time, weatherData$precip[0:length(result$time + 1)], xlab="DOY", ylab="Precip")

# Saving Soil Temp Data to CSV
soil_temp_df <- data.frame(
  # time
  time = result$time,
  
  # inputs
  min_temp_day = result$minimum_temp_day,
  max_temp_day = result$maximum_temp_day,
  
  # output
  soil_temp_L1 = result$soil_temperature_1,
  soil_temp_L2 = result$soil_temperature_2,
  soil_temp_L3 = result$soil_temperature_3,
  soil_temp_L4 = result$soil_temperature_4,
  soil_temp_L5 = result$soil_temperature_5,
  soil_temp_L6 = result$soil_temperature_6,
  soil_temp_avg = result$soil_temperature_avg
)

write.csv(soil_temp_df, "C:\\Users\\natal\\OneDrive\\documents\\masters\\research\\data\\soil_temp_results.csv")

################################################################################################################

# Calculating daily avg/min/max for each layer
# Putting results in dt
#sim_doy = floor(result$time/24)
sim_doy = result$doy
layer_dt <- data.table(
  time = sim_doy,
  L1 = result$soil_temperature_1,
  L2 = result$soil_temperature_2,
  L3 = result$soil_temperature_3,
  L4 = result$soil_temperature_4,
  L5 = result$soil_temperature_5,
  L6 = result$soil_temperature_6,
  layer_avg = result$soil_temperature_avg
)


# Computing daily averages
#L1_avg <- layer_dt %>% group_by(time) %>% summarize(daily_avg = mean(L1))
#L2_avg <- layer_dt %>% group_by(time) %>% summarize(daily_avg = mean(L2))
#L3_avg <- layer_dt %>% group_by(time) %>% summarize(daily_avg = mean(L3))
#L4_avg <- layer_dt %>% group_by(time) %>% summarize(daily_avg = mean(L4))
#L5_avg <- layer_dt %>% group_by(time) %>% summarize(daily_avg = mean(L5))
#L6_avg <- layer_dt %>% group_by(time) %>% summarize(daily_avg = mean(L6))
#profile_avg_daily <- layer_dt %>% group_by(time) %>% summarize(daily_avg = mean(layer_avg))

daily_avg_dt <- layer_dt[, lapply(.SD, mean, na.rm=TRUE), by = time,
                         .SDcols = patterns("^L|layer_avg")]

# Daily maximums
daily_max_dt <- layer_dt[, lapply(.SD, max, na.rm=TRUE), by = time,
                         .SDcols = patterns("^L|layer_avg")]

# Daily minimums
daily_min_dt <- layer_dt[, lapply(.SD, min, na.rm=TRUE), by = time,
                         .SDcols = patterns("^L|layer_avg")]

###############################################################################################################################
# Processesing observed data

# opening csv
warm_data <- fread('C:\\Users\\natal\\OneDrive\\Documents\\masters\\research\\data\\daily_data.csv')
warm_data$doy <- as.numeric(format(as.Date(with(warm_data, paste(year, month, day, sep = "-")), "%Y-%m-%d"), "%j"))

# selecting data from just the simulation period
warm_data_year = warm_data[year==2005]
warm_data_period <- warm_data_year[sim_doy[1]:sim_doy[length(sim_doy)]]

# Converting from Fahrenheit to Kelvin
# (degF - 32)/ 1.8 + 273.15

# make dts with 4in sod, 8 in sod avg/min/max
observed_avg <- data.table(
  time = warm_data_period$doy,
  four_in_sod = (as.numeric(warm_data_period$avg_soiltemp_4in_sod) - 32)/1.8 + 273.15,
  eight_in_sod = (as.numeric(warm_data_period$avg_soiltemp_8in_sod) - 32)/1.8 + 273.15
)

observed_min <- data.table(
  time = warm_data_period$doy,
  four_in_sod = (as.numeric(warm_data_period$min_soiltemp_4in_sod) - 32)/1.8 + 273.15,
  eight_in_sod = (as.numeric(warm_data_period$min_soiltemp_8in_sod) - 32)/1.8 + 273.15
)

observed_max <- data.table(
  time = warm_data_period$doy,
  four_in_sod = (as.numeric(warm_data_period$max_soiltemp_4in_sod) - 32)/1.8 + 273.15,
  eight_in_sod = (as.numeric(warm_data_period$max_soiltemp_8in_sod) - 32)/1.8 + 273.15
)

min_temp <- as.numeric(min(warm_data_year$min_air_temp))
max_temp <- as.numeric(max(warm_data_year$max_air_temp))

###############################################################################################################################

# Plotting observed vs simulated data and calculating R2

# L2 vs 4in sod

# avg
R2 <- 1 - (sum((observed_avg$four_in_sod - daily_avg_dt$L2)^2)/sum((observed_avg$four_in_sod - mean(observed_avg$four_in_sod))^2))
#R2 <- cor(observed_avg$four_in_sod, daily_avg_dt$L2, method = "pearson")^2
plot(observed_avg$four_in_sod, daily_avg_dt$L2, xlab='Observed 4in Sod (K)',
     ylab = 'Simulated Layer 2 (K)', main = 'Average 4in Sod vs Layer 2')
abline(a = 0, b = 1, col = "red", lty = 2)
legend("topleft", legend = paste("R² =", round(R2, 4)), bty = "n")

# min
R2 <- 1 - (sum((observed_min$four_in_sod - daily_min_dt$L2)^2)/sum((observed_min$four_in_sod - mean(observed_min$four_in_sod))^2))
#R2 <- cor(observed_min$four_in_sod, daily_min_dt$L2, method = "pearson")^2
plot(observed_min$four_in_sod, daily_min_dt$L2, xlab='Observed 4in Sod (K)',
     ylab = 'Simulated Layer 2 (K)', main = 'Minimum 4in Sod vs Layer 2')
abline(a = 0, b = 1, col = "red", lty = 2)
legend("topleft", legend = paste("R² =", round(R2, 4)), bty = "n")

ss_res <- sum((observed_avg$four_in_sod - daily_avg_dt$L2)^2)
ss_tot <- sum((observed_min$four_in_sod - mean(observed_min$four_in_sod))^2)

# max
R2 <- 1 - (sum((observed_max$four_in_sod - daily_max_dt$L2)^2)/sum((observed_max$four_in_sod - mean(observed_max$four_in_sod))^2))
#R2 <- cor(observed_max$four_in_sod, daily_max_dt$L2, method = "pearson")^2
plot(observed_max$four_in_sod, daily_max_dt$L2, xlab='Observed 4in Sod (K)',
     ylab = 'Simulated Layer 2 (K)', main = 'Maximum 4in Sod vs Layer 2')
abline(a = 0, b = 1, col = "red", lty = 2)
legend("topleft", legend = paste("R² =", round(R2, 4)), bty = "n")



# L3 vs 8in sod

# avg
R2 <- 1 - (sum((observed_avg$eight_in_sod - daily_avg_dt$L2)^2)/sum((observed_avg$eight_in_sod - mean(observed_avg$eight_in_sod))^2))
#R2 <- cor(observed_avg$eight_in_sod, daily_avg_dt$L3, method = "pearson")^2
plot(observed_avg$eight_in_sod, daily_avg_dt$L3, xlab='Observed 8in Sod (K)',
     ylab = 'Simulated Layer 3 (K)', main = 'Average 8in Sod vs Layer 3')
abline(a = 0, b = 1, col = "red", lty = 2)
legend("topleft", legend = paste("R² =", round(R2, 4)), bty = "n")

# min
R2 <- 1 - (sum((observed_min$eight_in_sod - daily_min_dt$L2)^2)/sum((observed_min$eight_in_sod - mean(observed_min$eight_in_sod))^2))
#R2 <- cor(observed_min$eight_in_sod, daily_min_dt$L3, method = "pearson")^2
plot(observed_min$eight_in_sod, daily_min_dt$L3, xlab='Observed 8in Sod (K)',
     ylab = 'Simulated Layer 3 (K)', main = 'Minimum 8in Sod vs Layer 3')
abline(a = 0, b = 1, col = "red", lty = 2)
legend("topleft", legend = paste("R² =", round(R2, 4)), bty = "n")

# max
R2 <- 1 - (sum((observed_max$eight_in_sod - daily_max_dt$L2)^2)/sum((observed_max$eight_in_sod - mean(observed_max$eight_in_sod))^2))
#R2 <- cor(observed_max$eight_in_sod, daily_max_dt$L3, method = "pearson")^2
plot(observed_max$eight_in_sod, daily_max_dt$L3, xlab='Observed 8in Sod (K)',
     ylab = 'Simulated Layer 3 (K)', main = 'Maximum 8in Sod vs Layer 3')
abline(a = 0, b = 1, col = "red", lty = 2)
legend("topleft", legend = paste("R² =", round(R2, 4)), bty = "n")


################################################################################################################################

plot(observed_avg$time, observed_avg$four_in_sod, col='blue')
points(observed_avg$time, daily_avg_dt$L2, col='red')

################################################################################################################################
# calculating RMSE

L2_avg_RMSE <- sqrt((1/length(observed_avg$four_in_sod))*sum((daily_avg_dt$L2 - observed_avg$four_in_sod)^2))
L2_min_RMSE <- sqrt((1/length(observed_min$four_in_sod))*sum((daily_min_dt$L2 - observed_min$four_in_sod)^2))
L2_max_RMSE <- sqrt((1/length(observed_max$four_in_sod))*sum((daily_max_dt$L2 - observed_max$four_in_sod)^2))

L3_avg_RMSE <- sqrt((1/length(observed_avg$eight_in_sod))*sum((daily_avg_dt$L3 - observed_avg$eight_in_sod)^2))
L3_min_RMSE <- sqrt((1/length(observed_min$eight_in_sod))*sum((daily_min_dt$L3 - observed_min$eight_in_sod)^2))
L3_max_RMSE <- sqrt((1/length(observed_max$eigth_in_sod))*sum((daily_max_dt$L3 - observed_max$eight_in_sod)^2))
################################################################################################################################
# plotting temperature in profile

plot(result$time/24, result$soil_temperature_1,
     type = 'l',
     lwd = '2',
     xlab = 'DOY',
     ylab = 'Soil Temperature (K)',
     main = 'Soil Temperature in each Soil Layer',
     col = 'red')

lines(result$time/24, result$soil_temperature_2, col = 'orange', lwd = '2')
lines(result$time/24, result$soil_temperature_3, col = 'green', lwd = '4')
lines(result$time/24, result$soil_temperature_4, col = 'blue', lwd = '2')

legend("bottomright", 
       legend = c("Layer 1", "Layer 2", "Layer 3", "Layer 4"),
       col = c("red", "orange", "green", "blue"),
       lwd = c(2, 2, 2, 2, 2),
       lty = c(1, 1, 1, 1, 1))

################################################################################################################################
# lag time demonstration

plot(result$time/24, result$soil_temperature_1,
     type = 'l',
     lwd = '2',
     xlim = c(151, 154),
     xlab = 'DOY',
     ylab = 'Soil Temperature (K)',
     main = 'Soil Temperature in each Soil Layer',
     col = 'red')

lines(result$time/24, result$soil_temperature_2, col = 'orange', lwd = '2')
lines(result$time/24, result$soil_temperature_3, col = 'green', lwd = '4')
lines(result$time/24, result$soil_temperature_4, col = 'blue', lwd = '2')

legend("bottomleft", 
       legend = c("Layer 1", "Layer 2", "Layer 3", "Layer 4"),
       col = c("red", "orange", "green", "blue"),
       lwd = c(2, 2, 2, 2, 2),
       lty = c(1, 1, 1, 1, 1))

i_max <- which.max(warm_data_year$avg_soiltemp_4in_sod)[1]
warm_data_year$day[i_max]

i_jan <- which(warm_data_year$month == 1)
i_jul <- which(warm_data_year$month == 8)

jan_avg_min = mean(as.numeric(warm_data_year$min_air_temp[i_jan[1]:i_jan[length(i_jan)]]))
jan_avg_max = mean(as.numeric(warm_data_year$max_air_temp[i_jan[1]:i_jan[length(i_jan)]]))
jul_avg_min = mean(as.numeric(warm_data_year$min_air_temp[i_jul[1]:i_jul[length(i_jul)]]))
jul_avg_max = mean(as.numeric(warm_data_year$max_air_temp[i_jul[1]:i_jul[length(i_jul)]]))

jan_avg <- (jan_avg_min + jan_avg_max)/2
jul_avg <- (jul_avg_min + jul_avg_max)/2

aug_avg <- mean(as.numeric(warm_data_year$avg_air_temp[i_jul[1]:i_jul[length(i_jul)]]))
