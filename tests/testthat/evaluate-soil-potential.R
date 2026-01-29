library(BioCro)
library(BioCroWP)
library(BioCroWater)

#load("C:/Users/natal/OneDrive/Documents/masters/research/data/soybean_weather_wp.rdata")
#weatherData <- merged_soybean_weather$'2002'
#simulated <- seq(from = 0.1, to = 0.44, length.out = 907)
#leftover <- rep(0.44, 3288 - 907)
#x <- c(simulated, leftover)
#weatherData$soil_water_content <- x

swc_list = seq(0.08, 0.47, length.out = 100)
swc_result = list()
swp_result = list()
for (swc in swc_list){
  inputs = list(max_rooting_layer = 4,
                soil_n_1 = 1.4239,
                soil_m_1 = 0.2977,
                soil_alpha_1 = 0.0069,
                soil_residual_wc_1 = 0.0892,
                soil_saturated_wc_1 = 0.463,
                soil_n_2 = 1.4239,
                soil_m_2 = 0.2977,
                soil_alpha_2 = 0.0069,
                soil_residual_wc_2 = 0.0892,
                soil_saturated_wc_2 = 0.463,
                soil_n_3 = 1.4239,
                soil_m_3 = 0.2977,
                soil_alpha_3 = 0.0069,
                soil_residual_wc_3 = 0.0892,
                soil_saturated_wc_3 = 0.463,
                soil_n_4 = 1.4239,
                soil_m_4 = 0.2977,
                soil_alpha_4 = 0.0069,
                soil_residual_wc_4 = 0.0892,
                soil_saturated_wc_4 = 0.463,
                soil_n_5 = 1.4239,
                soil_m_5 = 0.2977,
                soil_alpha_5 = 0.0069,
                soil_residual_wc_5 = 0.0892,
                soil_saturated_wc_5 = 0.463,
                soil_n_6 = 1.4239,
                soil_m_6 = 0.2977,
                soil_alpha_6 = 0.0069,
                soil_residual_wc_6 = 0.0892,
                soil_saturated_wc_6 = 0.463,
                soil_depth_1 = 5,
                soil_depth_2 = 10,
                soil_depth_3 = 20,
                soil_depth_4 = 20,
                soil_depth_5 = 20,
                soil_depth_6 = 25,
                soil_water_content_1 = swc,
                soil_water_content_2 = swc,
                soil_water_content_3 = swc,
                soil_water_content_4 = swc,
                soil_water_content_5 = swc,
                soil_water_content_6 = swc,
                soil_water_content = swc)


                result <- evaluate_module('BioCroWP:soil_potential',
                                input_quantities = inputs)
                
                swc_result <- c(swc_result, swc)
                swp_result <- c(swp_result, -1*(1/0.0000979)*result$soil_pressure_potential_1)
}

plot(swc_result, swp_result,
     #xlim = c(0, -2000),
     log = 'y',
     type = 'l',
     lwd = 4,
     xlab = 'Volumetric Soil Water Content (%)',
     ylab = '- Matric Potential (cm)',
     main = "Soil Moisture Retention Curve for Dry to Saturated Soil"
     #ylim = c(0.5, 0)
     )

#plot(swc_result, swp_result,
     #ylim = c(0,-60)
 #    )

##################################################################################################
# running just once
swc = 0.2
inputs = list(max_rooting_layer = 4,
              soil_n_1 = 1.4239,
              soil_m_1 = 0.2977,
              soil_alpha_1 = 0.0069,
              soil_residual_wc_1 = 0.0892,
              soil_saturated_wc_1 = 0.463,
              soil_n_2 = 1.4239,
              soil_m_2 = 0.2977,
              soil_alpha_2 = 0.0069,
              soil_residual_wc_2 = 0.0892,
              soil_saturated_wc_2 = 0.463,
              soil_n_3 = 1.4239,
              soil_m_3 = 0.2977,
              soil_alpha_3 = 0.0069,
              soil_residual_wc_3 = 0.0892,
              soil_saturated_wc_3 = 0.463,
              soil_n_4 = 1.4239,
              soil_m_4 = 0.2977,
              soil_alpha_4 = 0.0069,
              soil_residual_wc_4 = 0.0892,
              soil_saturated_wc_4 = 0.463,
              soil_n_5 = 1.4239,
              soil_m_5 = 0.2977,
              soil_alpha_5 = 0.0069,
              soil_residual_wc_5 = 0.0892,
              soil_saturated_wc_5 = 0.463,
              soil_n_6 = 1.4239,
              soil_m_6 = 0.2977,
              soil_alpha_6 = 0.0069,
              soil_residual_wc_6 = 0.0892,
              soil_saturated_wc_6 = 0.463,
              soil_depth_1 = 5,
              soil_depth_2 = 10,
              soil_depth_3 = 20,
              soil_depth_4 = 20,
              soil_depth_5 = 20,
              soil_depth_6 = 25,
              soil_water_content_1 = swc,
              soil_water_content_2 = swc,
              soil_water_content_3 = swc,
              soil_water_content_4 = swc,
              soil_water_content_5 = swc,
              soil_water_content_6 = swc,
              soil_water_content = swc)


result <- evaluate_module('BioCroWP:soil_potential',
                input_quantities = inputs)
