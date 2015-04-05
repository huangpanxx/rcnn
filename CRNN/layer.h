#ifndef LAYER_H
#define LAYER_H

#include "common.h"
#include "memory.h"
#include "picojson.h"
#include "array_operator.h"

class layer {
public:
    layer();
    virtual void setup_block(){};
    virtual void setup_params(){};
    virtual bool begin_seq(){ return true; }
    virtual bool forward(){ return true; };
    virtual void backward(){};
    virtual void end_batch(int size){};

    float learn_rate(){ return m_learn_rate; }
    float momentum_decay(){ return m_momentum_decay; }
    void set_learn_rate(float lr){ this->m_learn_rate = lr; }
    void set_momentum_decay(float md) { this->m_momentum_decay = md; }

    virtual void save(std::ostream& os) { }
    virtual void load(std::istream& is) { }

    std::string name(){ return m_name; }
    void set_name(const std::string name){ m_name = name; }

    bool enable_bp() { return m_enable_bp; }
    void set_enable_bp(bool b) {
        m_enable_bp = b;
    }

    array_operator_ptr& arr_op() {
        return this->m_array_operator;
    }

private:
    float m_learn_rate;
    float m_momentum_decay;
    bool m_enable_bp;
    std::string m_name;
    array_operator_ptr m_array_operator;
};

class loss_layer: public layer {
public:
    virtual float loss() = 0;
};

class data_layer : public layer {
public:
    virtual int batch() = 0;
    virtual void move_to_next_batch() = 0;
};


typedef std::shared_ptr<layer> layer_ptr;

typedef std::function<layer_ptr(
    const picojson::value& config,
    const std::string& layer_name,
    block_factory& bf)> layer_factory_fn;

void register_layer_factory(std::string name, layer_factory_fn fn);
layer_factory_fn get_layer_factory(std::string name);

#define REGISTER_LAYER(name,fn) \
    bool name##_layer_create_fn() { \
    register_layer_factory(#name, fn); \
    return true; \
    }\
    static bool name##_layer_create_var = name##_layer_create_fn();

#endif
