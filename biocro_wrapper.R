library(BioCro)
with_ephoto = (nchar(Sys.getenv("WITH_EPHOTO")) > 0)
if (with_ephoto) {
  library(yggdrasilBML)
}

# weather data includes 
#   fields: "year","doy","hour","temp","rh","windspeed","precip","solar","tz","jday","daylength"

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

  print(length(direct_modules))
  if (with_ephoto) {
    direct_modules[[15]] = "yggdrasilBML:ephotosynthesis"
  }
  print(length(direct_modules))

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