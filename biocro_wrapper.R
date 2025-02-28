library(BioCro)
with_ephoto = Sys.getenv("WITH_EPHOTO")
if (with_ephoto == "YGG") {
  library(yggdrasilBML)
} elseif (with_ephoto == "C") {
  library(BMLephoto)
}

BioCroWrapper <- function(param) {
  crop = 'soybean'
  year = '2002'
  param_names = names(param)
  if ("crop" %in% param_names) {
    crop = param[['crop']]
  }
  if ("year" %in% param_names) {
    year = param[['year']]
  }
  enzyme_sf = 1.0
  water_stress_approach = 1
  
  if (crop == 'soybean') {
    initial_values <- soybean$initial_values
    parameters <- soybean$parameters
    weather = get(toString(year), soybean_weather)
    direct_modules <- soybean$direct_modules
    differential_modules <- soybean$differential_modules
    ode_solver <- soybean$ode_solver
  } else {
    stop('Invalid crop name \"' + name + '\"')
  }
  parameters$enzyme_sf = enzyme_sf
  parameters$water_stress_approach = water_stress_approach

  if (with_ephoto == "YGG") {
    # Replace BioCro ten layer canopy modules
    direct_modules[[10]] = "yggdrasilBML:ten_layer_canopy_properties"
    direct_modules[[11]] = "yggdrasilBML:ten_layer_c3_canopy"
    direct_modules[[12]] = "yggdrasilBML:ten_layer_canopy_integrator"
  } elseif (with_ephoto == "C") {
    # Replace BioCro ten layer canopy modules
    direct_modules[[10]] = "BMLephoto:ten_layer_canopy_properties"
    direct_modules[[11]] = "BMLephoto:ten_layer_c3_canopy"
    direct_modules[[12]] = "BMLephoto:ten_layer_canopy_integrator"
  }
  weather = weather[10,]

  result <- run_biocro(
    initial_values,
    parameters,
    weather,
    direct_modules,
    differential_modules,
    ode_solver
  )

  return(result)
  
}