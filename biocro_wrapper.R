library(BioCro)
with_ephoto = Sys.getenv("WITH_EPHOTO")
if (with_ephoto == "YGG") {
  library(yggdrasilBML)
} else if (with_ephoto == "C") {
  library(BMLephoto)
}

update_param <- function(dst, src, fields) {
  src_select <- extract_param(src, fields)
  keys <- unique(c(names(dst), names(src_select)))
  out <- setNames(mapply(c, dst[keys], src_select[keys]), keys)
  return(out)
}

extract_param <- function(param, fields) {
  out = param[grep(
    paste(fields, collapse = "|"),
    names(param),
    ignore.case = TRUE
  )]
  return(out)
}

get_with_default = function (list, item, default = NULL) {
  value = list[[item]]
  if (is.null(value)) default else value
}

BioCroWrapper <- function(param, init_param = NULL, weather_param = NULL) {

  #########################################
  # Defaults from the built in crop model
  #########################################
  init_param = NULL
  weather_param = NULL
  weather = NULL
  # weather[10,]
  #      year doy hour  zen solar netsolar dw_solar up_solar temp    rh windspeed
  # 3634 2000 152    9 35.4  1390      500      667      147 27.1 0.656       5.8
  #           precip day_length time_zone_offset
  # 3634      0       14.4               -6
  crop = get_with_default(param, 'crop')  # , 'soybean')
  year = get_with_default(param, 'year')  # , 2002)
  output_timesteps = get_with_default(param, 'output_timesteps', FALSE)
  if (crop == 'soybean') {
    initial_state <- soybean$initial_values
    parameters <- soybean$parameters
    weather = get(toString(year), soybean_weather)
    direct_modules <- soybean$direct_modules
    differential_modules <- soybean$differential_modules
    ode_solver <- soybean$ode_solver
  } else {
    stop('Invalid crop name \"' + name + '\"')
  }
  
  #########################################
  # Update from input param
  #########################################
  parameters$enzyme_sf = get_with_default(param, 'enzyme_sf', 1.0)
  parameters$water_stress_approach = get_with_default(
    param, 'water_stress_approach', 1
  )
  parameters$Catm = get_with_default(param, 'Catm', 400)
  if (is.null(init_param)) {
    init_param = get_with_default(param, 'init')
  }
  if (is.null(weather_param)) {
    weather_param = get_with_default(param, 'weather')
  }
  if (is.null(weather_param)) {
    weather_param = extract_param(
      param,
      c("zen","netsolar","dw_solar","up_solar",
        "temp","rh","windspeed","precip","day_length",
        "time_zone_offset")
    )
    if (length(weather_param) == 0) {
      weather_param = NULL
    }
  }
  if (!is.null(weather_param)) {
    weather_param_time = extract_param(param, c("year","doy","hour"))
    weather_param = update_param(
      weather_param, param,
      c("year","doy","hour")
    )
  }
  #these dates were provided in MLM's paper
  #For other years, I simply used same dates took from this paper:https://doi.org/10.1093/jxb/erw435
  dates <- data.frame("year" = 2001:2006,
                      "sow"     = c(143, 152, 147, 149, 148, 148), 
                      "harvest" = c(291, 288, 289, 289, 270, 270))
  if(year<=2006){
    sowdate = get_with_default(
      param, 'doy_sow', dates$sow[which(dates$year == year)])
    harvestdate = get_with_default(
      param, 'doy_harvest', dates$harvest[which(dates$year == year)])
  } else {
    sowdate = get_with_default(param, 'doy_sow', 158)
    harvestdate = get_with_default(param, 'doy_harvest', 275)
  }
  if ("doy" %in% names(param)) {
    if (is.null(init_param)) {
      start_day = sowdate
    } else {
      start_day = min(param[['doy']])
    }
    end_day = max(param[['doy']])
  } else {
    start_day = get_with_default(param, 'doy_start', sowdate)
    end_day = get_with_default(param, 'doy_end', harvestdate - 1)
  }
  if ("hour" %in% names(param)) {
    start_hour = min(param[['hour']])
    end_hour = max(param[['hour']])
  } else {
    start_hour = get_with_default(param, 'hour_start', 0)
    end_hour = get_with_default(param, 'hour_end', 24)
  }
  restart = (start_day > sowdate)
  run_days  = end_day - start_day + 1
  run_hours = ((run_days - 1) * 24) + (end_hour - start_hour)
  #we need to know how many days finished to index the weather data correctly
  finished_days = start_day - sowdate 
  if (finished_days < 0) stop("start day is before sow date")
  finished_hours = finished_days * 24 + start_hour
  beg_ind = finished_hours + 1  #begin of index for weather
  #add an extra hour for the next init state 
  #this is because in BioCro, the output of hour x has the label at hour x+1! 
  end_ind = min(
    finished_hours + run_hours + 1,
    dim(weather)[1]
  ) #end of index for weather
  
  init_vars = names(initial_state)
  if (length(init_param) > 0) {
    # TODO: Test this
    init_param_sub = extract_param(init_param, init_vars)
    x_sub = data.frame(init_param_sub)
    last_values = x_sub[dim(x_sub)[1],]
    initial_state = initial_state
    for (ii in 1:length(initial_state)) {
      initial_state[[ii]] = as.numeric(last_values[ii])
    }
  } else {
    if (restart) {
      stop('Initial conditions required to start after the sow date')
    }
  }
  if (length(weather_param) > 0) {
    # TODO: Test this
    # TODO: Add missing fields?
    weather = data.frame(weather_param)
  } else {
    if (is.null(weather)) {
      if(year<=2006){
        weather_path = paste0("weather_data/",year,"_Bondville_IL_daylength.csv")
      } else {
        weather_path = "weather_data/NASA_data/BioCroInputs/site_1_2010_2022.csv"
      }
      weather <- read.csv(weather_path_old)
    }
    if(year<=2006) {
      sd.ind <- which(weather$doy == sowdate)[1]
      hd.ind <- which(weather$doy == harvestdate)[24]
    } else {
      colnames(weather)[colnames(weather)=="daylength"] = "day_length"
      weather = weather_all[weather_all$year==as.numeric(year),]
      sd.ind <- which(weather$doy == sowdate)[1]
      hd.ind <- which(weather$doy == harvestdate)[24]
    }
    weather <- weather[sd.ind:hd.ind,]  #growing season
  }
  #needed by module BioCro:solar_position_michalsky
  if(!"time_zone_offset" %in% colnames(weather))
  {
    print('no time zone offset exists in weather data. Adding one...')
    weather$time_zone_offset = get_with_default(
      param, 'time_zone_offset', -6
    )
  }
  print(paste("sow, harvest, start and end days are",
        sowdate, harvestdate, start_day, end_day))

  # subset of the weather for the run period ONLY!
  weather_sub <- weather[beg_ind:end_ind,]  

  if (with_ephoto == "YGG") {
    # Replace BioCro ten layer canopy modules
    direct_modules[[10]] = "yggdrasilBML:ten_layer_canopy_properties"
    direct_modules[[11]] = "yggdrasilBML:ten_layer_c3_canopy"
    direct_modules[[12]] = "yggdrasilBML:ten_layer_canopy_integrator"
  } else if (with_ephoto == "C") {
    # Replace BioCro ten layer canopy modules
    direct_modules[[10]] = "BMLephoto:ten_layer_canopy_properties"
    direct_modules[[11]] = "BMLephoto:ten_layer_c3_canopy"
    direct_modules[[12]] = "BMLephoto:ten_layer_canopy_integrator"
  }

  #loop through the number of run days
  restart = FALSE
  result = NULL
  for (i in 1:run_days) {
    initial_state_last = initial_state
    if (restart) {
      x_sub = result[,init_vars]
      last_values = x_sub[dim(x_sub)[1],]
      for (ii in 1:length(initial_state)) {
        initial_state_last[[ii]] = as.numeric(last_values[ii])
      }
    }
    b0 = (i-1)*24+1
    if (i == 1) {
      b0 = b0 + start_hour
    }
    b1 = (i-1)*24+1  #add an extra hour for this day, which will be the init state for the next day
    if (i == run_days) {
      b1 = b1 + end_hour
    } else {
      b1 = b1 + 24
    }
    b1 = min(b1,dim(weather_sub)[1])  #make sure index not out of range! 
    weather_dayi = weather_sub[b0:b1,]
    new_result <- run_biocro(
      initial_state_last,
      parameters,
      weather,
      direct_modules,
      differential_modules,
      ode_solver
    )
    if (output_timesteps) {
      if (is.null(result)) {
        result = new_result
      } else {
        rbind(result, new_result)
      }
    } else {
      result = new_result
    }
    restart = TRUE
  }

  if (output_timesteps) {
    return(result)
  } else {
    return(result[dim(result)[1],])
  }
}
