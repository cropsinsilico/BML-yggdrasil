#ifndef yggdrasilBML_YGGDRASIL_MODULES_H
#define yggdrasilBML_YGGDRASIL_MODULES_H

#include "../framework/module.h"
#include "../framework/state_map.h"

#ifdef WITH_YGGDRASIL

#include "YggInterface.hpp" // for yggdrasil connection

namespace yggdrasilBML
{

/**
 * @class ygg_direct_module
 *
 * @brief A base class for steady state external models connected via
 *   yggdrasil (Lang 2019)
 *
 * The inputs and outputs of the module must be set by the derived class.
 *
 *  References:
 *  Lang, M. M. 2019
 *
 */
template<typename MODULE>
class ygg_direct_module : public direct_module
{
   public:
    ygg_direct_module(
        state_map const& input_quantities,
        state_map* output_quantities)
        : direct_module() {
        for ( auto& it : MODULE::get_inputs() )
            inputs.insert(
                std::make_pair(it, get_input(input_quantities, it)));
        for ( auto& it : MODULE::get_outputs() )
            outputs.insert(
                std::make_pair(it, get_op(output_quantities, it)));
        const std::string& module_name = MODULE::get_name();
        ygg_init();
#ifdef _OPENMP
#pragma omp critical (model_name)
        {
#endif
            server_name = module_name + "_" + std::getenv("YGG_MODEL_NAME");
#ifdef _OPENMP
        }
#endif
    }
    static double get_doc_member(const rapidjson::Document& state,
                                 const std::string& name) {
        if (!state.IsObject()) {
            std::string msg("State is not an object");
            ygglog_error(msg.c_str());
            throw std::logic_error(msg);
        }
        if (!(state.HasMember(name) && state[name].IsDouble())) {
            std::string msg("Failed to extract \"" + name + "\" from state");
            ygglog_error(msg.c_str());
            throw std::logic_error(msg);
        }
        return state[name].GetDouble();
    }
    static void call_model(YggRpcClient& rpc,
                           rapidjson::Document& state) {
        int ret = 0;
    
        // call server funcition (ephotosynthesis)
        ret = rpc.sendVar(state);
        if (ret < 0) {
            std::string msg("Error in RPC send.\n");
            ygglog_error(msg.c_str());
            throw std::logic_error(msg);
        }
        state.SetNull();
        ret = rpc.recvVar(state);
        if (ret < 0) {
            std::string msg("Error in RPC recv.\n");
            ygglog_error(msg.c_str());
            throw std::logic_error(msg);
        }
        
    }
  protected:
    YggRpcClient get_comm() const {
        dtype_t* dtype_out = NULL;
        dtype_t* dtype_in = NULL;
        dtype_out = create_dtype_json_object(0, NULL, NULL, true);
        if (dtype_out == NULL) {
            std::string msg("Failed to create output data type.\n");
            ygglog_error(msg.c_str());
            throw std::logic_error(msg);
        }
        dtype_in = create_dtype_json_object(0, NULL, NULL, true);
        if (dtype_in == NULL) {
            std::string msg("Failed to create input data type.\n");
            ygglog_error(msg.c_str());
            throw std::logic_error(msg);
        }
        WITH_GLOBAL_SCOPE(YggRpcClient rpc(server_name.c_str(),
                                           dtype_out, dtype_in));
        if (!(rpc.pi()->flags & COMM_FLAG_VALID)) {
            std::string msg("Failed to create RPC comm.\n");
            ygglog_error(msg.c_str());
            throw std::logic_error(msg);
        }
        return rpc;
    }
    virtual void prepareInput(rapidjson::Document& state) const {
        state.SetObject();
        for ( const auto& it : inputs ) {
            rapidjson::Value name(it.first.c_str(),
                                  it.first.size(),
                                  state.GetAllocator());
            state.AddMember(name.Move(), it.second,
                            state.GetAllocator());
        }
    }
    virtual void processOutput(rapidjson::Document& state) const {
        state.SetObject();
        double val = 0.0;
        for ( auto& it : outputs ) {
            if (!(state.HasMember(it.first) &&
                  state[it.first].IsDouble())) {
                std::string msg("Failed to get value from map for element '");
                msg += it.first + "'.\n";
                ygglog_error(msg.c_str());
                std::cerr << state << std::endl;
                throw std::logic_error(msg);
            }
            val = state[it.first].GetDouble();
            update(it.second, val);
        }
    }
    void do_operation() const override {

        rapidjson::Document state;
        
        // set up variables to be passed in and out of rpc.call
        prepareInput(state);

        // yggdrasil call
        YggRpcClient rpc = get_comm();
        call_model(rpc, state);
        
        // get output parameters
        processOutput(state);
    }

    std::string server_name;
    std::unordered_map<std::string, const double&> inputs;
    std::unordered_map<std::string, double*> outputs;
};

template<typename MODULE>
class ygg_direct_timesync_module : public ygg_direct_module<MODULE>
{
   public:
    ygg_direct_timesync_module(
        state_map const& input_quantities,
        state_map* output_quantities)
        : ygg_direct_module<MODULE>(input_quantities, output_quantities) {
        this->server_name = "timesync";
    }
    YggRpcClient get_comm() const {
        WITH_GLOBAL_SCOPE(YggRpcClient rpc(yggTimesync(this->server_name.c_str(), "hrs")));
        if (!(rpc.pi()->flags & COMM_FLAG_VALID)) {
            std::string msg("Failed to create RPC comm.\n");
            ygglog_error(msg.c_str());
            throw std::logic_error(msg);
        }
        return rpc;
    }
};

}  // namespace yggdrasilBML
#endif // WITH_YGGDRASIL
#endif
