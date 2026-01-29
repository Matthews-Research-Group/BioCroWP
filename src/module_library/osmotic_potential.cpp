#include "osmotic_potential.h"

using BioCroWP::osmotic_potential;

string_vector osmotic_potential::get_inputs()
{
    return {
        "sunlit_leaf_temperature_layer_0",
        "sunlit_leaf_temperature_layer_1",
        "sunlit_leaf_temperature_layer_2",
        "sunlit_leaf_temperature_layer_3",
        "sunlit_leaf_temperature_layer_4",
        "sunlit_leaf_temperature_layer_5",
        "sunlit_leaf_temperature_layer_6",
        "sunlit_leaf_temperature_layer_7",
        "sunlit_leaf_temperature_layer_8",
        "sunlit_leaf_temperature_layer_9",

        "shaded_leaf_temperature_layer_0",
        "shaded_leaf_temperature_layer_1",
        "shaded_leaf_temperature_layer_2",
        "shaded_leaf_temperature_layer_3",
        "shaded_leaf_temperature_layer_4",
        "shaded_leaf_temperature_layer_5",
        "shaded_leaf_temperature_layer_6",
        "shaded_leaf_temperature_layer_7",
        "shaded_leaf_temperature_layer_8",
        "shaded_leaf_temperature_layer_9",


        // "leaf_temperature",               // K
        "soil_temperature_avg",           // K

        "root_volume",                    // m3
        "stem_volume", 
        "leaf_volume",
        
        "storage_water_frac",             // dimensionless
    };
}

string_vector osmotic_potential::get_outputs()
{
    return {
        "root_osmotic_potential",         // MPa
        "stem_osmotic_potential",
        "leaf_osmotic_potential"
    };
}

void osmotic_potential::do_operation() const
{
    // calculating average leaf temperature
    double sunlit_avg = (sunlit_leaf_temperature_layer_0 + sunlit_leaf_temperature_layer_1 +
            sunlit_leaf_temperature_layer_2 + sunlit_leaf_temperature_layer_3 + sunlit_leaf_temperature_layer_4 +
            sunlit_leaf_temperature_layer_5 + sunlit_leaf_temperature_layer_6 + sunlit_leaf_temperature_layer_7 +
            sunlit_leaf_temperature_layer_8 + sunlit_leaf_temperature_layer_9)/10;

    double shaded_avg = (shaded_leaf_temperature_layer_0 + shaded_leaf_temperature_layer_1 +
            shaded_leaf_temperature_layer_2 + shaded_leaf_temperature_layer_3 + shaded_leaf_temperature_layer_4 +
            shaded_leaf_temperature_layer_5 + shaded_leaf_temperature_layer_6 + shaded_leaf_temperature_layer_7 +
            shaded_leaf_temperature_layer_8 + shaded_leaf_temperature_layer_9)/10;

    double leaf_temperature = (sunlit_avg + shaded_avg)/2;

    //double storage_water_frac = 0.8;
    double M_sucrose = 342.3; // molar mass of sucrose (g/mol)
    double R = 8.31446261815324; // J*k-1*mol-1

    double m_sucrose_root = 0.1 * root_volume; // Update later to use partioning fraction from Ximin
    double m_sucrose_stem = 0.1 * stem_volume; // Total sugar mass in plant organ (g)
    double m_sucrose_leaf = 0.1 * leaf_volume; // Current eqn is from Coussement et al. (2018)

    double root_storage_water = storage_water_frac * root_volume; // m3
    double stem_storage_water = storage_water_frac * stem_volume;
    double leaf_storage_water = storage_water_frac * leaf_volume;

    double root_osmotic_potential_new = (-R*soil_temperature_avg*m_sucrose_root)/(root_storage_water*M_sucrose); // Dividing by 1000 so values are reported in MPa
    double stem_osmotic_potential_new = (-R*leaf_temperature*m_sucrose_stem)/(stem_storage_water*M_sucrose);
    double leaf_osmotic_potential_new = (-R*leaf_temperature*m_sucrose_leaf)/(leaf_storage_water*M_sucrose);

    update(root_osmotic_potential_op, root_osmotic_potential_new);
    update(stem_osmotic_potential_op, stem_osmotic_potential_new);
    update(leaf_osmotic_potential_op, leaf_osmotic_potential_new);
}