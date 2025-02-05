#ifndef C3PHOTO_H
#define C3PHOTO_H

#include <map>
#include <string>
struct c3_str {
    double Assim;
    double Gs;
    double Ci;
    double GrossAssim;
    double iterTimes;
    double penalty;
};

#ifdef WITH_YGGDRASIL

// Forward declaration
class YggRpcClient;

struct c3_str c3photoC(
    YggRpcClient& rpc,
    double const Qp,
    double const Tleaf,
    double const RH,
    double const Vcmax0,
    double const Jmax0,
    double const TPU_rate_max,
    double const Rd0,
    double const bb0,
    double const bb1,
    double const Gs_min,
    double Ca,
    double const AP,
    double const O2,
    double const thet,
    double const StomWS,
    int const water_stress_approach,
    double const electrons_per_carboxylation,
    double const electrons_per_oxygenation,
    double const enzyme_sf);

#endif // WITH_YGGDRASIL

struct c3_str c3photoC_FvCB(
    double const Qp,
    double const Tleaf,
    double const RH,
    double const Vcmax0,
    double const Jmax0,
    double const TPU_rate_max,
    double const Rd0,
    double const bb0,
    double const bb1,
    double const Gs_min,
    double Ca,
    double const AP,
    double const O2,
    double const thet,
    double const StomWS,
    int const water_stress_approach,
    double const electrons_per_carboxylation,
    double const electrons_per_oxygenation);

double solc(double LeafT);
double solo(double LeafT);

#endif
