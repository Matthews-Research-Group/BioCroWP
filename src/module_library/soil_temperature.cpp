#include "soil_temperature.h"
#include <cmath>
#include <vector>

// sinusoidal function to estimate hourly soil temperature fluctuations
// Hillel, 1982; Marshall and Holmes, 1988; Wu and Nofziger, 1999

using BioCroWP::soil_temperature;

string_vector soil_temperature::get_inputs()
{
    return {
        "soil_depth_1", // cm
        "soil_depth_2",
        "soil_depth_3",
        "soil_depth_4",
        "soil_depth_5",
        "soil_depth_6",

        "soil_water_content_1", // volumetric swc
        "soil_water_content_2",
        "soil_water_content_3",
        "soil_water_content_4",
        "soil_water_content_5",
        "soil_water_content_6",

        "max_rooting_layer",

        "minimum_temp_day", // degrees C
        "maximum_temp_day",

        "soil_clay_content_1", // % volume
        "soil_clay_content_2",
        "soil_clay_content_3",
        "soil_clay_content_4",
        "soil_clay_content_5",
        "soil_clay_content_6",

        "soil_saturation_capacity_1",
        "soil_saturation_capacity_2",
        "soil_saturation_capacity_3",
        "soil_saturation_capacity_4",
        "soil_saturation_capacity_5",
        "soil_saturation_capacity_6",

        "soil_type_indicator_1",
        "soil_type_indicator_2",
        "soil_type_indicator_3",
        "soil_type_indicator_4",
        "soil_type_indicator_5",
        "soil_type_indicator_6",

        "hour",

    };
}

string_vector soil_temperature::get_outputs()
{
    return {
        "soil_temperature_1", // K
        "soil_temperature_2",
        "soil_temperature_3",
        "soil_temperature_4",
        "soil_temperature_5",
        "soil_temperature_6",
        "soil_temperature_avg"
    };
}

void soil_temperature::do_operation() const
{
    double pi = 3.14159265358979323846;

    double sd_arr[] = {
        soil_depth_1/100,
        soil_depth_2/100,
        soil_depth_3/100,
        soil_depth_4/100,
        soil_depth_5/100,
        soil_depth_6/100
    }; // m

    double swc_arr[] = {
        soil_water_content_1,
        soil_water_content_2,
        soil_water_content_3,
        soil_water_content_4,
        soil_water_content_5,
        soil_water_content_6
    };

    double soil_sat_capacity_arr[] = {
        soil_saturation_capacity_1,
        soil_saturation_capacity_2,
        soil_saturation_capacity_3,
        soil_saturation_capacity_4,
        soil_saturation_capacity_5,
        soil_saturation_capacity_6
    }; 

    double clay_content_arr[] = {
        soil_clay_content_1,
        soil_clay_content_2,
        soil_clay_content_3,
        soil_clay_content_4,
        soil_clay_content_5,
        soil_clay_content_6
    }; 

    double soil_type_indicator_arr[] = {
        soil_type_indicator_1,
        soil_type_indicator_2,
        soil_type_indicator_3,
        soil_type_indicator_4,
        soil_type_indicator_5,
        soil_type_indicator_6
    };

    double tot_soil_depth = 0.0;
    for(int l = 0; l < max_rooting_layer; l++){
        tot_soil_depth += sd_arr[l]; // m
    }

    double tot_soil_depth_arr[] = {
        sd_arr[0],
        sd_arr[0] + sd_arr[1],
        sd_arr[0] + sd_arr[1] + sd_arr[2],
        sd_arr[0] + sd_arr[1] + sd_arr[2] + sd_arr[3],
        sd_arr[0] + sd_arr[1] + sd_arr[2] + sd_arr[3] + sd_arr[4],
        sd_arr[0] + sd_arr[1] + sd_arr[2] + sd_arr[3] + sd_arr[4] + sd_arr[5]
    };

    // Degree of saturation calculated via definitions of porosity and volumetric swc
    std::vector<double> s_r_arr;
    for(int l = 0; l < max_rooting_layer; l++){
        s_r_arr.push_back(swc_arr[l]/soil_sat_capacity_arr[l]); // dimensionless
    }

    // fudge by 0.5 degrees celcius is recommended because air temp is used instead of soil surface temp (Moore et al. (2020))
    double min_K = minimum_temp_day + 273.15 + 0.5; // K
    double max_K = maximum_temp_day + 273.15 + 0.5;

    // Thermal conductivity of water
    double k_w = 51.41/24; // kJ m-1 hr-1 K-1

    // soil particle thermal conductivity from Gemant (1950)
    std::vector<double> k_s_arr;
    for(int l = 0; l < max_rooting_layer; l++){
        k_s_arr.push_back((504.58 - 2.85*clay_content_arr[l])/24); // kJ m-1 hr-1 K-1
    } 

    // thermal conductivity model is from Cote and Konrad (2005)
    // saturated soil thermal conductivity
    std::vector<double> k_sat_arr;
    for(int l = 0; l < max_rooting_layer; l++){
        k_sat_arr.push_back(pow(k_s_arr[l], (1 - soil_sat_capacity_arr[l]))*pow(k_w, soil_sat_capacity_arr[l])); // kJ m-1 d-1 K-1
    }
    
    // dry soil thermal conductivity
    // X = 0.75 (W/m*deg C) and n = 1.2 (dimensionless) for natural mineral soils
    // W/(m*deg C) = (J/(m*s*deg C))
    // 1 J/(m*s*deg C)(1 kJ/1000 J)(3600 s/1 hr)(1 deg C/ 1 deg K) = 3.6 kJ m-1 hr-1 K-1
    std::vector<double> k_dry_arr;
    for(int l = 0; l < max_rooting_layer; l++){
        k_dry_arr.push_back((0.75*3.6)*pow(10, (-1.2*soil_sat_capacity_arr[l]))); // kJ m-1 hr-1 K-1
    }

    // determining k parameter for each soil layer
    std::vector<double> k_arr; // empty k vector
    for (int l = 0; l < max_rooting_layer; l++){
        if (soil_type_indicator_arr[l] == 1 || soil_type_indicator_arr[l] == 2 || soil_type_indicator_arr[l] == 3) {
            k_arr.push_back(3.55); // medium or fine sand
        } else {
            k_arr.push_back(1.90); // silty or clayey soil
    }
    }
    
    // normalized thermal conductivity
    std::vector<double> k_r_arr;
    for (int l = 0; l < max_rooting_layer; l++){
       k_r_arr.push_back((k_arr[l]*s_r_arr[l])/(1 + (k_arr[l] - 1)*s_r_arr[l])); // Unitless
    } 

    std::vector<double> k_tot_arr;
    for (int l = 0; l < max_rooting_layer; l++){
       k_tot_arr.push_back((k_sat_arr[l] - k_dry_arr[l])*k_r_arr[l] + k_dry_arr[l]);  // kJ m-1 hr-1 K-1
    }

    // Hillel, D. 1982. Introduction to soil physics. Academic Press, San Diego, CA.
    std::vector<double> heat_cap_arr;
    for (int l = 0; l < max_rooting_layer; l++){
       heat_cap_arr.push_back(2000*(1 - soil_sat_capacity_arr[l]) + 4200*swc_arr[l]); // kJ m-3 K-1
    }

    // sinusoidal function for hourly soil temperature variation
    double w = (pi*2)/24; // frequency (1/hr)
    std::vector<double> d_arr;
    for (int l = 0; l < max_rooting_layer; l++){
       d_arr.push_back(pow(((2*k_tot_arr[l])/(heat_cap_arr[l]*w)), 0.5)); // m
    }

    // amplitude at a given depth (hr)
    std::vector<double> a_z_arr;
    for(int l = 0; l < max_rooting_layer; l++){
        a_z_arr.push_back(((max_K - min_K)/2)*exp(-tot_soil_depth_arr[l]/d_arr[l])); // K
    }

    // average daily soil surface temp
    double T_a = (min_K + max_K)/2; // K

    // lag time (hrs) calculation for each soil layer
    // the amount of time it takes for a temperature fluctuation to travel from the surface to a given depth
    // lag time = (24hrs/2*pi) x (z/d), from Chu
    std::vector<double> lag_time_arr;
    for(int l = 0; l < max_rooting_layer; l++){
        lag_time_arr.push_back((24/(2*pi))*(tot_soil_depth_arr[l]/d_arr[l])); // hr
    }

    // phase constant aligns the temperature minimum to the actual observed minimum
    std::vector<double> phase_arr;
    for(int l = 0; l < max_rooting_layer; l++){
        phase_arr.push_back((pi/2) + w*lag_time_arr[l]); // dimensionless
    }

    // soil temperature at each soil layer
    std::vector<double> soil_temperature_arr;
    double T_i0 = T_a; // air temperature or soil temperature at previous layer
    double T_i = 0; // soil temperature at current layer
    for(int l = 0; l < max_rooting_layer; l++){
        T_i = T_i0 + a_z_arr[l]*sin(w*hour - (tot_soil_depth_arr[l]/d_arr[l]) - phase_arr[l]); // soil temperature at layer i
        soil_temperature_arr.push_back(T_i);
        T_i0 = T_i; 
    }

    // averaging soil temp
    double temp_tot = 0.0;
    for(int l = 0; l < max_rooting_layer; l++){
        temp_tot += (soil_temperature_arr[l]*sd_arr[l]); // K*m
    }
    double temp_avg = temp_tot/tot_soil_depth; // K

    update(soil_temperature_1_op, soil_temperature_arr[0]);
    update(soil_temperature_2_op, soil_temperature_arr[1]);
    update(soil_temperature_3_op, soil_temperature_arr[2]);
    update(soil_temperature_4_op, soil_temperature_arr[3]);
    update(soil_temperature_5_op, soil_temperature_arr[4]);
    update(soil_temperature_6_op, soil_temperature_arr[5]);
    update(soil_temperature_avg_op, temp_avg);
}